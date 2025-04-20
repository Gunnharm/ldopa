add_executable(LdopaLibEventlogTests 
    csvlog_test.cpp
obsolete_csvlog_test.cpp
    sqlitelog_test.cpp
)

target_link_libraries(LdopaLibEventlogTests GTest::GTest GTest::Main)
target_link_libraries(LdopaLibEventlogTests ldopa-lib)

add_test(NAME LdopaLibEventlogTests COMMAND LdopaLibEventlogTests)
