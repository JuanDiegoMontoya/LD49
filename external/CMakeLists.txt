# external content definitions

option(BUILD_TESTING "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        3.3.2
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG        0.9.9.8
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        v1.84.2
)

FetchContent_MakeAvailable(glm glfw)

# MakeAvailable isn't sufficient for ImGui, so we need to manually add it
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_library(lib_imgui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

    target_include_directories(lib_imgui PUBLIC 
        ${imgui_SOURCE_DIR} 
        ${imgui_SOURCE_DIR}/backends
        ${glfw_SOURCE_DIR}/include)
endif()

# hacks
set_property(TARGET glfw PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
set_property(TARGET lib_imgui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

# add other subdirectories in external/, which aren't grabbed by FetchContent
add_subdirectory(glad)
add_subdirectory(tinyobjloader)