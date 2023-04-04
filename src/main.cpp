#include <iostream>

#include "args.hpp"
#include "downloader.hpp"

int main(int argc, char** argv) {
    if (system("yt-dlp -h" PIPE_TO_NULL) != 0) {
        std::cerr << "yt-dlp not found" << std::endl;
        std::cerr << "Install: https://github.com/yt-dlp/yt-dlp/wiki/Installation" << std::endl;
        return EXIT_FAILURE;
    }

    mqr::args args;
    if (!args.parse(argc, argv)) return EXIT_FAILURE;

    const mqr::downloader dl({args.options, args.flags});

    if (args.flags.at("verbose")) {
        std::cout << args;

        std::cout << "yt-dlp version: ";
        std::cout.flush();
        system("yt-dlp --version");

        std::cout << dl;
    }

    if (dl.download() != 0) std::cout << "There were some errors while downloading" << std::endl;

    return EXIT_SUCCESS;
}
