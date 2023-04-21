import argparse
from .Downloader import Downloader


class SCApplication:

    def __init__(self):
        self.__name = "scollector"
        self.__description = "Collects tracks you haven't heard yet from SoundCloud charts"
        self.__epilog = "example: scollector --country NL --charts top trending --genres pop rock"

        self.__charts = ["top", "trending"]
        self.__genres = [
            "all-music", "alternativerock", "ambient", "audiobooks", "business",
            "classical", "comedy", "country", "danceedm", "dancehall",
            "deephouse", "disco", "drumbass", "dubstep", "electronic",
            "entertainment", "folksingersongwriter", "hiphoprap", "house",
            "indie", "jazzblues", "latin", "learning", "metal", "newspolitics",
            "piano", "pop", "rbsoul", "reggae", "reggaeton",
            "religionspirituality", "rock", "science", "soundtrack", "sports",
            "storytelling", "techno", "technology", "trance", "trap", "triphop",
            "world"
        ]
        self.__countries = [
            'ad', 'ae', 'af', 'ag', 'ai', 'al', 'am', 'ao', 'aq', 'ar', 'as',
            'at', 'au', 'aw', 'ax', 'az', 'ba', 'bb', 'bd', 'be', 'bf', 'bg',
            'bh', 'bi', 'bj', 'bl', 'bm', 'bn', 'bo', 'bq', 'br', 'bs', 'bt',
            'bv', 'bw', 'by', 'bz', 'ca', 'cc', 'cd', 'cf', 'cg', 'ch', 'ci',
            'ck', 'cl', 'cm', 'cn', 'co', 'cr', 'cu', 'cv', 'cw', 'cx', 'cy',
            'cz', 'de', 'dj', 'dk', 'dm', 'do', 'dz', 'ec', 'ee', 'eg', 'eh',
            'er', 'es', 'et', 'fi', 'fj', 'fk', 'fm', 'fo', 'fr', 'ga', 'gb',
            'gd', 'ge', 'gf', 'gg', 'gh', 'gi', 'gl', 'gm', 'gn', 'gp', 'gq',
            'gr', 'gs', 'gt', 'gu', 'gw', 'gy', 'hk', 'hm', 'hn', 'hr', 'ht',
            'hu', 'id', 'ie', 'il', 'im', 'in', 'io', 'iq', 'ir', 'is', 'it',
            'je', 'jm', 'jo', 'jp', 'ke', 'kg', 'kh', 'ki', 'km', 'kn', 'kp',
            'kr', 'kw', 'ky', 'kz', 'la', 'lb', 'lc', 'li', 'lk', 'lr', 'ls',
            'lt', 'lu', 'lv', 'ly', 'ma', 'mc', 'md', 'me', 'mf', 'mg', 'mh',
            'mk', 'ml', 'mm', 'mn', 'mo', 'mp', 'mq', 'mr', 'ms', 'mt', 'mu',
            'mv', 'mw', 'mx', 'my', 'mz', 'na', 'nc', 'ne', 'nf', 'ng', 'ni',
            'nl', 'no', 'np', 'nr', 'nu', 'nz', 'om', 'pa', 'pe', 'pf', 'pg',
            'ph', 'pk', 'pl', 'pm', 'pn', 'pr', 'ps', 'pt', 'pw', 'py', 'qa',
            're', 'ro', 'rs', 'ru', 'rw', 'sa', 'sb', 'sc', 'sd', 'se', 'sg',
            'sh', 'si', 'sj', 'sk', 'sl', 'sm', 'sn', 'so', 'sr', 'ss', 'st',
            'sv', 'sx', 'sy', 'sz', 'tc', 'td', 'tf', 'tg', 'th', 'tj', 'tk',
            'tl', 'tm', 'tn', 'to', 'tr', 'tt', 'tv', 'tw', 'tz', 'ua', 'ug',
            'um', 'us', 'uy', 'uz', 'va', 'vc', 've', 'vg', 'vi', 'vn', 'vu',
            'wf', 'ws', 'ye', 'yt', 'za', 'zm', 'zw'
        ]

    def run(self):
        """Parse CLI arguments then run downloader"""

        def process_charts(chart: str) -> str:
            """Process chart from CLI argument"""

            c = chart.lower()
            if c not in self.__charts:
                raise ValueError(
                    f"Country: expected chart 'top' or 'trending', got {c}")
            return c

        def process_genres(genre: str) -> str:
            """Process genre from CLI argument"""

            g = genre.lower()
            if g not in self.__genres:
                raise ValueError(
                    f"Country: expected chart 'top' or 'trending', got {g}")
            return g

        def process_country(country: str) -> str:
            """Process country code from CLI argument"""

            c = country.lower()
            if c not in self.__countries:
                raise ValueError(
                    f"Country: expected two-letter ISO 3166-2 code, got {c}")
            return c

        parser = argparse.ArgumentParser(prog=self.__name,
                                         description=self.__description,
                                         epilog=self.__epilog)

        # Options
        parser.add_argument("--charts",
                            type=process_charts,
                            nargs="+",
                            required=True,
                            help="Charts to download: top, trending")
        parser.add_argument(
            "--genres",
            type=process_genres,
            nargs="+",
            required=True,
            help=
            "Genres to download from charts: all-music, world, hiphoprap, rock, ... (see README)"
        )
        parser.add_argument(
            "--country",
            type=process_country,
            metavar="CODE",
            required=True,
            help=
            "which country playlist to download (two-letter ISO 3166-2), e.g. RU, TH, MX"
        )
        parser.add_argument(
            "-o",
            "--output",
            type=str,
            metavar="PATH",
            help="path where scollector_dl directory will be created")
        parser.add_argument(
            "--rate",
            type=str,
            metavar="BPS",
            help="maximum download rate in bytes per second, e.g. 50K, 4.2M")
        parser.add_argument(
            "-d",
            "--duration",
            type=int,
            metavar="SECONDS",
            help="maximum song duration in seconds, e.g. 600 or 130")

        # Flags
        flags = parser.add_argument_group("flags")
        flags.add_argument("-v",
                           "--verbose",
                           action="store_true",
                           help="more detailed log")
        flags.add_argument(
            "--reset",
            action="store_true",
            help="Delete ALL database entries before starting the download")
        flags.add_argument("--cleanup",
                           action="store_true",
                           help="all yt-dlp residual files will be deleted")
        flags.add_argument("-n",
                           "--normalize",
                           action="store_true",
                           help="exclude unsafe characters from filenames")
        args = parser.parse_args()

        args.charts.sort()
        args.genres.sort()
        dl = Downloader(parser.prog, args)

        if args.verbose:
            dl.info_print()

        # Download playlists
        if not dl.download():
            print("There were some errors while downloading")
