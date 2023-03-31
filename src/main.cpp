#include <iostream>

#include "args.hpp"
#include "downloader.hpp"

#ifdef _WIN32
    #define PIPE_TO_NULL " 2>NUL 1>NUL "
#else
    #define PIPE_TO_NULL " 2>/dev/null 1>/dev/null "
#endif

int main(int argc, char** argv) {
    if (system("yt-dlp -h" PIPE_TO_NULL) != 0) {
        std::cerr << "yt-dlp not found" << std::endl;
        std::cerr << "Install: https://github.com/yt-dlp/yt-dlp/wiki/Installation" << std::endl;
        return EXIT_FAILURE;
    }

    mqr::args args;
    if (!args.parse(argc, argv)) return EXIT_FAILURE;

    mqr::downloader dl;

    if (args.verbose) {
        std::cout << "yt-dlp version: ";
        std::cout.flush();
        system("yt-dlp --version");

        std::cout << "Output directory: " << args.output << std::endl;
    }

    if (!dl.download(args.verbose)) std::cout << "There were some errors while downloading" << std::endl;

    return EXIT_SUCCESS;
}
