#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <filesystem>
#include <string>
#include <vector>

#ifdef _WIN32
    #define PIPE_TO_NULL " 2>NUL 1>NUL "
#else
    #define PIPE_TO_NULL " 2>/dev/null 1>/dev/null "
#endif

#define PIPE_TO_STDOUT " 2>&1 "

namespace mqr {
namespace fs = std::filesystem;

class downloader {
   private:
    fs::path dest_dir;
    std::string max_rate;
    const std::string lang;
    const std::vector<std::string> charts = {"top", "trending"};
    const std::vector<std::string> genres = {"danceedm", "electronic", "hiphoprap", "house"};

   public:
    downloader(fs::path output, std::string& rate_limit);
    bool download(bool cleanup, bool normalize);

    const std::vector<std::string> get_charts() const;
    const std::vector<std::string> get_genres() const;
    const fs::path get_destination() const;

    friend std::ostream& operator<<(std::ostream& os, const downloader& obj);
};
}  // namespace mqr

#endif  // DOWNLOADER_HPP
