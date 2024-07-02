add_library(glfw3 INTERFACE)
target_include_directories(glfw3 INTERFACE .)
target_link_directories(glfw3 INTERFACE .)
target_link_libraries(glfw3 INTERFACE libglfw3.a)
