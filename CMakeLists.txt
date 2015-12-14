cmake_minimum_required(VERSION 3.4)

project(ServerArchKiwi)

set(CMAKE_MODULE_PATH CMake)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(FindPkgConfig)

file(GLOB_RECURSE source_files src/*)
include_directories(inc)
include_directories(../wiringPi)
link_directories(../wiringPi)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O3 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(kiwi ${source_files})

target_link_libraries(kiwi wiringPi pthread)
