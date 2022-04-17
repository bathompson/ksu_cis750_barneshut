add_executable(serial_direct_sum serial_direct_sum.c ../common/fileIO_util.c ../common/vector_utils.c)

target_include_directories(serial_direct_sum PUBLIC ../common)
