file(GLOB_RECURSE sources *.cpp)
add_library(gb-emu-lib ${sources})

target_link_libraries(gb-emu-lib PRIVATE)
target_include_directories(gb-emu-lib PUBLIC .)
