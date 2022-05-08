add_executable(parallel_barnes_hut cuda_barnes_hut.cu  ../common/fileIO_util.c
    ../common/vector_utils.h ../common/vector_utils.c
    ../common/vector_utils.cuh ../common/vector_utils.cu ../common/octree.c
    cuda_octree.h cuda_octree.c)

set_target_properties(parallel_barnes_hut PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(parallel_barnes_hut PUBLIC ../common)
target_include_directories(parallel_barnes_hut PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
