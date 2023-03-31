#include <iostream>

#include "args.hpp"

int main(int argc, char** argv) {
    mqr::args args;
    if (!args.parse(argc, argv)) return EXIT_FAILURE;

    if (args.verbose) {
        std::cout << "Output directory: " << args.output << std::endl;
    }

    return EXIT_SUCCESS;
}
