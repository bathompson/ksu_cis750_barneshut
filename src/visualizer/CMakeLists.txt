CPMAddPackage("gh:glfw/glfw#3.3.7")
CPMAddPackage("gh:Dav1dde/glad#v0.1.36")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(
    NAME stb
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG af1a5bc352164740c1cc1354942b1c6b72eacb8a
    DOWNLOAD_ONLY YES
)

add_executable(visualizer visualizer.cpp)

target_link_libraries(visualizer glfw)
target_link_libraries(visualizer glad)
target_link_libraries(visualizer glm::glm)
target_include_directories(visualizer PUBLIC ${stb_SOURCE_DIR})
