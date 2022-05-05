add_executable(serial_direct_sum serial_direct_sum.cpp ../common/fileIO_util.c ../common/vector_utils.c)

target_include_directories(serial_direct_sum PUBLIC ../common)

add_executable(serial_barnes_hut serial_barnes_hut.cpp ../common/fileIO_util.c ../common/vector_utils.c ../common/octree.c)

target_include_directories(serial_barnes_hut PUBLIC ../common)

