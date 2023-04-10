# scollector

Collects tracks **you haven't heard yet** from [SoundCloud](https://soundcloud.com/discover) charts and downloads them using [yt-dlp](https://github.com/yt-dlp/yt-dlp).
Uses SQLite database engine and Python multiprocessing library.

The program works fine on GNU/Linux and Windows. macOS is also supported but not tested.

## Screenshots

![](https://hostux.pics/images/2023/04/08/scrathf7c2d159e4ba0f68.png)

On PC with **8** threads and a speed limit of 8 Mbps, downloading the playlists from scratch (the database was cleared) took 5 minutes 17 seconds.

Considering that more than half of the time was spent downloading files, scollector does its job **much faster** than any other single-threaded script or manual download and comparison :)

If there are no new songs in playlists, the check takes about 1-2 minutes. More threads - faster check.

## Installation

One of the best ways to safely install and use python packages on any OS is to use **[pipx](https://github.com/pypa/pipx)**:
- `pipx` package on Debian, Ubuntu, Mint, Kali, Fedora, Solus, EPEL
- `python-pipx` package on Arch Linux, Manjaro

Other GNU/Linux distros, Windows, macOS:

```bash
python -m pip install --user pipx

python -m pipx ensurepath
```

If you want to use pip instead, I **strongly** discourage you from doing so, as it almost always results in package conflicts on the system.

```bash
git clone https://github.com/meequrox/scollector.git

cd scollector

pipx install .
```

You also need to have `ffmpeg` installed for the thumbnail embedding to work.
I recommend using distro package manager on GNU/Linux, [brew](https://trac.ffmpeg.org/wiki/CompilationGuide/macOS#ffmpegthroughHomebrew) on macOS, [choco](https://community.chocolatey.org/packages/ffmpeg) on Windows.

## Usage

The most convenient way to use the program is to create an alias or shortcut. For example, I use this: `alias scc='scollector --country RU -vn --cleanup -o ~/Music --rate 12M -d 800'`

```bash
# Print help message
./scollector -h

# Common use
./scollector --country us --reset
./scollector --country FR --cleanup -n
./scollector --country ru -o ~/Music -r 80M -d 1000 --cleanup
```

Find codes for `--country` option [here](https://en.wikipedia.org/wiki/ISO_3166-2#Current_codes).
Note that there are some codes for which playlists do not exist. In this case, you will get `Unable to download JSON metadata: HTTP Error 404: Not Found` message from yt-dlp.

If `--output` option is not specified, the current working directory is used as destination directory.

scollector creates a new directory named *scollector_dl* in destination directory, so it won't harm your existing files.

## TODO
- [ ] Remove hardcode for charts and genres. The user himself specifies which charts and genres to download.
