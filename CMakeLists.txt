cmake_minimum_required(VERSION 3.5)

project(scollector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(scollector src/main.cpp)

target_sources(scollector PRIVATE
    src/args.hpp
    src/db.hpp
    src/downloader.hpp

    src/args.cpp
    src/db.cpp
    src/downloader.cpp
)

find_package(SQLite3 REQUIRED)
target_link_libraries(scollector PRIVATE SQLite::SQLite3)

find_package(OpenMP REQUIRED)
target_link_libraries(scollector PRIVATE OpenMP::OpenMP_CXX)

set_target_properties(scollector PROPERTIES LINK_FLAGS_RELEASE -s)

install(TARGETS scollector
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
