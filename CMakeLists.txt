cmake_minimum_required(VERSION 3.6)
project(DRR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp drr.cpp drr.h packet.hpp)
add_executable(DRR ${SOURCE_FILES})