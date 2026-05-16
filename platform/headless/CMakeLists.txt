add_executable(zirc-headless headless.cpp)

target_link_libraries(zirc-headless PRIVATE zirc-lib)
set_target_properties(zirc-headless PROPERTIES OUTPUT_NAME "zirc")
