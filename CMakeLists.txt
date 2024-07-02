cmake_minimum_required(VERSION 3.28)
project(Storage)
set(CMAKE_VERBOSE_MAKEFILE  ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(./vendor)

link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(./include)
include_directories(./include)
include_directories(./vendor)


add_executable(storage src/main.cpp
src/Console.cpp
src/CSV.cpp
src/FileDialog.cpp
src/Item.cpp
src/ItemDatabase.cpp
src/program.cpp
src/ProgramStateDatabase.cpp
src/utils.cpp
src/Gui/GuiDatabase.cpp
src/Gui/GuiTable.cpp)
target_link_libraries(storage gl opengl32 ole32 oleaut32 imm32 comdlg32 gdi32 boost_header logger glfw3 _sqlite3 sqllitecpp stb simpleini imgui)
target_compile_definitions(storage PRIVATE GERMAN=1)