find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_executable(gb-emu-gtk gtk.cpp)
target_link_libraries(gb-emu-gtk PRIVATE gb-emu-lib PkgConfig::GTKMM)
