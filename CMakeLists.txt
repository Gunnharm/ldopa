cmake_minimum_required(VERSION 3.10)

project(ldopa)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost 1.60 REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found! install with something like sudo apt-get install libboost-all-dev")
endif()

add_subdirectory(library/src)