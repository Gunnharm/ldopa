add_subdirectory(eventlog)
add_subdirectory(graphs)
add_subdirectory(ts)

add_library(ldopa-lib
    $<TARGET_OBJECTS:ldopa-lib-module-ts>
    $<TARGET_OBJECTS:ldopa-lib-module-graphs>
    $<TARGET_OBJECTS:ldopa-lib-module-eventlog>
    utils.cpp
)

target_link_libraries(ldopa-lib PRIVATE SQLite::SQLite3)
target_link_libraries(ldopa-lib PUBLIC xilib)
