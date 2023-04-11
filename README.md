## scollector

Collects tracks **you haven't heard yet** from [SoundCloud](https://soundcloud.com/discover) charts and downloads them using [yt-dlp](https://github.com/yt-dlp/yt-dlp).
Uses SQLite database engine and Python multiprocessing library.

![GNU/Linux](https://img.shields.io/badge/Linux-FCC624.svg?style=for-the-badge&logo=Linux&logoColor=black) ![Windows](https://img.shields.io/badge/Windows-0078D6.svg?style=for-the-badge&logo=Windows&logoColor=white) ![macOS](https://img.shields.io/badge/macOS-000000.svg?style=for-the-badge&logo=macOS&logoColor=white)

## Description

SoundCloud has charts that are separated by track genre. Collectively, they are **playlists**. Playlists are updated once a day: a dozen songs are usually added to the tops, and many times more to the new ones.

The idea is to have a program that will download from these playlists only those tracks that **the user has not yet heard**. It becomes something like a semi-automatic library, where the user only **has to sift out** the tracks he likes.

scollector does not require authorization and does its job much faster than any single-threaded scripts.

## Screenshots

![](https://img.tedomum.net/data/saymyname-bea14a.png)

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


## Upgrade

pipx remembers which directory scollector was installed from, so you can just do

```bash
# Cloned repo
cd scollector

git pull

pipx upgrade scollector
```

## Usage

The most convenient way to use the program is to create an alias or shortcut.

For example: `alias scc='scollector --country GB -vn --cleanup -o ~/Music --charts top trending --genres all-music world rock'`

```bash
# Print help message
./scollector -h

# Common use
./scollector --country US --charts top --genres world --reset
./scollector --country FR --charts trending top --genres rock deephouse hiphoprap --cleanup -n
./scollector --country RU --genres house metal --charts trending -o ~/Music -r 80M -d 1000 --cleanup
```

Find codes for `--country` option [here](https://en.wikipedia.org/wiki/ISO_3166-2#Current_codes). Note that there are some codes for which playlists do not exist. In this case, you will get `Unable to download JSON metadata: HTTP Error 404: Not Found` message from yt-dlp.

**Charts to choose from**: `top`, `trending`.

**Genres to choose from**: `all-music`, `alternativerock`, `ambient`, `audiobooks`, `business`, `classical`, `comedy`, `country`, `danceedm`, `dancehall`, `deephouse`, `disco`, `drumbass`, `dubstep`, `electronic`, `entertainment`, `folksingersongwriter`, `hiphoprap`, `house`, `indie`, `jazzblues`, `latin`, `learning`, `metal`, `newspolitics`, `piano`, `pop`, `rbsoul`, `reggae`, `reggaeton`, `religionspirituality`, `rock`, `science`, `soundtrack`, `sports`, `storytelling`, `techno`, `technology`, `trance`, `trap`, `triphop`, `world`.

scollector creates a new directory named *scollector_dl* in destination directory, so it won't harm your existing files.
If `--output` option is not specified, the current working directory is used as destination directory.
