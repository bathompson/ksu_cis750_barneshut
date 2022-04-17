CPMAddPackage("gh:glfw/glfw#3.3.7")
CPMAddPackage("gh:Dav1dde/glad#v0.1.36")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(
    NAME stb
    GIT_REPOSITORY "https://github.com/nothings/stb.git"
    GIT_TAG af1a5bc352164740c1cc1354942b1c6b72eacb8a
    DOWNLOAD_ONLY YES
)
CPMAddPackage(
    NAME imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG v1.87
    DOWNLOAD_ONLY YES
)
CPMAddPackage(
    NAME imgui-filebrowser
    GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
    GIT_TAG ad86141dadc76b6338cfd069b1ef4883adf90e3f
    DOWNLOAD_ONLY YES
)

cmrc_add_resource_library(
    resources ALIAS resources::rc
    shader/star_fragment.glsl
    shader/star_vertex.glsl
    textures/massgradient.png
    textures/light.png
)

aux_source_directory(${imgui_SOURCE_DIR} IMGUI_SOURCES)
add_executable(visualizer
    shader_program.cpp shader_program.h
    vertex_buffer.h vertex.h
    camera.h camera_controller.cpp camera_controller.h
    result_file.cpp result_file.h
    imgui_ui.cpp imgui_ui.h
    ${IMGUI_SOURCES} ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    visualizer.cpp texture.cpp texture.h)

target_link_libraries(visualizer resources::rc)
target_link_libraries(visualizer glfw)
target_link_libraries(visualizer glad)
target_link_libraries(visualizer glm::glm)
target_link_options(visualizer PRIVATE -static-libgcc -static-libstdc++ -lstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive)
target_include_directories(visualizer PUBLIC ${stb_SOURCE_DIR})
target_include_directories(visualizer PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(visualizer PUBLIC ${imgui-filebrowser_SOURCE_DIR})
