add_executable(LdopaLibPerformanceTests
    performance_1_test.cpp
)

target_link_libraries(LdopaLibPerformanceTests GTest::GTest GTest::Main)
target_link_libraries(LdopaLibPerformanceTests ldopa-lib)

add_test(NAME LdopaLibPerformanceTests COMMAND LdopaLibPerformanceTests)
