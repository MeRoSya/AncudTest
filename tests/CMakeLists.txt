cmake_minimum_required(VERSION 3.0.0)

set (EXECUTABLE_DIR ${CMAKE_BINARY_DIR}/src)
add_test(FileGenerator_Prefix_Invalid ${EXECUTABLE_DIR}/FileGenerator -s 10 -p -1)
add_test(FileGenerator_Prefix_OutOfRange ${EXECUTABLE_DIR}/FileGenerator -s 10 -p 10)
add_test(FileGenerator_Size_Invalid ${EXECUTABLE_DIR}/FileGenerator -s fsfs -p 1)
add_test(FileGenerator_Size_Null ${EXECUTABLE_DIR}/FileGenerator -s 0)
add_test(FileGenerator_Size_Prefix_Invalid ${EXECUTABLE_DIR}/FileGenerator -s fsfs -p fds)
add_test(FileGenerator_Size_Prefix_Invalid_OutOfRange ${EXECUTABLE_DIR}/FileGenerator -s fsfs -p 10)
add_test(ParallelSort_InputPath_Invalid ${EXECUTABLE_DIR}/ParallelSort -i fsfs)
add_test(ParallelSort_InputPath_Null ${EXECUTABLE_DIR}/ParallelSort -i)
add_test(ParallelSort_Regular_Sort ${EXECUTABLE_DIR}/FileGenerator ${EXECUTABLE_DIR}/ParallelSort)
add_test(SortCheck_Checked_Sort sh -c "${EXECUTABLE_DIR}/FileGenerator && ${EXECUTABLE_DIR}/ParallelSort && ${EXECUTABLE_DIR}/SortCheck")
add_test(SortCheck_Checked_UnSort ${EXECUTABLE_DIR}/SortCheck -f Array)

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
