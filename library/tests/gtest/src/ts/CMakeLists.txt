add_executable(LdopaLibTSTests 
    algos/evlog_ts_red_dotwriter_1_test.cpp
    algos/ts_misc_converters_1_test.cpp
    algos/ts_simple_builder_1_test.cpp

    models/eventlog_ts_2_test.cpp
    models/evlog_ts_red_1_test.cpp
    models/labeledts_1_test.cpp
)

target_link_libraries(LdopaLibTSTests GTest::GTest GTest::Main)
target_link_libraries(LdopaLibTSTests ldopa-lib)

add_test(NAME LdopaLibTSTests COMMAND ./library/tests/gtest/src/ts/LdopaLibTSTests)
