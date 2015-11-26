project(ServerArchKiwi)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -O3 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable(server main.cpp)

target_link_libraries(server wiringPi pthread)
