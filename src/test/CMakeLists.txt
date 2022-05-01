add_executable(compare_outputs
        compare_outputs.cpp
        ../visualizer/result_file.cpp)

target_include_directories(compare_outputs PUBLIC ../visualizer)
