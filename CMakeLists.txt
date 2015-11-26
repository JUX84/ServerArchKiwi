project(ServerArchKiwi)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "-Wall -O3 -DNDEBUG")

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O3 -DNDEBUG")

add_executable(server main.cpp)

target_link_libraries(server wiringPi pthread)
