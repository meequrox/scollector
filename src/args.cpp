#include "args.hpp"

namespace mqr {
#define LW(w) std::left << std::setw(w)

args::args() { reset_options(); }

bool args::parse(int argc, char** argv) {
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
                std::cerr << arg_next << " doesn't exist or isn't a directory" << std::endl << std::endl;
                help();
                return false;
            }

            i++;
        } else if (arg == "-c")
            cleanup = true;
        else {
            help();
            return false;
        }
    }

    return true;
}

void args::reset_options() {
    verbose = false;
    cleanup = false;
    output = fs::current_path();
}

void args::help() {
    constexpr int w = 16;

    std::cout << "scollector" << std::endl << std::endl;
    std::cout << "options:" << std::endl;

    std::cout << LW(w) << " -h,--help"
              << "print this help message" << std::endl;

    std::cout << LW(w) << " -v"
              << "print additional info" << std::endl;

    std::cout << LW(w) << " -o"
              << "set output directory" << std::endl;

    std::cout << LW(w) << " -c"
              << "yt-dlp may leave some images and .part files in directory" << std::endl;
    std::cout << LW(w) << ""
              << "with this option ALL {.png, .jpg, .part} files will be deleted" << std::endl;
}
}  // namespace mqr
