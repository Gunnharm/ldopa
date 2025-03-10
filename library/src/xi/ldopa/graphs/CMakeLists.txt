file(GLOB SRC_FILES "*.cpp" "algos/grviz/*.cpp")

add_library(ldopa-lib-module-graphs OBJECT ${SRC_FILES})