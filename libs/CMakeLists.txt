cmake_minimum_required(VERSION 3.0.0)

find_package(Boost REQUIRED COMPONENTS filesystem)

set (CMAKE_CXX_COMPILE clanlg++)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Parsing SHARED Parsing.cpp Parsing.hpp)




