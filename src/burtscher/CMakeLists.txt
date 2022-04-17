add_executable(burtscher_cuda_implementation
    ../common/fileIO_util.c ../common/vector_utils.c
    BarnesHutParticleSystem.cpp debug.cpp kernels.cu main.cpp Particle_cuda.cu ParticleSystem.cpp)

target_include_directories(burtscher_cuda_implementation PUBLIC ../common)
target_include_directories(burtscher_cuda_implementation PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
