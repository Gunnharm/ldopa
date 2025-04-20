file(GLOB SRC_FILES "sqlite/*.cpp" "obsolete1/*.cpp" "csvlog/*.cpp")

add_library(ldopa-lib-module-eventlog OBJECT ${SRC_FILES})