cmake_minimum_required(VERSION 3.19)
project(ksu_cis750_barneshut LANGUAGES C CXX CUDA)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 14)

include_directories(src)
include(lib/CPM.cmake)
include(lib/CMakeRC.cmake)

add_subdirectory(src/serial)
add_subdirectory(src/parallel)
add_subdirectory(src/visualizer)
add_subdirectory(src/datageneration)
add_subdirectory(src/burtscher)
add_subdirectory(src/nyland)
