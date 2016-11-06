
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.60 COMPONENTS system REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARY_DIRS})

set(SOURCE_FILES measureArrivalRate.cpp)
add_executable(mar ${SOURCE_FILES})
target_link_libraries(mar ${Boost_LIBRARIES})