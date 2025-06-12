cmake_minimum_required(VERSION 3.10.0)
cmake_policy(SET CMP0072 NEW)
project(CHIP8 VERSION 0.1.0 LANGUAGES C CXX)

if(WIN32)
  set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2/cmake)
endif()
find_package(SDL2 REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
set(IMGUIDIALOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/ImGuiFileDialog)

file(GLOB SOURCES src/*.cpp)
file(GLOB IMGUI_SOURCES
  ${IMGUI_DIR}/*.cpp
  ${IMGUI_DIR}/backends/*.cpp
)
file(GLOB IMGUIDIALOG_SOURCES ${IMGUIDIALOG_DIR}/*.cpp)

include_directories(
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
  ${IMGUIDIALOG_DIR}
)

add_executable(CHIP8 ${SOURCES} ${IMGUI_SOURCES} ${IMGUIDIALOG_SOURCES})
target_link_libraries(CHIP8 PRIVATE SDL2::SDL2)
