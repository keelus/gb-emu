find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_executable(gb-emu-sdl2 sdl2.cpp)

target_link_libraries(gb-emu-sdl2 PRIVATE gb-emu-lib
    ${SDL2_LIBRARIES}
)
target_include_directories(gb-emu-sdl2 PRIVATE
    ${SDL2_INCLUDE_DIRS}
)
