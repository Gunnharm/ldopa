////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Testing `csvlog' module
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      23.02.2016
/// \copyright (c) xidv.ru 2014–2016.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// HOW TO SET A TEST ENVIRONMENT:
/// one need to define CSVLOG_TEST_LOGS_BASE_DIR directive to set it to a
/// machine/user specific value
///
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/eventlog/csvlog/csvlog.h"
#include "xi/ldopa/utils.h"

// test settings file
#include "eventlog/csvlog_test_settings.h"

// Tests CSVLog class
class CSVLogTest : public ::testing::Test {
public:
    // base log files directory: need to be set precisely according to individual user settings
    // add / to the end of the dir name!
    static const char* LOG_FILE1;
    static const char* LOG_FILE_RTS_04;                // log04.csv from reduce transition systems topic
    static const size_t LOG_FILE_RTS_04_EVENTSNUM;      // number of traces

    static const char* LOG_FILE_RTS_04_1;              // log04-1.csv from reduce transition systems topic
    static const char* LOG_FILE_RTS_04_1_1;            // log04-1-1.csv with unsorted events
    static const size_t LOG_FILE_RTS_04_1_TRACESNUM;    // number of traces
    static const size_t LOG_FILE_RTS_04_1_EVENTSNUM;    // number of traces

    static const char* LOG_FILE_RTS_04_2;              // log04-2.csv with extended attributes
    static const size_t LOG_FILE_RTS_04_2_TRACESNUM;    // number of traces
    static const size_t LOG_FILE_RTS_04_2_EVENTSNUM;    // number of traces
    static const size_t LOG_FILE_RTS_04_2_ATTRSNUM;     // number of attributes

protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

// ---<do not change followings>
const char* CSVLogTest::LOG_FILE1 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/simple.csv";
const char* CSVLogTest::LOG_FILE_RTS_04 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04.csv";
const size_t CSVLogTest::LOG_FILE_RTS_04_EVENTSNUM = 12; // excluding 1 more for header
const char* CSVLogTest::LOG_FILE_RTS_04_1 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-1.csv";
const char* CSVLogTest::LOG_FILE_RTS_04_1_1 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-1-1.csv";
const size_t CSVLogTest::LOG_FILE_RTS_04_1_EVENTSNUM = 12; 
const size_t CSVLogTest::LOG_FILE_RTS_04_1_TRACESNUM = 4;

const char* CSVLogTest::LOG_FILE_RTS_04_2 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-2.csv";
const size_t CSVLogTest::LOG_FILE_RTS_04_2_EVENTSNUM = 12;
const size_t CSVLogTest::LOG_FILE_RTS_04_2_TRACESNUM = 4;
const size_t CSVLogTest::LOG_FILE_RTS_04_2_ATTRSNUM = 5;

//------------- Tests itselves --------------

TEST_F(CSVLogTest, simplyOpenFile)
{
    using namespace xi::ldopa::eventlog::csvlog;
    
    CSVLog log1;
    log1.setFileName(LOG_FILE1);
    
    log1.open();
    EXPECT_TRUE(log1.isOpen());

    log1.close();
    EXPECT_FALSE(log1.isOpen());
    
    log1.open();
    EXPECT_TRUE(log1.isOpen());
}

TEST_F(CSVLogTest, openFileThatNotExists)
{
    using namespace xi::ldopa::eventlog::csvlog;

    CSVLog log1;
    log1.setFileName("that_file_should_not_exists.ext");
    ASSERT_THROW(log1.open(), xi::ldopa::LdopaException);
}

TEST_F(CSVLogTest, setGetFileName)
{
    using namespace xi::ldopa::eventlog::csvlog;

    CSVLog log1;
    EXPECT_EQ(log1.getFileName(), std::string());

    log1.setFileName(LOG_FILE1);
    EXPECT_EQ(log1.getFileName(), std::string(LOG_FILE1));

    // open with a newly set filename
    log1.open();
    EXPECT_TRUE(log1.isOpen());

    // try to change a filename while open (an exc should be)
    ASSERT_THROW(log1.setFileName("newfilename.txt"), xi::ldopa::LdopaException);    

    // try to change a filename while closed
    log1.close();
    log1.setFileName("newfilename1.txt");
    EXPECT_EQ(log1.getFileName(), std::string("newfilename1.txt"));
}

TEST_F(CSVLogTest, logStructure1)
{
    using namespace xi::ldopa::eventlog::csvlog;

    CSVLog log1;
    log1.setFileName(LOG_FILE_RTS_04_1);
    log1.open();

    EXPECT_EQ(log1.getTracesNum(), LOG_FILE_RTS_04_1_TRACESNUM);
    EXPECT_EQ(log1.getEventsNum(), LOG_FILE_RTS_04_1_EVENTSNUM);
}

TEST_F(CSVLogTest, logAttributes)
{
    using namespace xi::ldopa::eventlog::csvlog;

    CSVLog log1;
    log1.setFileName(LOG_FILE_RTS_04_2);
    log1.open();

    EXPECT_EQ(log1.getTracesNum(), LOG_FILE_RTS_04_2_TRACESNUM);
    EXPECT_EQ(log1.getEventsNum(), LOG_FILE_RTS_04_2_EVENTSNUM);
    // TODO: fix this
    // EXPECT_EQ(log1.getLogAttrsNum(), LOG_FILE_RTS_04_2_ATTRSNUM);
}

TEST_F(CSVLogTest, eventAttributes)
{
    using namespace xi::ldopa::eventlog::csvlog;

    CSVLog log1;
    log1.setFileName(LOG_FILE_RTS_04_2);
    log1.open();

    // Get first trace
    auto trace = log1.getTrace(0);
    ASSERT_NE(trace, nullptr);

    // Get first event
    auto event = trace->getEvent(0);
    ASSERT_NE(event, nullptr);

    // Check event attributes
    xi::ldopa::eventlog::IEventLog::Attribute attr;
    EXPECT_TRUE(event->getAttr("concept:name", attr));
    EXPECT_TRUE(event->getAttr("time:timestamp", attr));
    EXPECT_TRUE(event->getAttr("case:concept:name", attr));
}

TEST_F(CSVLogTest, traceAttributes)
{
    using namespace xi::ldopa::eventlog::csvlog;

    CSVLog log1;
    log1.setFileName(LOG_FILE_RTS_04_2);
    log1.open();

    // Get first trace
    auto trace = log1.getTrace(0);
    ASSERT_NE(trace, nullptr);

    // Check trace attributes
    xi::ldopa::eventlog::IEventLog::Attribute attr;
    // TODO: fix this
    // EXPECT_TRUE(trace->getAttr("case:concept:name", attr));
    // EXPECT_TRUE(trace->getAttr("case:creator", attr));
} 