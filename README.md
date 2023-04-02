# scollector

Collects tracks **you haven't heard yet** from [SoundCloud](https://soundcloud.com/discover) charts and downloads them using [yt-dlp](https://github.com/yt-dlp/yt-dlp).

Currently only UNIX-like OSes are supported, feel free to [add support for Windows](https://github.com/meequrox/scollector/blob/05513a50db1a65a7b4943441baa2f992c8c5b428/src/downloader.cpp#L14).

## Screenshots

![Verbose](https://hostux.pics/images/2023/04/02/eAhkXhk22799c6a9d0a63f9.png)

## Installation

You need to install these libraries first:
- nlohmann/json (Debian: `nlohmann-json-dev`, Arch: `nlohmann-json`)
- SQLite3 (Debian: `libsqlite3-dev`, Arch: `sqlite`)

```bash
git clone https://github.com/meequrox/scollector.git

cd scollector/build

cmake ..

make
```

## Usage

```bash
# Print help message
./scollector -h

# Common use
./scollector -l us

./scollector -l fr -nc

./scollector -l ru -cn -o ~/Music -r 80M -d 1000
```

If -o option is not specified, the current working directory is used as destination directory.

scollector creates a new directory named *scollector_dl* in destination directory, so it won't harm your existing files.
