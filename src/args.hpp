#ifndef ARGS_HPP
#define ARGS_HPP

#include <iomanip>
#include <iostream>

namespace mqr {
class args {
   public:
    bool verbose;

    args() { reset_options(); }

    void reset_options() { verbose = false; }

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

            if (arg == "-v" || arg == "--verbose") verbose = true;
        }

        return true;
    }

    void help() {
        std::cout << "scollector" << std::endl << std::endl;
        std::cout << "options:" << std::endl;
        std::cout << std::left << std::setw(30) << "  -v, --verbose"
                  << "print extra info" << std::endl;
    }
};
}  // namespace mqr

#endif  // ARGS_HPP
