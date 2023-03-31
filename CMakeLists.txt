cmake_minimum_required(VERSION 3.5)

project(scollector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(scollector src/main.cpp)

target_sources(scollector PRIVATE
    src/args.hpp

    src/args.cpp
)

install(TARGETS scollector
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
