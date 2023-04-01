#ifndef ARGS_HPP
#define ARGS_HPP

#include <filesystem>
#include <iomanip>
#include <iostream>

namespace mqr {
namespace fs = std::filesystem;

class args {
   public:
    bool verbose;
    bool cleanup;
    bool normalize;
    std::string rate_limit;
    std::string duration_limit;
    fs::path output;

    args();
    void reset_options();

    bool parse(int argc, char** argv);
    void help(char* binary);

    friend std::ostream& operator<<(std::ostream& os, const args& obj);
};
}  // namespace mqr

#endif  // ARGS_HPP
