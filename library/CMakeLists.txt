if(NOT WIN32)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(tests/gtest)