#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <string>
#include <vector>

#ifdef _WIN32
    #define PIPE_TO_NULL " 2>NUL 1>NUL "
#else
    #define PIPE_TO_NULL " 2>/dev/null 1>/dev/null "
#endif

#define PIPE_TO_STDOUT " 2>&1 "

namespace mqr {
class downloader {
   private:
    std::string lang;
    std::vector<std::string> charts = {"top", "trending"};
    std::vector<std::string> genres = {"danceedm", "electronic", "hiphoprap", "house"};

   public:
    downloader();
    bool download(bool verbose);

    void print_charts();
    void print_genres();
};
}  // namespace mqr

#endif  // DOWNLOADER_HPP
