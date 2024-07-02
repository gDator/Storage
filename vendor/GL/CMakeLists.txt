add_library(gl INTERFACE)
target_include_directories(gl INTERFACE .)
target_link_directories(gl INTERFACE .)
target_link_libraries(gl INTERFACE libglew32s.a)