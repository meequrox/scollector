import sys
import os
import multiprocessing

import yt_dlp
from .Database import Database


def playlist_process_init(lock_db, counter_total):
    """Global shared objects initializer"""

    global sharedLockDB  # multiprocessing.Lock
    sharedLockDB = lock_db

    global sharedCounterTotal  # multiprocessing.Value
    sharedCounterTotal = counter_total


class Downloader:
    """
    Custom wrapper around yt-dlp interface.
    Downloads charts from SoundCloud.
    """

    def __init__(self, name: str, params):

        def get_data_path() -> str:
            """
            Returns the path to store data on the current OS.
            Should work correctly on GNU/Linux, Windows and macOS.
            """

            if sys.platform.startswith("win32"):
                path = os.getenv('APPDATA',
                                 default=os.path.join(os.path.expanduser('~'),
                                                      "AppData", "Roaming"))
            elif sys.platform.startswith("darwin"):
                path = os.path.join(os.path.expanduser('~'), "Library")
            else:
                path = os.getenv('XDG_DATA_HOME',
                                 default=os.path.join(os.path.expanduser('~'),
                                                      '.local', 'share'))

            return path

        # From yt-dlp/yt_dlp/__init__.py
        def rate_limit_stoi(value_str: str) -> str | None:
            """Converts string download rate with K, M, G, ... letters to int"""

            if value_str is None:
                return None

            numeric_limit = yt_dlp.utils.parse_bytes(value_str)
            if numeric_limit is None:
                raise ValueError(f"invalid rate limit '{value_str}' given")

            return numeric_limit

        self.__country: str = params.country
        self.__maxDuration: int = params.duration or -1
        self.__maxRate: int = rate_limit_stoi(params.rate) or -1

        self.__resetDB: bool = params.reset
        self.__cleanup: bool = params.cleanup
        self.__normalize: bool = params.normalize

        self.__databasePath: str = os.path.join(get_data_path(), name)
        os.makedirs(self.__databasePath, exist_ok=True)
        self.__databasePath = os.path.join(self.__databasePath, "main.db")

        self.__outputPath: str = params.output or os.path.abspath(os.getcwd())
        self.__outputPath = os.path.join(self.__outputPath, f"{name}_dl")

        self.__playlists: list[str] = []
        for c in params.charts:
            for g in params.genres:
                self.__playlists.append(f"{c}:{g}")

        self.__ydl_opts: dict = {
            "quiet": True,
            "no_warnings": True,
            "outtmpl": "%(uploader)s - %(title)s.%(id)s.%(ext)s",
            "windowsfilenames": True,
            "ignoreerrors": True,
            "consoletitle": True,
            "overwrites": False,
            "writethumbnail": False,
            "cachedir": False,
            "postprocessors": [
                {
                    "key": "FFmpegMetadata",
                    "add_metadata": True
                },
                {
                    "key": "EmbedThumbnail"
                },
            ],
            "geo_bypass": True,
            "noprogress": True,
            # "progress_template": {"download": "'%(info.title)s' %(progress._default_template)s"}
        }
        if self.__maxRate > 0:
            self.__ydl_opts["ratelimit"] = self.__maxRate

    def info_print(self):
        """Prints information about involved paths and downloaded playlists"""

        print(f"{self.__class__.__name__} options:")
        print(f" {'{0: <32}'.format('Country code:')} {self.__country}")
        if self.__maxDuration > 0:
            print(
                f" {'{0: <32}'.format('Maximum duration:')} {self.__maxDuration}"
            )
        if self.__maxRate > 0:
            print(
                f" {'{0: <32}'.format('Maximum download rate:')} {self.__maxRate} B/s"
            )
        print(f" {'{0: <32}'.format('Reset database:')} {self.__resetDB}")
        print(
            f" {'{0: <32}'.format('Cleanup after download:')} {self.__cleanup}")
        print(
            f" {'{0: <32}'.format('Normalize filenames:')} {self.__normalize}")
        print(f" {'{0: <32}'.format('Database path:')} {self.__databasePath}")
        print(
            f" {'{0: <32}'.format('Output directory path:')} {self.__outputPath}"
        )
        print(f" {'{0: <32}'.format('Playlists:')}", end=" ")
        for p in self.__playlists:
            print(p, end=" ")
        print()
        print()

    @staticmethod
    def __is_compatible_ext(exts: list[str], path: str) -> bool:
        """Determines if extension of specified file is in the list of matches"""

        p, ext = os.path.splitext(path)
        return ext.lower() in exts

    def __files_cleanup(self):
        """Finds any files known to be download artifacts and then removes them"""

        if not self.__cleanup:
            return

        exts = [".png", ".jpg", ".part", ".ytdl"]
        for f in os.listdir(self.__outputPath):
            if os.path.isfile(f) and self.__is_compatible_ext(exts, f):
                os.remove(f)

    def __files_normalize(self):
        """Finds song files and then renames them to filesystem-safe variant"""

        if not self.__normalize:
            return

        def filter_filename(fn: str) -> str:
            """
            Removes characters potentially dangerous for various filesystems from the filename.
            Leaves only letters of the Russian alphabet and ASCII characters.
            """

            new = ""
            for c in fn:
                rus: bool = ("а" <= c <= "я") or ("А" <= c <= "Я") or c in "ёЁ"
                _ascii: bool = (" " <= c <= "}") and c not in "'?@`$"
                if rus or _ascii:
                    new += c

            new = new.strip()
            while new.find("  ") != -1:
                new = new.replace("  ", " ")

            # Each song has its own unique id in filename, so filtered filename cannot be empty
            return new

        exts = [".mp3", ".wav", ".aac"]
        for f in os.listdir(self.__outputPath):
            if os.path.isfile(f) and self.__is_compatible_ext(exts, f):
                fn_from: str = os.path.basename(f)
                fn_to: str = filter_filename(fn_from)
                if fn_from != fn_to:
                    os.rename(fn_from, fn_to)

    def __playlist_get_songs(self, playlist: str) -> dict[int, str]:
        """Loads JSON information for given playlist and creates {id: url} dictionary of compatible entries"""

        url = f"https://soundcloud.com/discover/sets/charts-{playlist}:{self.__country}"
        songs: dict[int, str] = {}

        with yt_dlp.YoutubeDL(self.__ydl_opts) as ydl:
            info = ydl.sanitize_info(ydl.extract_info(url, download=False))

        if info is not None and "entries" in info.keys():
            for songInfo in info["entries"]:
                try:
                    if self.__maxDuration <= 0:
                        # If max_duration is not set
                        songs[songInfo["id"]] = songInfo["webpage_url"]
                    elif songInfo["duration"] <= self.__maxDuration:
                        # If max_duration is set and the song is shorter
                        songs[songInfo["id"]] = songInfo["webpage_url"]
                except KeyError:
                    # If entry does not have some keys, do nothing
                    pass
        else:
            print(f"{url}: Bad JSON")

        return songs

    def __links_download(self, urls: list[str]):
        """Downloads file for each URL in list"""

        with yt_dlp.YoutubeDL(self.__ydl_opts) as ydl:
            code: int = ydl.download(urls)
        if code:
            print("yt-dlp failed to download this playlist")

    def playlist_process(self, playlist: str):
        """
        This method is NOT intended to be called directly, but cannot be private due to the multiprocessing map.

        To download, use the download() method.
        """

        def get_process_num() -> int:
            """
            Determines which core the process is running on and returns its number (from 1 to ...).

            Returns 0 if running on single core (MainProcess).
            """

            name = str(multiprocessing.current_process().name)
            id_list = list(filter(str.isdigit, name))

            return int(id_list[0]) if len(id_list) else 0

        # Separate connection for each process
        db = Database(self.__databasePath)
        if not db.is_open():
            return False

        pnum: int = get_process_num()
        playlist_fstr: str = "{0: <32}".format(playlist)
        print(f"P{pnum}: {playlist_fstr} - Downloading JSON")

        songs: dict[int, str] = self.__playlist_get_songs(playlist)
        print(f"P{pnum}: {playlist_fstr} + JSON downloaded")

        urls: list[str] = []
        ids: list[int] = []

        # Only 1 process at a time can perform R/W operations with database
        sharedLockDB.acquire()  # Lock database mutex

        for song_id, song_url in songs.items():
            if not db.id_exists(song_id):
                ids.append(song_id)
                urls.append(song_url)

        db.transaction_begin()
        for song_id in ids:
            db.insert(song_id)
        db.transaction_end()

        sharedLockDB.release()  # Unlock database mutex
        db.close()

        count = len(urls)
        if count:
            print(f"P{pnum}: {playlist_fstr[:-1]} -> Downloading {count} songs")

            # Can do I/O without locking because there are no filenames with same id
            self.__links_download(urls)

            with sharedCounterTotal.get_lock():
                sharedCounterTotal.value += count

            print(f"P{pnum}: {playlist_fstr[:-1]} <- {count} songs downloaded")

    def download(self):
        """Start downloading playlists"""

        db = Database(self.__databasePath)
        if self.__resetDB:
            db.clear()

        print(f"{db.rows()} IDs in the database")
        db.close()

        # Remember OLDPWD then cd into output directory
        prev_path: str = os.path.abspath(os.getcwd())
        os.makedirs(self.__outputPath, exist_ok=True)
        os.chdir(self.__outputPath)

        # Shared objects that initialize global variables in playlist_process_init() function
        local_lock_db = multiprocessing.Lock()
        local_counter_total = multiprocessing.Value("i", 0)

        # Multiprocessing: each process downloads its own playlist. No GIL.
        pool = multiprocessing.Pool(initializer=playlist_process_init,
                                    initargs=(local_lock_db,
                                              local_counter_total))
        pool.map(self.playlist_process, self.__playlists)
        pool.close()
        pool.join()

        self.__files_cleanup()
        self.__files_normalize()

        files_count: int = len([f for f in os.listdir() if os.path.isfile(f)])
        print(
            f"\nTotal: {local_counter_total.value or 'no'} files should have been downloaded, {files_count or 'no'} files in output directory"
        )

        # cd -
        os.chdir(prev_path)
        return True
