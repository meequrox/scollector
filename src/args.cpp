#include "args.hpp"

#include <iostream>

namespace mqr {
args::args() { reset_options(); }

bool args::parse(int argc, char** argv) {
    if (!argv) return false;

    for (int i = 1; i < argc; i++) {
        if (!argv[i]) {
            reset_options();
            return false;
        }

        const std::string arg = argv[i];
        std::string opt_type = arg.substr(0, 2);
        if (opt_type == "--") {
            const std::string opt = arg.substr(2);

            if (opt == "help") {
                help(argv[0]);
                return false;
            } else {
                std::cout << "Unknown option " << arg << std::endl;
                help(argv[0]);
                return false;
            }
        }

        opt_type = arg.substr(0, 1);
        if (opt_type == "-") {
            const std::string opt = arg.substr(1);

            for (const auto& ch : opt) {
                if (ch == 'h') {
                    help(argv[0]);
                    return false;
                } else if (ch == 'l') {
                    if (i + 1 >= argc) {
                        std::cout << "-l option without country code" << std::endl;
                        help(argv[0]);
                        return false;
                    }

                    options["country_code"] = argv[i + 1];
                    i++;

                    if (options["country_code"].size() != 2) {
                        std::cout << "Unknown country code " << options["country_code"] << std::endl;
                        help(argv[0]);
                        return false;
                    }
                } else if (ch == 'v') {
                    flags["verbose"] = true;
                } else if (ch == 'o') {
                    if (i + 1 >= argc) {
                        std::cout << "-o option without path" << std::endl;
                        help(argv[0]);
                        return false;
                    }

                    if (fs::exists(argv[i + 1]) && fs::is_directory(argv[i + 1])) {
                        fs::path p = argv[i + 1];
                        options["output"] = fs::absolute(p.lexically_normal()) / "scollector_dl";
                    } else {
                        std::cerr << argv[i + 1] << " doesn't exist or isn't a directory" << std::endl
                                  << std::endl;
                        help(argv[0]);
                        return false;
                    }

                    i++;
                } else if (ch == 'c') {
                    flags["cleanup"] = true;
                } else if (ch == 'n') {
                    flags["normalize"] = true;
                } else if (ch == 'r') {
                    if (i + 1 >= argc) {
                        std::cout << "-r option without next arg" << std::endl;
                        help(argv[0]);
                        return false;
                    }

                    options["rate_limit"] = argv[i + 1];
                    i++;
                } else if (ch == 'd') {
                    if (i + 1 >= argc) {
                        std::cout << "-d option without next arg" << std::endl;
                        help(argv[0]);
                        return false;
                    }

                    options["duration_limit"] = argv[i + 1];
                    i++;
                } else {
                    std::cout << "Unknown option -" << ch << std::endl;
                    help(argv[0]);
                    return false;
                }
            }
        } else {
            std::cout << "Unknown arg " << arg << std::endl;
            help(argv[0]);
            return false;
        }
    }

    if (options["country_code"].empty()) {
        std::cout << "-l option is required but not set" << std::endl;
        help(argv[0]);
        return false;
    }

    return true;
}

void args::reset_options() {
    flags["verbose"] = false;
    flags["cleanup"] = false;
    flags["normalize"] = false;

    options["country_code"].clear();
    options["rate_limit"].clear();
    options["duration_limit"].clear();
    options["output"] = fs::current_path() / "scollector_dl";
}

#define LW(w) std::left << std::setw(w)

void args::help(char* binary) const {
    constexpr int w = 16;

    std::cout << "Usage: " << binary << " -l COUNTRY [OPTIONS] [FLAGS]" << std::endl << std::endl;
    std::cout << "options:" << std::endl;

    std::cout << LW(w) << " -l COUNTRY"
              << "which country playlist to download (two-letter ISO 3166-2), e.g. ru, th, mx"
              << std::endl;

    std::cout << LW(w) << " -o PATH"
              << "set path where scollector directory will be created" << std::endl;

    std::cout << std::endl << "flags:" << std::endl;

    std::cout << LW(w) << " -h,--help"
              << "print this help message" << std::endl;

    std::cout << LW(w) << " -v"
              << "print additional info" << std::endl;

    std::cout << LW(w) << " -c"
              << "yt-dlp may leave some images and .part files in directory" << std::endl;
    std::cout << LW(w) << ""
              << "with this option {.png, .jpg, .part} files will be deleted" << std::endl;

    std::cout << LW(w) << " -n"
              << "normalize song filenames in directory" << std::endl;
    std::cout << LW(w) << ""
              << "with this option {.mp3, .wav, .aac} files will be renamed" << std::endl;

    std::cout << LW(w) << " -r"
              << "maximum download rate in bytes per second, e.g. 50K or 4.2M" << std::endl;

    std::cout << LW(w) << " -d"
              << "maximum song duration in seconds, e.g. 600 or 130" << std::endl;
}

#define watchpair(os, x) \
    os << std::left << std::setw(24) << x.first + ":" << std::boolalpha << x.second << std::endl;

std::ostream& operator<<(std::ostream& os, const args& args) {
    os << "Args options:" << std::endl;
    for (const auto& flag : args.flags) watchpair(os, flag);
    for (const auto& opt : args.options) watchpair(os, opt);

    return os << std::endl;
}
}  // namespace mqr
