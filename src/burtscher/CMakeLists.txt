add_executable(burtscher_cuda_implementation
    ../common/fileIO_util.c ../common/vector_utils.c
    ECL-BH_45.cu)

target_include_directories(burtscher_cuda_implementation PUBLIC ../common)
target_include_directories(burtscher_cuda_implementation PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
