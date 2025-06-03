cmake_minimum_required(VERSION 3.10.0)
project(CHIP-8 VERSION 0.1.0 LANGUAGES C CXX)
file(GLOB SOURCES src/*.cpp)

add_executable(CHIP-8 ${SOURCES})
target_include_directories(CHIP-8 PRIVATE include)
target_link_libraries(CHIP-8 PRIVATE SDL2)

