file(GLOB_RECURSE sources *.cpp)
add_library(zirc-lib ${sources})

target_link_libraries(zirc-lib PRIVATE)
target_include_directories(zirc-lib PUBLIC .)
