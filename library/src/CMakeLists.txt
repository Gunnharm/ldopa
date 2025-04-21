include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(xi/attributes)
add_subdirectory(xi/strutils)

add_library(xilib
    STATIC
    $<TARGET_OBJECTS:xilib-attributes>
    $<TARGET_OBJECTS:xilib-strutils>
)

add_subdirectory(xi/ldopa)
