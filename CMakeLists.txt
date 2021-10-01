# the project's main CMakeLists file

cmake_minimum_required(VERSION 3.14)

project(LD49)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

set(source_files 
	src/main.cpp
	src/gfx/mesh.cpp
	src/gfx/shader.cpp
)

set(header_files
	src/gfx/mesh.h
	src/gfx/shader.h
	src/utility/transparent_string_hash.h
	src/utility/defer.h
)

add_executable(game ${source_files} ${header_files})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)

target_include_directories(game PRIVATE external)
target_include_directories(game PUBLIC src)

target_link_libraries(game glm glfw lib_imgui lib_glad lib_tinyobjloader)

# tell imgui_impl_opengl3 that we'll provide OpenGL function pointers
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

find_package(OpenGL REQUIRED)

# enable asan for debug builds
if (DEBUG)
    if (WIN32)
        target_compile_options(rvpt PUBLIC /fsanitize=address)
    else()
        target_compile_options(rvpt PUBLIC -fsanitize=address)
    endif()
endif()

# Determine whether we're compiling with clang++
string(FIND "${CMAKE_CXX_COMPILER}" "clang++" GAME_COMPILER_CLANGPP)
if(GAME_COMPILER_CLANGPP GREATER -1)
  set(GAME_COMPILER_CLANGPP 1)
else()
  set(GAME_COMPILER_CLANGPP 0)
endif()

target_compile_options(game
	INTERFACE
	$<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>,${GAME_COMPILER_CLANGPP}>:
	-Wall
	-Wextra
	-pedantic-errors
	-Wconversion
	-Wsign-conversion>
	$<$<CXX_COMPILER_ID:MSVC>:
	/WX
	/W4>
)

# copies files to the build folder
add_custom_target(copy-asset-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    DEPENDS game)