file(GLOB_RECURSE sources *.cpp)
add_library(zirc-lib ${sources})

set_target_properties(zirc-lib PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(zirc-lib PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wuninitialized
)

target_link_libraries(zirc-lib PRIVATE)
target_include_directories(zirc-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
