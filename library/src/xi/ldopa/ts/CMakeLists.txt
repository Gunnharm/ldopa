file(GLOB SRC_FILES "models/*.cpp" "algos/*.cpp")

add_library(ldopa-lib-module-ts OBJECT ${SRC_FILES})