add_executable(XilibCoreTests 
    attributes/boost_attr_1_test.cpp

    collections/custom_enums.cpp
    collections/enumerators_1_test.cpp
)

target_link_libraries(XilibCoreTests GTest::GTest GTest::Main)
target_link_libraries(XilibCoreTests xilib)

add_test(NAME XilibCoreTests COMMAND ./library/tests/gtest/src/core/XilibCoreTests)
