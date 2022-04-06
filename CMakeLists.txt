cmake_minimum_required(VERSION 3.19)
project(ksu_cis750_barneshut LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CUDA_STANDARD 14)

include_directories(src)

add_subdirectory(src/serial)
add_subdirectory(src/parallel)
add_subdirectory(src/visualizer)
