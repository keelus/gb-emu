cmake_minimum_required(VERSION 3.31)
project(gb-emu CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

file(GLOB_RECURSE sources src/*.cpp)
list(FILTER sources EXCLUDE REGEX "src/main.cpp$")
add_library(gb-emu-lib ${sources})

target_link_libraries(gb-emu-lib PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

target_include_directories(gb-emu-lib PUBLIC
    src/
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

add_executable(gb-emu src/main.cpp)
target_link_libraries(gb-emu PRIVATE gb-emu-lib)
