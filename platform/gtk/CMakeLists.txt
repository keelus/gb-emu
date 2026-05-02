find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

add_executable(zirc-gtk gtk.cpp)
target_link_libraries(zirc-gtk PRIVATE zirc-lib
    PkgConfig::GTKMM
    PkgConfig::FONTCONFIG
    PkgConfig::PORTAUDIO
)

target_compile_options(zirc-gtk PRIVATE
    ${FONTCONFIG_CFLAGS}
    ${PORTAUDIO_CFLAGS}
)
set_target_properties(zirc-gtk PROPERTIES OUTPUT_NAME "zirc")
