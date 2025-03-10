file(GLOB SRC_FILES "sqlite/*.cpp")

add_library(ldopa-lib-module-eventlog OBJECT ${SRC_FILES})