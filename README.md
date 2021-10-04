# Top Texplosion

My submission for the Ludum Dare 49 game jam.

## Dependencies
- CMake (meta build system)
- Dear ImGui (UI)
- GLM (math)
- GLFW (windowing)
- OpenGL 4.5 (graphics)
- PhysX (physics)
- TinyObjLoader (model loading)
- Glad (OpenGL loader)

I couldn't get PhysX to play nice with FetchContent, so I manually built the binaries and used that for the release. You'll have to provide your own PhysX binaries or fix the CMakeLists.txt to build this yourself. Everything else should be in the repo or automatically fetched when running CMake.
