#include "args.hpp"

#define watch(os, x) os << std::left << std::setw(16) << #x ":" << std::boolalpha << x << std::endl;

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
        std::string opt_type = arg.substr(0, 2);
        if (opt_type == "--") {
            std::string opt = arg.substr(2);

            if (opt == "help") {
                help();
                return false;
            } else {
                std::cout << "Unknown option " << arg << std::endl;
                help();
                return false;
            }
        }

        opt_type = arg.substr(0, 1);
        if (opt_type == "-") {
            std::string opt = arg.substr(1);

            for (const auto& ch : opt) {
                if (ch == 'h') {
                    help();
                    return false;
                } else if (ch == 'v') {
                    verbose = true;
                } else if (ch == 'o') {
                    if (i + 1 >= argc) {
                        std::cout << "-o option without path" << std::endl;
                        help();
                        return false;
                    }

                    std::string arg_next = argv[i + 1];
                    if (fs::exists(arg_next) && fs::is_directory(arg_next)) {
                        output = arg_next;
                        output = fs::absolute(output.lexically_normal()) / "scollector";
                    } else {
                        std::cerr << arg_next << " doesn't exist or isn't a directory" << std::endl
                                  << std::endl;
                        help();
                        return false;
                    }

                    i++;
                } else if (ch == 'c') {
                    cleanup = true;
                } else if (ch == 'n') {
                    normalize = true;
                } else {
                    std::cout << "Unknown option -" << ch << std::endl;
                    help();
                    return false;
                }
            }
        } else {
            std::cout << "Unknown arg " << arg << std::endl;
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

    std::cout << LW(w) << " -o PATH"
              << "set path where scollector directory will be created" << std::endl;

    std::cout << LW(w) << " -c"
              << "yt-dlp may leave some images and .part files in directory" << std::endl;
    std::cout << LW(w) << ""
              << "with this option {.png, .jpg, .part} files will be deleted" << std::endl;

    std::cout << LW(w) << " -n"
              << "normalize song filenames in directory" << std::endl;
    std::cout << LW(w) << ""
              << "with this option {.mp3, .wav, .aac} files will be renamed" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const args& obj) {
    os << "Args options:" << std::endl;
    watch(os, obj.verbose);
    watch(os, obj.output);
    watch(os, obj.cleanup);
    watch(os, obj.normalize);

    return os;
}
}  // namespace mqr
