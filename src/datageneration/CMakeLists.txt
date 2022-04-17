add_executable(data_generator data_generator.c  ../common/vector_utils.c ../common/fileIO_util.c)

target_include_directories(data_generator PUBLIC ../common)
