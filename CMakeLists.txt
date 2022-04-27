cmake_minimum_required(VERSION 3.19)
project(ksu_cis750_barneshut LANGUAGES C CXX CUDA)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 14)

include_directories(src)
include(lib/CPM.cmake)
include(lib/CMakeRC.cmake)

link_libraries(m)

add_subdirectory(src/serial)
add_subdirectory(src/parallel)
if (WIN32)
    add_subdirectory(src/visualizer)
endif (WIN32)
add_subdirectory(src/datageneration)
add_subdirectory(src/burtscher)
add_subdirectory(src/nyland)
