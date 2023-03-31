#include <iostream>

#include "args.hpp"

int main(int argc, char** argv) {
    mqr::args args;
    if (!args.parse(argc, argv)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
