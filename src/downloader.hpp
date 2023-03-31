#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <string>
#include <vector>

namespace mqr {
class downloader {
   private:
    std::string lang;
    std::vector<std::string> charts = {"top", "trending"};
    std::vector<std::string> genres = {"danceedm", "electronic", "hiphoprap", "house"};

   public:
    downloader();
    bool download();

    void print_charts();
    void print_genres();
};
}  // namespace mqr

#endif  // DOWNLOADER_HPP
