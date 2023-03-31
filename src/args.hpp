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
    fs::path output;

    args();
    void reset_options();

    bool parse(int argc, char** argv);
    void help();
};
}  // namespace mqr

#endif  // ARGS_HPP
