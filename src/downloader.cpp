#include "downloader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

// TODO: add user-specific options to args parser
#define OPTS_GEN "--ignore-errors "
#define OPTS_VID "--match-filter \"duration<=?600\" "
#define OPTS_DL "-r 8M "
#define OPTS_FS "--force-overwrites -o \"%(uploader)s - %(title)s.%(ext)s\" --windows-filenames "
#define OPTS_VRB "--no-warnings "
#define OPTS_PP "--embed-thumbnail --add-metadata "

#define OPTS_ALL OPTS_GEN OPTS_VID OPTS_DL OPTS_FS OPTS_VRB OPTS_PP

#define SC_BASEURL "https://soundcloud.com/discover/sets/charts-"

namespace mqr {
using json = nlohmann::json;
namespace fs = std::filesystem;

downloader::downloader() : lang("ru") {}

bool downloader::download(bool verbose) {
    if (verbose) {
        print_charts();
        print_genres();
    }

    for (const auto& chart : charts) {
        fs::create_directory(chart);
        fs::current_path(chart);

        for (const auto& genre : genres) {
            fs::create_directory(genre);
            fs::current_path(genre);

            std::string url = SC_BASEURL + chart + ":" + genre + ":" + lang + " ";
            std::string cmd = "yt-dlp " + url + OPTS_ALL + "--dump-json ";

            std::cout << std::endl << "Loading " << chart << ":" << genre << ":" << std::endl;
            if (verbose) std::cout << cmd << std::endl;

            FILE* pipe = popen((cmd + PIPE_TO_STDOUT).c_str(), "r");
            if (!pipe) {
                std::cerr << "Error: failed to open pipe" << std::endl;
                std::cerr << "Command: " << cmd << std::endl;
                return false;
            }

            std::string output;

            char buffer[1024] = {'\0'};
            while (fgets(buffer, 1024, pipe)) output += buffer;
            pclose(pipe);

            // Fix JSON
            size_t index = 0;
            while (true) {
                index = output.find("}}\n{", index);
                if (index == std::string::npos) break;

                output.replace(index, std::size("}}\n{\"id\":"), "}}, {\"id\":");
                index += std::size("}}\n{\"id\":");
            }
            output = "[" + output + "]";

            json songs = json::parse(output);

            // TEMP
            std::ofstream json_dump(chart + "." + genre);
            json_dump << songs.dump();
            json_dump.close();

            url.clear();
            for (const auto& song : songs) {
                // TODO: check song id in DB

                url += song["webpage_url"];
                url += " ";
            }

            cmd = "yt-dlp " + url + OPTS_ALL;
            system((cmd + PIPE_TO_STDOUT).c_str());

            std::cout << "Finish " << chart << ":" << genre << " download" << std::endl;
            fs::current_path("..");
        }
        fs::current_path("..");
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
