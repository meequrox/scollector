#include "downloader.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

#define watch(os, x) os << std::left << std::setw(8) << #x ":" << x << std::endl;

namespace mqr {
using json = nlohmann::json;

downloader::downloader(fs::path output) : lang("ru"), dest_dir(output) {}

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

inline static bool is_compatible_extension(std::vector<std::string>& exts, fs::path file) {
    std::string file_ext = file.extension().generic_string();
    std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

    return std::find(exts.begin(), exts.end(), file_ext) != exts.end();
}

void remove_images_in_dir(const downloader& obj) {
    std::vector<std::string> extensions = {".png", ".jpg", ".part"};

    for (const fs::directory_entry& entry : fs::directory_iterator(obj.get_destination())) {
        if (entry.is_regular_file() && is_compatible_extension(extensions, entry.path())) {
            fs::remove(entry.path());
        }
    }
}

// TODO: add user-specific options to args parser
constexpr char o_gen[] = "-i -r 8M --match-filter \"duration<=?600\" ";
constexpr char o_prg[] = "-q --progress --no-warnings ";
constexpr char o_prgt[] = "--progress-template \"'%(info.title)s' %(progress._default_template)s\" ";
constexpr char o_out[] = "-q --progress --no-warnings ";
constexpr char o_pp[] = "--embed-thumbnail --embed-metadata ";

bool downloader::download(bool cleanup) {
    fs::path prev_path = fs::current_path();
    fs::current_path(dest_dir);

    bool success = true;
    constexpr char baseurl[] = "https://soundcloud.com/discover/sets/charts-";

    for (const auto& chart : charts) {
        if (!success) break;

        for (const auto& genre : genres) {
            std::string url = baseurl + chart + ":" + genre + ":" + lang + " ";
            std::string cmd = "yt-dlp " + url + o_gen + "--dump-json ";

            std::cout << std::endl << chart << ":" << genre << " - Loading JSON info" << std::endl;

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

    if (cleanup) remove_images_in_dir(*this);

    // TODO: Normalize songs filenames (*.mp3 *.wav *.aac)

    fs::current_path(prev_path);
    return success;
}

std::ostream& operator<<(std::ostream& os, const downloader& obj) {
    os << "Downloader output directory: " << obj.dest_dir << std::endl;

    os << "Downloader charts:";
    for (const auto& chart : obj.charts) {
        os << " " << chart;
    }
    os << std::endl;

    os << "Downloader genres:";
    for (const auto& genre : obj.genres) {
        os << " " << genre;
    }
    os << std::endl;

    os << "Downloader options:" << std::endl;
    watch(os, o_gen);
    watch(os, o_prg);
    watch(os, o_prgt);
    watch(os, o_out);
    watch(os, o_pp);

    return os;
}

const std::vector<std::string> downloader::get_charts() const { return charts; }

const std::vector<std::string> downloader::get_genres() const { return genres; }

const fs::path downloader::get_destination() const { return dest_dir; }
}  // namespace mqr
