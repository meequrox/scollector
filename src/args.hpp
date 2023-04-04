#ifndef ARGS_HPP
#define ARGS_HPP

#include <filesystem>
#include <map>

namespace mqr {
namespace fs = std::filesystem;

class args {
   public:
    std::map<std::string, bool> flags;
    std::map<std::string, std::string> options;

    args();
    void reset_options();

    bool parse(int argc, char** argv);
    void help(char* binary) const;

    friend std::ostream& operator<<(std::ostream& os, const args& obj);
};
}  // namespace mqr

#endif  // ARGS_HPP
