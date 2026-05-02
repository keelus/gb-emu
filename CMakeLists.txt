cmake_minimum_required(VERSION 3.31)
project(zirc CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(core)
add_subdirectory(platform)
