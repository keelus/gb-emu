find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)
pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

file(GLOB_RECURSE sources *.cpp)
add_executable(zirc-gtk ${sources})
target_link_libraries(zirc-gtk PRIVATE zirc-lib
    PkgConfig::GTKMM
    PkgConfig::FONTCONFIG
    PkgConfig::PORTAUDIO
    OpenGL::GL
    PkgConfig::GLEW
    PkgConfig::SDL2
)

target_include_directories(zirc-gtk PUBLIC .)
target_compile_options(zirc-gtk PRIVATE
    ${FONTCONFIG_CFLAGS}
    ${PORTAUDIO_CFLAGS}
    ${GLEW_CFLAGS}
)
set_target_properties(zirc-gtk PROPERTIES OUTPUT_NAME "zirc")
