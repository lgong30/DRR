set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)


set(SOURCE_FILES drr_churning_rate.cpp)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

find_package(Boost COMPONENTS system REQUIRED)

if (WIN32)
    find_library(WS2_32_LIBRARY ws2_32)
endif(WIN32)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Fatal error: Boost (version >= 1.55) required.")
else()
    message(STATUS "Setting up BOOST")
    message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
    message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    add_executable(drr_churning_rate ${SOURCE_FILES})
    if (WIN32)
        target_link_libraries(drr_churning_rate ${Boost_LIBRARIES} wsock32 ws2_32)
    else()
        target_link_libraries(drr_churning_rate ${Boost_LIBRARIES})
    endif(WIN32)
endif (NOT Boost_FOUND)