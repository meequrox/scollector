#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <filesystem>
#include <map>
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

typedef struct {
    std::map<std::string, std::string> options;
    std::map<std::string, bool> flags;
} downloader_args;

class downloader {
   private:
    const downloader_args args;

    fs::path db_path;
    std::vector<std::string> playlists;

   public:
    downloader(const downloader_args& args);
    int download() const;

    friend std::ostream& operator<<(std::ostream& os, const downloader& obj);
};
}  // namespace mqr

#endif  // DOWNLOADER_HPP
