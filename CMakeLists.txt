cmake_minimum_required(VERSION 3.10.0)
cmake_policy(SET CMP0072 NEW)
project(CHIP-8 VERSION 0.1.0 LANGUAGES C CXX)

find_package(SDL2 REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)

file(GLOB SOURCES src/*.cpp)
file(GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/*.cpp
  ${IMGUI_DIR}/backends/*.cpp
)

include_directories(
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
)

add_executable(CHIP-8 ${SOURCES} ${IMGUI_SOURCES})
target_link_libraries(CHIP-8 PRIVATE SDL2::SDL2)
