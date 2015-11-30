cmake_minimum_required(VERSION 3.4)

project(ServerArchKiwi)

set(CMAKE_MODULE_PATH CMake)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(FindPkgConfig)

FIND_PACKAGE(LibEV REQUIRED)
link_directories(${LibEV_LIBRARY_DIRS})
include_directories(${LibEV_INCLUDE_DIRS})

file(GLOB_RECURSE source_files src/*)
include_directories(inc)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O3 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(kiwi ${source_files})

target_link_libraries(kiwi wiringPi pthread ev)
