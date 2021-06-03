cmake_minimum_required(VERSION 3.0.0)

find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(${CMAKE_SOURCE_DIR}/libs)
link_directories(${CMAKE_SOURCE_DIR}/libs)

set (CMAKE_CXX_COMPILE clanlg++)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(ParallelSort ParallelSort.cpp)
add_executable(FileGenerator FileGenerator.cpp)
add_executable(SortCheck SortCheck.cpp)


target_include_directories(ParallelSort PUBLIC ${Boost_INCLUDE_DIR})
target_link_directories(ParallelSort PUBLIC ${Boost_LIBRARY_DIR})
target_link_libraries(ParallelSort Boost::filesystem pthread Parsing)
target_link_libraries(FileGenerator Parsing)
target_link_libraries(SortCheck Parsing)




