#!/usr/bin/env python3

import argparse


def process_country(s):
    return str(s[:2]).lower()


class SCApplication:
    def __init__(self):
        self.name = "scollector"
        self.description = "Collects tracks you haven't heard yet from SoundCloud charts"

    def run(self):
        parser = argparse.ArgumentParser(prog=self.name, description=self.description)
        # Options
        parser.add_argument("--country", type=process_country, required=True, help="which country playlist to download (two-letter ISO 3166-2), e.g. RU, TH, MX")
        parser.add_argument("-o", "--output", type=str, help="path where scollector_dl directory will be created")
        parser.add_argument("-r", "--rate", type=str, help="maximum download rate in bytes per second, e.g. 50K, 4.2M")
        parser.add_argument("-d", "--duration", type=int, help="maximum song duration in seconds, e.g. 600 or 130")

        # Flags
        parser.add_argument("-v", "--verbose", action="store_true", help="more detailed log")
        parser.add_argument("--cleanup", action="store_true", help="all yt-dlp residual files will be deleted")
        parser.add_argument("-n", "--normalize", action="store_true", help="exclude unsafe characters from filenames")
        args = parser.parse_args()

        if args.verbose:
            print(str(args)[10:-1])


if __name__ == "__main__":
    app = SCApplication()
    app.run()
