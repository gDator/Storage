add_library(imgui
        src/imgui.cpp
        src/imgui_draw.cpp
        src/imgui_stdlib.cpp
        src/imgui_tables.cpp
        src/imgui_widgets.cpp
        src/implot.cpp
        src/implot_items.cpp
        #src/imgui_demo.cpp
        src/imgui_spectrum.cpp
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ./include)
target_link_libraries(imgui PUBLIC glfw3)