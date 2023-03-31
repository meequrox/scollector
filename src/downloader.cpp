#include "downloader.hpp"

#include <iostream>

// TODO: add user-specific options to args parser
#define DL_OPTS_GENERAL "--ignore-errors --use-extractors soundcloud "
#define DL_OPTS_VIDEO "--match-filter \"duration<=?600\" "
#define DL_OPTS_DOWNLOAD "-r 8M "
// TODO: try --restrict-filenames
#define DL_OPTS_FS "--force-overwrites -o \"%(uploader)s - %(title)s.%(ext)s\" --windows-filenames "
#define DL_OPTS_THUMB "--write-thumbnail "
#define DL_OPTS_VERBOSE "--no-warnings "
#define DL_OPTS_PP "--embed-thumbnail --add-metadata "

#define DL_OPTS_ALL \
    DL_OPTS_GENERAL DL_OPTS_VIDEO DL_OPTS_DOWNLOAD DL_OPTS_FS DL_OPTS_THUMB DL_OPTS_VERBOSE DL_OPTS_PP

#define SC_BASEURL "https://soundcloud.com/discover/sets/charts-"
#define PIPE_TO_STDOUT " 2>&1 "

namespace mqr {
downloader::downloader() : lang("ru") {}

bool downloader::download() {
    for (const auto& chart : charts) {
        for (const auto& genre : genres) {
            std::string url = SC_BASEURL + chart + ":" + genre + ":" + lang + " ";
            std::string cmd = "yt_dlp " + url + DL_OPTS_ALL;

            // IN PROGRESS
            std::cout << cmd << std::endl << std::endl;
            //    FILE* pipe = popen(cmd PIPE_TO_NULL, "r");
            //    if (!pipe) {
            //        std::cerr << "Error: failed to open pipe" << std::endl;
            //        std::cerr << "Command: " << cmd << std::endl;
            //        return EXIT_FAILURE;
            //    }
            //    std::string output;
            //    char buffer[1024] = {'\0'};
            //    while (fgets(buffer, 1024, pipe)) output += buffer;
            //    pclose(pipe);
            //    std::cout << output;
        }
    }

    return true;
}

void downloader::print_charts() {
    std::cout << "Downloader charts:";
    for (const auto& chart : charts) {
        std::cout << " " << chart;
    }
    std::cout << std::endl;
}

void downloader::print_genres() {
    std::cout << "Downloader genres:";
    for (const auto& genre : genres) {
        std::cout << " " << genre;
    }
    std::cout << std::endl;
}
}  // namespace mqr
