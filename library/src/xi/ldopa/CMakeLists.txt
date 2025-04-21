add_subdirectory(eventlog)
add_subdirectory(graphs)
add_subdirectory(ts)
add_subdirectory(pn)

set(SOURCES
    $<TARGET_OBJECTS:ldopa-lib-module-ts>
    $<TARGET_OBJECTS:ldopa-lib-module-graphs>
    $<TARGET_OBJECTS:ldopa-lib-module-eventlog>
    $<TARGET_OBJECTS:ldopa-lib-module-pn>
    utils.cpp
)

add_library(ldopa-lib STATIC ${SOURCES})

add_library(ldopa-lib-shared SHARED ${SOURCES})
target_compile_definitions(ldopa-lib-shared PRIVATE XILIB_DLLб XILIB_DLL_EXPORTS)
install(TARGETS ldopa-lib-shared
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
)

target_link_libraries(ldopa-lib PRIVATE SQLite::SQLite3)
target_link_libraries(ldopa-lib PUBLIC xilib)

target_link_libraries(ldopa-lib-shared PRIVATE SQLite::SQLite3)
target_link_libraries(ldopa-lib-shared PUBLIC xilib)
