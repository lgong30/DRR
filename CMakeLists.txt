cmake_minimum_required(VERSION 3.6)

if (WIN32)
    #do something
    set(CMAKE_MAKE_PROGRAM mingw32-make)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif (WIN32)

project(DRR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(SOURCE_FILES main.cpp drr.hpp packet.hpp packetAPIs.hpp)

add_subdirectory(test)
