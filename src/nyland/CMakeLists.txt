CPMAddPackage("gh:FreeGLUTProject/freeglut#v3.2.2")
CPMAddPackage(
    NAME glew-cmake
    GIT_REPOSITORY "https://github.com/Perlmint/glew-cmake"
    GIT_TAG glew-cmake-2.2.0
    OPTIONS "ONLY_LIBS ON" "glew-cmake_BUILD_SHARED NO"
)

add_executable(nyland_cuda_implementation
    ../common/fileIO_util.c ../common/vector_utils.c
    bank_checker.cpp cmd_arg_reader.cpp cutil.cpp error_checker.cpp stopwatch.cpp param.cpp paramgl.cpp
    bodysystemcuda.cu bodysystemcpu.cpp bodysystemcuda.cpp nbody.cpp render_particles.cpp nbody_gold.cpp)

if (WIN32)
    target_sources(nyland_cuda_implementation PRIVATE stopwatch_win.cpp)
endif()

if (UNIX)
    target_sources(nyland_cuda_implementation PRIVATE stopwatch_linux.cpp)
endif (UNIX)

target_link_libraries(nyland_cuda_implementation freeglut_static)
target_link_libraries(nyland_cuda_implementation libglew_static)

target_include_directories(nyland_cuda_implementation PUBLIC ${freeglut_SOURCE_DIR}/include)
target_include_directories(nyland_cuda_implementation PUBLIC ${glew-cmake_SOURCE_DIR}/include)
target_include_directories(nyland_cuda_implementation PUBLIC .)
target_include_directories(nyland_cuda_implementation PUBLIC ../common)
target_include_directories(nyland_cuda_implementation PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
