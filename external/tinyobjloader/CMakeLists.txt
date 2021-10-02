add_library(lib_tinyobjloader src/tiny_obj_loader.cpp)
target_include_directories(lib_tinyobjloader PUBLIC include)

# hack
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
set_property(TARGET lib_tinyobjloader PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")