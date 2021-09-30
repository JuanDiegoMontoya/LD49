cmake_minimum_required(VERSION 3.14)

project(LD49)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

set(source_files 
	src/main.cpp)

add_executable(game ${source_files})# ${header_files})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)

target_include_directories(game PRIVATE external)

target_link_libraries(game glm glfw lib_imgui lib_glad)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

find_package(OpenGL REQUIRED)