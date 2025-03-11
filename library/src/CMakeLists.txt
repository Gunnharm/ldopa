include_directories(.)

add_subdirectory(xi/attributes)
add_subdirectory(xi/strutils)

add_library(xilib
    $<TARGET_OBJECTS:xilib-attributes>
    $<TARGET_OBJECTS:xilib-strutils>
)

add_subdirectory(xi/ldopa)
