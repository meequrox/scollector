#include "downloader.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace mqr {
using json = nlohmann::json;

downloader::downloader() : lang("ru") {}

static std::string make_json_array(const std::string& str) {
    size_t index = 0;
    std::string ja_str = str;

    const std::string substr = "}}\n{\"id\":";
    const std::string new_substr = "}},{\"id\":";
    const size_t offset = substr.length();
    while (true) {
        index = ja_str.find(substr, index);
        if (index == std::string::npos) break;

        ja_str.replace(index, offset, new_substr);
        index += offset;
    }

    return "[" + ja_str + "]";
}

static void dump_json_to_file(const json& json, const std::string& filename) {
    std::ofstream file(filename);
    file << json.dump();
    file.close();
}

std::string read_from_pipe(const std::string& cmd) {
    const std::string cmd_mod = cmd + PIPE_TO_STDOUT;
    FILE* pipe = popen(cmd_mod.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: failed to open pipe" << std::endl;
        std::cerr << "Command: " << cmd_mod << std::endl;

        return "";
    }

    std::string dest;
    char buffer[1024] = {'\0'};
    while (fgets(buffer, 1024, pipe)) dest += buffer;
    pclose(pipe);

    return dest;
}

bool downloader::download(fs::path output, bool verbose) {
    if (verbose) {
        std::cout << "Downloader output directory: " << output << std::endl;
        print_charts();
        print_genres();
    }

    // TODO: add user-specific options to args parser
    constexpr char o_gen[] = "-i -r 8M --match-filter \"duration<=?600\" ";
    constexpr char o_prg[] = "-q --progress --no-warnings ";
    constexpr char o_prgt[] = "--progress-template \"'%(info.title)s' %(progress._default_template)s\" ";
    constexpr char o_out[] = "-q --progress --no-warnings ";
    constexpr char o_pp[] = "--embed-thumbnail --embed-metadata ";

    fs::path prev_path = fs::current_path();
    fs::current_path(output);

    bool success = true;
    constexpr char baseurl[] = "https://soundcloud.com/discover/sets/charts-";

    for (const auto& chart : charts) {
        if (!success) break;

        for (const auto& genre : genres) {
            std::string url = baseurl + chart + ":" + genre + ":" + lang + " ";
            std::string cmd = "yt-dlp " + url + o_gen + "--dump-json ";

            std::cout << std::endl << chart << ":" << genre << " - Loading JSON info" << std::endl;
            if (verbose) std::cout << cmd << std::endl;

            std::string json_str = read_from_pipe(cmd);
            if (json_str.empty()) {
                success = false;
                break;
            }

            json_str = make_json_array(json_str);
            json songs = json::parse(json_str);
            dump_json_to_file(songs, chart + "." + genre);  // TEMP

            url.clear();
            size_t count = 0;
            for (const auto& song : songs) {
                // TODO: check song id in DB

                url += song["webpage_url"];
                url += " ";
                count++;
            }

            std::cout << chart << ":" << genre << " - Downloading " << count << " songs..." << std::endl;
            if (count) {
                cmd = "yt-dlp " + url + o_gen + o_prg + o_prgt + o_out + o_pp;
                system((cmd + PIPE_TO_STDOUT).c_str());
            }

            std::cout << chart << ":" << genre << " - Download finished (" << count << ")" << std::endl;
        }
    }

    // TODO: Delete images artifacts
    // TODO: Normalize songs filenames (*.mp3 *.wav *.aac)

    fs::current_path(prev_path);
    return success;
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
