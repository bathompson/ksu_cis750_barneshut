add_executable(nyland_cuda_implementation
    ../common/fileIO_util.c ../common/vector_utils.c
    bank_checker.cpp cmd_arg_reader.cpp cutil.cpp stopwatch.cpp param.cpp
    bodysystemcuda.cu bodysystemcpu.cpp bodysystemcuda.cpp nbody.cpp nbody_gold.cpp)

if (WIN32)
    target_sources(nyland_cuda_implementation PRIVATE stopwatch_win.cpp)
endif()

if (UNIX)
    target_sources(nyland_cuda_implementation PRIVATE stopwatch_linux.cpp)
endif (UNIX)

target_include_directories(nyland_cuda_implementation PUBLIC .)
target_include_directories(nyland_cuda_implementation PUBLIC ../common)
target_include_directories(nyland_cuda_implementation PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
