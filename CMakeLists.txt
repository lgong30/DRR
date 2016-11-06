cmake_minimum_required(VERSION 3.6)
project(DRR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(SOURCE_FILES main.cpp drr.hpp packet.hpp packetAPIs.hpp)

add_subdirectory(test)
#add_executable(DRR ${SOURCE_FILES})