find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_executable(zirc-sdl2 sdl2.cpp)

target_link_libraries(zirc-sdl2 PRIVATE zirc-lib
    ${SDL2_LIBRARIES}
)
target_include_directories(zirc-sdl2 PRIVATE
    ${SDL2_INCLUDE_DIRS}
)
set_target_properties(zirc-sdl2 PROPERTIES OUTPUT_NAME "zirc")
