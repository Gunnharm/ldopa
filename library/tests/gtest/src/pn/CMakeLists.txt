add_executable(LdopaLibPNTests 
    algos/base_ptnet_dotwriter_1_test.cpp
    algos/pn_synthesis_1_test.cpp

    models/base_ptnet_1_test.cpp
    models/base_ptnet_1_test.cpp
    models/base_ptnet_1_test.cpp
)

target_link_libraries(LdopaLibPNTests GTest::GTest GTest::Main)
target_link_libraries(LdopaLibPNTests ldopa-lib)

add_test(NAME LdopaLibPNTests COMMAND ./library/tests/gtest/src/pn/LdopaLibPNTests)
