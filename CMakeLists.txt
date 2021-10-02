# the project's main CMakeLists file

cmake_minimum_required(VERSION 3.14)

# part of the hack to change the MSVC runtime
cmake_policy(SET CMP0091 NEW)

project(LD49)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

# add PhysX binaries
set(PHYSX_LIBRARIES
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysXExtensions_static_64.lib
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysX_64.lib
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysXPvdSDK_static_64.lib
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysXCharacterKinematic_static_64.lib
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysXCooking_64.lib
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysXCommon_64.lib
	${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/PhysXFoundation_64.lib
	#${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/lib/SceneQuery_static_64.lib
)

set(source_files
	src/main.cpp
	src/components.cpp
	src/gfx/mesh.cpp
	src/gfx/shader.cpp
	src/gfx/renderer.cpp
	src/gfx/camera.cpp
	src/game/game.cpp
	src/game/physics.cpp
)

set(header_files
	src/components.h
	src/macros.h
	src/gfx/mesh.h
	src/gfx/shader.h
	src/gfx/renderer.h
	src/gfx/camera.h
	src/utility/transparent_string_hash.h
	src/utility/defer.h
	src/game/game.h
	src/game/physics.h
)

add_executable(game ${source_files} ${header_files})

# hacks
set_property(TARGET game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)

target_include_directories(game PRIVATE external external/PhysX/pxshared/include)
target_include_directories(game PUBLIC src)

# tell imgui_impl_opengl3 that we'll provide OpenGL function pointers
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

find_package(OpenGL REQUIRED)

# enable asan for debug builds
if (DEBUG)
    if (WIN32)
        target_compile_options(game PUBLIC /fsanitize=address)
    else()
        target_compile_options(game PUBLIC -fsanitize=address)
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
	#-pedantic-errors
	-Wconversion
	-Wsign-conversion>
	$<$<CXX_COMPILER_ID:MSVC>:
	#/WX-
	/W3
	>
)

# copies assets to the build folder
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)
add_dependencies(game copy_assets)

add_custom_target(copy_physx_binaries ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/external/PhysX/bin/x64/debug/dll ${CMAKE_BINARY_DIR})
add_dependencies(game copy_physx_binaries)

target_link_libraries(game glm glfw lib_imgui lib_glad lib_tinyobjloader ${PHYSX_LIBRARIES})





#### Build PhysX library ####
#include(FetchContent)
#
#FetchContent_GetProperties(physx)
#if(NOT physx_POPULATED)
#	FetchContent_Populate(physx)
#endif()
#
## PHYSX_PATH - path to the `{cloned repository}/physx` repo directory git://github.com/NVIDIAGameWorks/PhysX.git
#set(ENV{PHYSX_PATH} ${physx_SOURCE_DIR}/physx)
#set(PHYSX_ROOT_DIR $ENV{PHYSX_PATH} ) #This is needed for $ENV{PHYSX_PATH}/compiler/public/CMakeLists.txt
#set(PHYSX_INCLUDE_DIRS $ENV{PHYSX_PATH}/include/ $ENV{PHYSX_PATH}/../pxshared/include/)
#set(PHYSX_LIBRARIES
#	PhysXExtensions
#	PhysX
#	PhysXPvdSDK
#	PhysXVehicle
#	PhysXCharacterKinematic
#	PhysXCooking
#	PhysXCommon
#	PhysXFoundation
#	# SnippetUtils
#)
#
#set(TARGET_BUILD_PLATFORM "windows") # has to match the TARGET_BUILD_PLATFORM in $ENV{PHYSX_PATH}/physix/buildtools/cmake_generate_projects.py
#set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
#set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
#set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
#set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
#set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
#set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
#set(PXSHARED_PATH $ENV{PHYSX_PATH}/../pxshared)
#set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
#set(CMAKEMODULES_VERSION "1.27")
#set(CMAKEMODULES_PATH $ENV{PHYSX_PATH}/../externals/cmakemodules)
#set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
#set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
#
## Call into PhysX's CMake scripts
#add_subdirectory($ENV{PHYSX_PATH}/compiler/public externals/physx)
#
## Add physx libraries to target
##target_link_libraries(game PUBLIC ${PHYSX_LIBRARIES})
#
##### Windows only: Copy the Physx dll files to the simulation executable####
#if (TARGET_BUILD_PLATFORM STREQUAL "windows")
#	# References NvidiaBuildOptions.cmake to figure out if system is 32/64 bit
#	IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
#		SET(LIBPATH_SUFFIX "64")
#	ELSE()
#		SET(LIBPATH_SUFFIX "32")
#	ENDIF()
#	GetPlatformBinName(PLATFORM_BIN_NAME ${LIBPATH_SUFFIX})
#	set(PhysxOutputPath ${PX_OUTPUT_LIB_DIR}/bin/${PLATFORM_BIN_NAME}/)
#	message("Physx Output Path: " ${PhysxOutputPath})
#
#	# copy PhysX dll's to build dir. Happens on every build.
#	#add_custom_command(TARGET game POST_BUILD
#	#	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PhysxOutputPath} ${CMAKE_BINARY_DIR})
#		#COMMAND ${CMAKE_COMMAND} -E copy_directory "${PhysxOutputPath}" "$<TARGET_FILE_DIR:target_name >/..")
#	
#	add_custom_target(copy_physx_binaries ALL
#		COMMAND ${CMAKE_COMMAND} -E copy_directory ${PhysxOutputPath} ${CMAKE_BINARY_DIR})
#	add_dependencies(game copy_physx_binaries)
#endif()
#
#message(STATUS "PhysX include dir: " ${PHYSX_INCLUDE_DIRS})
#target_include_directories(game SYSTEM ${PHYSX_INCLUDE_DIRS})
#
#target_link_libraries(game glm glfw lib_imgui lib_glad lib_tinyobjloader ${PHYSX_LIBRARIES})