add_executable(LdopaLibGraphsTests 
    bidigraph_1_test.cpp
    bidigraph_2_test.cpp
    bidigraph_test.cpp
)

target_link_libraries(LdopaLibGraphsTests GTest::GTest GTest::Main)
target_link_libraries(LdopaLibGraphsTests ldopa-lib)

add_test(NAME LdopaLibGraphsTests COMMAND LdopaLibGraphsTests)
