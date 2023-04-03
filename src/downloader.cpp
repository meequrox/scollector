#include "downloader.hpp"

#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
// #include <omp.h>

#include "db.hpp"

#define watch(os, x) os << std::left << std::setw(24) << #x ":" << x << std::endl;

namespace mqr {
static fs::path get_data_path() {
    // Currently UNIX-like only

    fs::path p;
    const char* xdg_data_path = std::getenv("XDG_DATA_HOME");
    if (xdg_data_path)
        p = xdg_data_path;
    else
        p = std::string("/home/") + std::getenv("USER") + "/.local/share";

    return p;
}

downloader::downloader(const fs::path& output, const std::string& rate_limit,
                       const std::string& duration_limit)
    : dest_dir(output), max_rate(rate_limit), max_duration(duration_limit) {
    const std::vector<std::string> charts = {"top", "trending"};
    const std::vector<std::string> genres = {"danceedm", "electronic", "hiphoprap", "house"};
    for (const auto& c : charts) {
        for (const auto& g : genres) {
            playlists.push_back(c + ":" + g);
        }
    }

    const fs::path data_dir = get_data_path() / "scollector";
    fs::create_directories(data_dir);

    db_path = data_dir / "main.db";
}

static std::string make_json_array(const std::string& str) {
    size_t index = 0;
    std::string ja_str = str;

    const std::string substr = "}}\n{\"id\":";
    const std::string new_substr = "}},{\"id\":";
    const size_t offset = substr.size();

    while (true) {
        index = ja_str.find(substr, index);
        if (index == std::string::npos) break;

        ja_str.replace(index, offset, new_substr);
        index += offset;
    }

    return "[" + ja_str + "]";
}

static std::string read_from_pipe(const std::string& cmd) {
    const std::string cmd_mod = cmd + PIPE_TO_STDOUT;
    FILE* pipe = popen(cmd_mod.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: failed to open pipe" << std::endl;
        std::cerr << "Command: " << cmd_mod << std::endl;

        return "";
    }

    std::string dest;
    char buffer[8192] = {0};
    while (fgets(buffer, 8192, pipe)) dest += buffer;
    pclose(pipe);

    return dest;
}

inline static bool is_compatible_extension(const std::vector<std::string>& exts, const fs::path file) {
    std::string file_ext = file.extension().generic_string();
    std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

    return std::find(exts.begin(), exts.end(), file_ext) != exts.end();
}

void remove_images_in_dir(const fs::path dir) {
    const std::vector<std::string> extensions = {".png", ".jpg", ".part"};

    for (const fs::directory_entry& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && is_compatible_extension(extensions, entry.path())) {
            fs::remove(entry.path());
        }
    }
}

inline static bool isprint_ru(const char c) {
    const int code = (int)(unsigned char)c;

    // lowercase + uppercase: абвгдеёжзийклмноп || рстуфхцчшщъыьэюя
    return (code >= 176 && code <= 191) || (code >= 128 && code <= 175);
}

static std::string filter_filename(const std::string& str) {
    std::string fn;
    const size_t len = str.size();

    int i = 0;
    while (str[i] == ' ') i++;  // ltrim whitespace

    for (; i < len; i++) {
        int code = (int)(unsigned char)str[i];

        // Exclude ' ? @ `
        if (code == 39 || code == 63 || code == 64 || code == 96) continue;

        if ((code == 208 || code == 209) && i + 1 < len && isprint_ru(str[i + 1])) {
            // Next char is in RU alphabet
            fn += str[i];
            fn += str[i + 1];
            i++;
        } else if (code == 32 && i > 0 && fn[fn.size() - 1] != 32) {
            // Space: avoid doubles
            fn += str[i];
        } else if (code >= 33 && code <= 127) {
            // Other ASCII chars
            fn += str[i];
        }
    }

    // Bad filename (.ext)
    if (fn.size() <= 4) {
        srand(time(nullptr));
        fn = std::to_string(rand()) + fn;
    }

    return fn;
}

static void normalize_filenames(const fs::path dir) {
    const std::vector<std::string> extensions = {".mp3", ".wav", ".aac"};

    for (const fs::directory_entry& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && is_compatible_extension(extensions, entry.path())) {
            const std::string from = entry.path().filename().generic_string();
            std::string to = filter_filename(from);

            if (from != to) fs::rename(from, to);
        }
    }
}

constexpr char o_gen[] = "-i ";
constexpr char o_prg[] = "-q --progress --no-warnings ";
constexpr char o_prgt[] = "--progress-template \"'%(info.title)s' %(progress._default_template)s\" ";
constexpr char o_out[] = "-q --progress --no-warnings ";
constexpr char o_pp[] = "--embed-thumbnail --embed-metadata ";

bool downloader::download(const std::string& country, bool cleanup, bool normalize) const {
    sqlite3* db = nullptr;
    if (!db_open(db_path.c_str(), &db)) return false;

    const fs::path prev_path = fs::current_path();

    bool success = true;
    bool dest_dir_created = false;
    constexpr char baseurl[] = "https://soundcloud.com/discover/sets/charts-";

    // #pragma omp parallel {
    // #pragma omp for
    for (const auto& p : playlists) {
        std::string url = baseurl + p + ":" + country + " ";
        std::string cmd = "yt-dlp " + url + o_gen + "--dump-json ";
        if (!max_duration.empty()) cmd += "--match-filter \"duration<=?" + max_duration + "\" ";
        if (!max_rate.empty()) cmd += "-r " + max_rate;

        std::cout << std::endl << p << " - Downloading JSON" << std::endl;
        const std::string json_str = read_from_pipe(cmd);
        if (json_str.empty()) {
            // #pragma omp atomic
            success = false;
            break;
        }

        const nlohmann::json songs = nlohmann::json::parse(make_json_array(json_str));

        // #pragma omp critical(cout)
        std::cout << p << " - JSON downloaded" << std::endl;

        size_t count = 0;
        std::vector<uint32_t> ids;
        url.clear();
        for (const auto& song : songs) {
            uint32_t id = std::stoull(static_cast<std::string>(song["id"]));

            // lock mutex_db
            if (!db_id_exists(id, db)) {
                ids.push_back(id);
                count++;

                url += song["webpage_url"];
                url += " ";
            }
            // free mutex_db
        }

        // #pragma omp critical(cout)
        std::cout << p << " - Downloading " << count << " songs..." << std::endl;

        if (count) {
            cmd = "yt-dlp " + url + o_gen + o_prg + o_prgt + o_out + o_pp;
            if (!max_duration.empty()) cmd += "--match-filter \"duration<=?" + max_duration + "\" ";
            if (!max_rate.empty()) cmd += "-r " + max_rate;

            // #pragma omp critical(fs)
            // {
            fs::create_directory(dest_dir);
            fs::current_path(dest_dir);
            dest_dir_created = true;
            system((cmd + PIPE_TO_STDOUT).c_str());
            // }

            // lock mutex_db
            db_begin(db);
            for (const auto& id : ids) db_insert(db, id);
            db_end(db);
            // free mutex_db
        }

        // #pragma omp critical(cout)
        std::cout << p << " - Download finished (" << count << ")" << std::endl;
    }
    // }

    if (cleanup && dest_dir_created) remove_images_in_dir(dest_dir);
    if (normalize && dest_dir_created) normalize_filenames(dest_dir);

    if (dest_dir_created) fs::current_path(prev_path);
    db_close(db);

    return success;
}

std::ostream& operator<<(std::ostream& os, const downloader& obj) {
    os << "Downloader playlists:";
    for (const auto& p : obj.playlists) {
        os << " " << p;
    }
    os << std::endl;

    os << "Downloader options:" << std::endl;
    watch(os, obj.db_path);
    watch(os, obj.dest_dir);
    watch(os, o_gen);
    watch(os, o_prg);
    watch(os, o_prgt);
    watch(os, o_out);
    watch(os, o_pp);
    watch(os, obj.max_rate);
    watch(os, obj.max_duration);

    return os;
}
}  // namespace mqr
