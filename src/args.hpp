#ifndef ARGS_HPP
#define ARGS_HPP

#include <filesystem>
#include <iomanip>
#include <iostream>

namespace mqr {
namespace fs = std::filesystem;

#define SET_LWIDTH(w) std::left << std::setw(w)

class args {
   public:
    bool verbose;
    fs::path output;

    args() { reset_options(); }

    void reset_options() {
        verbose = false;
        output = fs::current_path();
    }

    bool parse(int argc, char** argv) {
        if (!argv) return false;

        for (int i = 1; i < argc; i++) {
            if (!argv[i]) {
                reset_options();
                return false;
            }

            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                help();
                return false;
            }

            if (arg == "-v")
                verbose = true;
            else if (arg == "-o") {
                if (i + 1 >= argc) {
                    help();
                    return false;
                }

                std::string arg_next = argv[i + 1];
                if (fs::exists(arg_next) && fs::is_directory(arg_next)) {
                    output = arg_next;
                    output = fs::absolute(output.lexically_normal());
                } else {
                    std::cerr << arg_next << " doesn't exist or isn't a directory" << std::endl
                              << std::endl;
                    help();
                    return false;
                }

                i++;
            } else {
                help();
                return false;
            }
        }

        return true;
    }

    void help() {
        std::cout << "scollector" << std::endl << std::endl;
        std::cout << "options:" << std::endl;
        std::cout << SET_LWIDTH(16) << " -h,--help"
                  << "print this help message" << std::endl;
        std::cout << SET_LWIDTH(16) << " -v"
                  << "print extra info" << std::endl;
        std::cout << SET_LWIDTH(16) << " -o"
                  << "set output directory" << std::endl;
    }
};
}  // namespace mqr

#endif  // ARGS_HPP