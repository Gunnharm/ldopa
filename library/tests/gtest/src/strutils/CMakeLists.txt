add_executable(XilibStrutilsTests 
    search_algos_test.cpp
    set_str_pool_test.cpp
    substr_test.cpp
)

target_link_libraries(XilibStrutilsTests GTest::GTest GTest::Main)
target_link_libraries(XilibStrutilsTests xilib)

add_test(NAME XilibStrutilsTests COMMAND ./library/tests/gtest/src/strutils/XilibStrutilsTests)
