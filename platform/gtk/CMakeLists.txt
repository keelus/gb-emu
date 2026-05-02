find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

add_executable(gb-emu-gtk gtk.cpp)
target_link_libraries(gb-emu-gtk PRIVATE gb-emu-lib
    PkgConfig::GTKMM
    PkgConfig::FONTCONFIG
    PkgConfig::PORTAUDIO
)

target_compile_options(gb-emu-gtk PRIVATE
    ${FONTCONFIG_CFLAGS}
    ${PORTAUDIO_CFLAGS}
)
