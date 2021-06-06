cmake_minimum_required(VERSION 3.0.0)
project(AncudTest VERSION 0.1.0)

add_subdirectory(src)
add_subdirectory(libs)

if(BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()