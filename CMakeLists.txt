cmake_minimum_required(VERSION 3.0.0)
project(UncudTest VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

add_executable(ParallelSort ParallelSort.cpp)
add_executable(FileGenerator FileGenerator.cpp)
add_executable(SortCheck SortCheck.cpp)

include(CTest)
enable_testing()
add_test(FileGenerator_Prefix_Invalid FileGenerator -s 10 -p -1)
add_test(FileGenerator_Size_Invalid FileGenerator -s fsfs -p 1)
add_test(FileGenerator_Size_Prefix_Invalid FileGenerator -s fsfs -p fds)
add_test(ParallelSort_Path_Invalid ParallelSort -f fsfs)
add_test(ParallelSort_Path_Null ParallelSort -f)
add_test(ParallelSort_Regular_Sort FileGenerator ParallelSort)
add_test(ParallelSort_Checked_Sort bash -c ./FileGenerator ./ParallelSort ./SortCheck)
add_test(ParallelSort_Checked_UnSort bash -c ./FileGenerator; ./ParallelSort; ./SortCheck -f Array)

set_tests_properties(FileGenerator_Prefix_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid prefix type")
set_tests_properties(FileGenerator_Size_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid size")
set_tests_properties(FileGenerator_Size_Prefix_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid size")
set_tests_properties(ParallelSort_Path_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Error, while trying to open the file")
set_tests_properties(ParallelSort_Path_Null PROPERTIES
PASS_REGULAR_EXPRESSION "ParallelSort: option requires an argument -- f")
set_tests_properties(ParallelSort_Regular_Sort PROPERTIES
FAIL_REGULAR_EXPRESSION "Error, while trying to open the file")
set_tests_properties(ParallelSort_Checked_Sort PROPERTIES
FAIL_REGULAR_EXPRESSION "Error, while trying to open the file;Error occured while reading the file;Array is not sorted")
set_tests_properties(ParallelSort_Checked_UnSort PROPERTIES
FAIL_REGULAR_EXPRESSION "Error, while trying to open the file;Error occured while reading the file;Array is sorted")


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)