cmake_minimum_required(VERSION 3.0.0)
project(UncudTest VERSION 0.1.0)

find_package(Boost 1.75 REQUIRED COMPONENTS filesystem)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set (CMAKE_CXX_COMPILE clanlg++)
set (CMAKE_CXX_STANDARD 11)



add_executable(ParallelSort ParallelSort.cpp)
add_executable(FileGenerator FileGenerator.cpp)
add_executable(SortCheck SortCheck.cpp)

TARGET_LINK_LIBRARIES(ParallelSort Boost::filesystem)



include(CTest)
enable_testing()
add_test(FileGenerator_Prefix_Invalid FileGenerator -s 10 -p -1)
add_test(FileGenerator_Prefix_OutOfRange FileGenerator -s 10 -p 10)
add_test(FileGenerator_Size_Invalid FileGenerator -s fsfs -p 1)
add_test(FileGenerator_Size_Null FileGenerator -s 0)
add_test(FileGenerator_Size_Prefix_Invalid FileGenerator -s fsfs -p fds)
add_test(FileGenerator_Size_Prefix_Invalid_OutOfRange FileGenerator -s fsfs -p 10)
add_test(ParallelSort_InputPath_Invalid ParallelSort -i fsfs)
add_test(ParallelSort_InputPath_Null ParallelSort -i)
add_test(ParallelSort_Regular_Sort FileGenerator ParallelSort)
add_test(SortCheck_Checked_Sort SortCheck)
add_test(SortCheck_Checked_UnSort SortCheck -f Array)

set_tests_properties(FileGenerator_Prefix_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid prefix type;Default prefix will be used")
set_tests_properties(FileGenerator_Prefix_OutOfRange PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid prefix value: is too big, or too small.;Default prefix will be used")
set_tests_properties(FileGenerator_Size_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid size;Default size will be used")
set_tests_properties(FileGenerator_Size_Null PROPERTIES
PASS_REGULAR_EXPRESSION "The size cannot be 0;Default size will be used")
set_tests_properties(FileGenerator_Size_Prefix_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid size;Default size will be used;Invalid prefix type;Default prefix will be used")
set_tests_properties(FileGenerator_Size_Prefix_Invalid_OutOfRange PROPERTIES
PASS_REGULAR_EXPRESSION "Invalid size;Default size will be used;Invalid prefix value: is too big, or too small.;Default prefix will be used")
set_tests_properties(ParallelSort_InputPath_Invalid PROPERTIES
PASS_REGULAR_EXPRESSION "The input file doesn't exist")
set_tests_properties(ParallelSort_InputPath_Null PROPERTIES
PASS_REGULAR_EXPRESSION "ParallelSort: option requires an argument -- i")
set_tests_properties(ParallelSort_Regular_Sort PROPERTIES
FAIL_REGULAR_EXPRESSION "An error occurred, while trying to open the file")
set_tests_properties(SortCheck_Checked_Sort PROPERTIES
PASS_REGULAR_EXPRESSION "The array is sorted")
set_tests_properties(SortCheck_Checked_UnSort PROPERTIES
PASS_REGULAR_EXPRESSION "The array is not sorted")
