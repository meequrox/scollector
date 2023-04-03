# scollector

Collects tracks **you haven't heard yet** from [SoundCloud](https://soundcloud.com/discover) charts and downloads them using [yt-dlp](https://github.com/yt-dlp/yt-dlp). Uses SQLite database engine and OpenMP multiprocessing API.

The app builds and works correctly on UNIX-like operating systems. If you like pain, you can try to compile under Windows, in theory there should be no runtime errors.

## Screenshots

![](https://img.tedomum.net/data/2023-04-03_18-33-12ae41.png)

## Installation

You need to install these libraries first:
- nlohmann/json (Debian: `nlohmann-json-dev`, Arch: `nlohmann-json`)
- SQLite3 (Debian: `libsqlite3-dev`, Arch: `sqlite`)
- OpenMP (Debian: `libomp-dev`, Arch: `openmp`)

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
