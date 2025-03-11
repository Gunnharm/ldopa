find_package(GTest REQUIRED)

include_directories(../../src)

enable_testing()
include_directories(src)

add_compile_definitions(
    TS_TEST_MODELS_BASE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/work_files/"
    CSVLOG1_TEST_LOGS_BASE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/work_files/"
    TS_TEST_MODELS_BASE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/work_files/"
)

add_subdirectory(src/ts)
