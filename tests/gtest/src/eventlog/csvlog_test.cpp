////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Testing `csvlog' module
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      23.02.2016
/// \copyright (c) xidv.ru 2014–2016.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// HOW TO SET A TEST ENVIRONMENT:
/// one need to define CSVLOG1_TEST_LOGS_BASE_DIR directive to set it to a
/// machine/user specific value
///
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/eventlog/obsolete1/csvlog.h"
#include "xi/ldopa/eventlog/obsolete1/csvlog_events.h"
#include "xi/ldopa/utils.h"

// test settings file
//#include "xi/ldopa/eventlog/csvlog_test_settings.h"
#include "eventlog/csvlog_test_settings.h"

// std
#include <fstream>
#include <set>
#include <memory>

// Tests CSVLog class. Part 1
class CSVLog_1_Test : public ::testing::Test {
public:
    //-----<Types>-----
    typedef std::shared_ptr<xi::ldopa::eventlog::obsolete1::ITrace::IEventEnumerator> IEventEnumShPtr;
public:
    // base log files directory: need to be set precisely according to individual user settings
    // add / to the end of the dir name!
    //static const char* LOGS_BASE_DIR;
    
    // test log file 1
    static const char*  LOG_FILE1;
    static const char*  LOG_FILE_RTS_04;                // log04.csv from reduce transition systems topic
    static const size_t LOG_FILE_RTS_04_EVENTSNUM;      // number of traces

    static const char*  LOG_FILE_RTS_04_1;              // log04-1.csv from reduce transition systems topic
    static const char*  LOG_FILE_RTS_04_1_1;            // log04-1-1.csv with unsorted events
    static const size_t LOG_FILE_RTS_04_1_TRACESNUM;    // number of traces
    static const size_t LOG_FILE_RTS_04_1_EVENTSNUM;    // number of traces

    static const char*  LOG_FILE_RTS_04_2;              // log04-2.csv with extended attributes
    static const size_t LOG_FILE_RTS_04_2_TRACESNUM;    // number of traces
    static const size_t LOG_FILE_RTS_04_2_EVENTSNUM;    // number of traces
    static const size_t LOG_FILE_RTS_04_2_ATTRSNUM;     // number of attributes

protected:
    virtual void SetUp()
    {
        //SetUpGraph();
    } // virtual void SetUp() 

    virtual void TearDown()
    {
        //delete _gr1;
    }

}; // class CSVLog_1_Test



// ---<do not change followings>
const char* CSVLog_1_Test::LOG_FILE1        = CSVLOG1_TEST_LOGS_BASE_DIR "logs/simple.csv";
const char* CSVLog_1_Test::LOG_FILE_RTS_04  = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04.csv";
const size_t CSVLog_1_Test::LOG_FILE_RTS_04_EVENTSNUM = 12; // excluding 1 more for header
const char* CSVLog_1_Test::LOG_FILE_RTS_04_1 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-1.csv";
const char* CSVLog_1_Test::LOG_FILE_RTS_04_1_1 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-1-1.csv";
const size_t CSVLog_1_Test::LOG_FILE_RTS_04_1_EVENTSNUM = 12; 
const size_t CSVLog_1_Test::LOG_FILE_RTS_04_1_TRACESNUM = 4;

const char* CSVLog_1_Test::LOG_FILE_RTS_04_2 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-2.csv";
const size_t CSVLog_1_Test::LOG_FILE_RTS_04_2_EVENTSNUM = 12;
const size_t CSVLog_1_Test::LOG_FILE_RTS_04_2_TRACESNUM = 4;
const size_t CSVLog_1_Test::LOG_FILE_RTS_04_2_ATTRSNUM = 5;


//===================================================================================


/** \brief Simple implementation of ICSVLogAttrCacher, which does nothing */
class SimpleTestAttrCacher : public xi::ldopa::eventlog::obsolete1::ICSVLogAttrCacher
{
public:
    SimpleTestAttrCacher()
        : _parseEventCounter(0), _invalidateAllEventCounter(0)
    {}
public:
    virtual void parseEventLine(xi::ldopa::eventlog::obsolete1::IEvent* ev, xi::ldopa::eventlog::obsolete1::ITrace* tr, 
        xi::ldopa::eventlog::obsolete1::IEventLog::CaseID caseId, std::string& line, 
        xi::strutils::VectorOfPairOfIndices& attrPairs, std::streamoff offs) override
    {
        //int a = 0;
        ++_parseEventCounter;
    }

    virtual void invalidateAllEvents() override
    {
        ++_invalidateAllEventCounter;
        //int a = 0;
    }

    virtual void invalidateEvent(xi::ldopa::eventlog::obsolete1::IEvent* ev) override
    {
    }
public:
    int _parseEventCounter;
    int _invalidateAllEventCounter;
}; // class SimpleTestAttrCacher

//------------- Tests itselves --------------

TEST_F(CSVLog_1_Test, simplyOpenFile)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;
    
    SimpleCSVLogEventCreator evcr;
    CSVLog log1(CSVLog_1_Test::LOG_FILE1, &evcr);
    
    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(log1.isOpen(), true);

    log1.close();
    EXPECT_EQ(log1.isOpen(), false);
    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(log1.isOpen(), true);
}

TEST_F(CSVLog_1_Test, openFileThatNotExists)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator;
    CSVLog log1(std::string("that_file_should_not_exists.ext"), &evCreator);
    ASSERT_THROW(log1.open(), LdopaException);
}


TEST_F(CSVLog_1_Test, openWithExistingStream)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    std::ifstream file(CSVLog_1_Test::LOG_FILE1);
    SimpleCSVLogEventCreator evCreator;
    CSVLog log1(&file, &evCreator);

    EXPECT_EQ(log1.isOpen(), true);
    EXPECT_EQ(log1.open(), true);
}

TEST_F(CSVLog_1_Test, defSettingsInit)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    CSVLog log1;
    EXPECT_EQ(log1.isFirstLineAsHeader(), true);
}


TEST_F(CSVLog_1_Test, setGetFileName)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    CSVLog log1;
    EXPECT_EQ(log1.getFileName(), std::string());

    log1.setFileName(CSVLog_1_Test::LOG_FILE1);
    EXPECT_EQ(log1.getFileName(), std::string(CSVLog_1_Test::LOG_FILE1));

    // open with a newly set filename
    EXPECT_EQ(log1.open(), true);

    // try to change a filename while open (an exc should be)
    ASSERT_THROW(log1.setFileName("newfilename.txt"), LdopaException);    

    // try to change a filename while closed
    log1.close();
    log1.setFileName("newfilename1.txt");
    EXPECT_EQ(log1.getFileName(), std::string("newfilename1.txt"));
}


TEST_F(CSVLog_1_Test, caseMapping1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator;
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04, &evCreator);
    ITraces* trcs = log1.getTraces();

    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(trcs);
    EXPECT_NE(csvTraces, nullptr);
    
    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
}

TEST_F(CSVLog_1_Test, logStructure1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evcr;
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_1, &evcr);
    ITraces* trcs = log1.getTraces();

    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(trcs);
    EXPECT_NE(csvTraces, nullptr);

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);

    size_t trNum = trcs->getNumOfTraces();
    EXPECT_EQ(trNum, LOG_FILE_RTS_04_1_TRACESNUM);
}


TEST_F(CSVLog_1_Test, logTraceAutoPreparing1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // &log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_1_1, &evCreator);
    
    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(log1.getTraces());
    EXPECT_NE(csvTraces, nullptr);

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);

    EXPECT_EQ(csvTraces->getNumOfTraces(), LOG_FILE_RTS_04_1_TRACESNUM);

    // look for non existing trace 42:
    ITrace* tr42 = (*csvTraces)[42];
    EXPECT_EQ(tr42, nullptr);

    // here we expect any trace unprepared
    ITrace* tr1 = (*csvTraces)[1];
    EXPECT_NE(tr1, nullptr);
    CSVLogTrace* tr1_csv = dynamic_cast<CSVLogTrace*>(tr1);
    EXPECT_NE(tr1_csv, nullptr);

    // should be unprepared here
    EXPECT_EQ(tr1_csv->isPrepared(), false);
    
    IEvent* ev1 = (*tr1_csv)[0];
    EXPECT_NE(ev1, nullptr);

    // now, trace shoud has got prepared
    EXPECT_EQ(tr1_csv->isPrepared(), true);
}


TEST_F(CSVLog_1_Test, enumEvents1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // (&log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_1_1, &evCreator);
    
    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(log1.getTraces());

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(csvTraces->getNumOfTraces(), LOG_FILE_RTS_04_1_TRACESNUM);
   
    // here we expect any trace unprepared
    ITrace* tr1 = (*csvTraces)[1];          // a trace for case with ID == 1 (not simly the very first trace)

    ITrace::IEventEnumerator* en = tr1->enumerateEvents();

    int i = 0;
    while (en->hasNext())
    {
        IEvent* ev = en->getNext();
        ++i;
    }
    EXPECT_EQ(i, 3);        // 3 events into 1-st trace is expected

    delete en;

    // solution with shared_ptr and more abstract type
    typedef std::shared_ptr<ITrace::IEventEnumerator> IEventEnumShPtr;
    IEventEnumShPtr enp((*log1.getTraces())[4]->enumerateEvents());
    i = 0;
    while (enp->hasNext())
    {
        IEvent* ev = enp->getNext();
        ++i;
    }
    EXPECT_EQ(i, 3);        // 3 events into 1-st trace is expected
}


TEST_F(CSVLog_1_Test, enumTraces1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // (&log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_1_1, &evCreator);

    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(log1.getTraces());

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(csvTraces->getNumOfTraces(), LOG_FILE_RTS_04_1_TRACESNUM);

    ITraces::ITraceEnumerator* tn = log1.getTraces()->enumerateTraces();

    int i = 0;
    while (tn->hasNext())
    {
        ITrace* tr = tn->getNext();
        ++i;
    }
    EXPECT_EQ(i, LOG_FILE_RTS_04_1_TRACESNUM);        // the same 4 traces

    delete tn;

    // solution with shared_ptr and more abstract type
    typedef std::shared_ptr<ITraces::ITraceEnumerator> ITraceEnumShPtr;
    ITraceEnumShPtr tnp(log1.getTraces()->enumerateTraces());
    i = 0;
    while (tnp->hasNext())
    {
        ITrace* tr = tnp->getNext();
        ++i;
    }
    EXPECT_EQ(i, LOG_FILE_RTS_04_1_TRACESNUM);        // the same 4 traces
}


TEST_F(CSVLog_1_Test, attrHeaders)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // (&log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_2, &evCreator);

    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(log1.getTraces());

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(csvTraces->getNumOfTraces(), LOG_FILE_RTS_04_2_TRACESNUM);

    // number of attributes
    EXPECT_EQ(log1.getAttrHeaders().size(), LOG_FILE_RTS_04_2_ATTRSNUM);
}


TEST_F(CSVLog_1_Test, extractStrAttr1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // (&log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_2, &evCreator);
    
    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(log1.getTraces());

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(csvTraces->getNumOfTraces(), LOG_FILE_RTS_04_2_TRACESNUM);
    
    // get a trace for the case 1
    ITrace* tr1 = (*csvTraces)[1];
    
    // mention first trace has a sequence of events ordered as follows: <1, 0, 2>
    // extracts attribute # 3
    std::string atr1_3;
    bool aeres1_3 = (*tr1)[0]->getAttrAsStr(3, atr1_3);
    EXPECT_EQ(aeres1_3, true);
    EXPECT_EQ(atr1_3, "sdf");

    // extracts attribute # 4
    std::string atr1_4;
    bool aeres1_4 = (*tr1)[0]->getAttrAsStr(4, atr1_4);
    EXPECT_EQ(aeres1_4, true);
    EXPECT_EQ(atr1_4, "654");
}


TEST_F(CSVLog_1_Test, attrCacher1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // (&log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_2, &evCreator);
    
    SimpleTestAttrCacher attrCacher;
    log1.setEventAttrCacher(&attrCacher);

    // do mapping prior to prepare cases
    CSVLogTraces* csvTraces = dynamic_cast<CSVLogTraces*>(log1.getTraces());

    // here we have a more typed tracer
    csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
    //EXPECT_EQ(csvTraces->getNumOfTraces(), LOG_FILE_RTS_04_2_TRACESNUM);

    // simply counts how many times eventAttrExtractor has been invoked
    // technically, invalidateAll is invoked for the first time during file open procedure
    EXPECT_EQ(attrCacher._parseEventCounter, LOG_FILE_RTS_04_2_EVENTSNUM);
    EXPECT_EQ(attrCacher._invalidateAllEventCounter, 0);

    // manually close the file
    log1.close();
    EXPECT_EQ(attrCacher._invalidateAllEventCounter, 1);    // one more when close
}

TEST_F(CSVLog_1_Test, eventActName)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::eventlog::obsolete1;

    SimpleCSVLogEventCreator evCreator; // (&log1);
    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04_1_1, &evCreator);
    
    // do mapping prior to prepare cases
    CSVLogTraces& traces = *log1.getTracesLocal();
    traces.mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure

    EXPECT_EQ(log1.open(), true);
    EXPECT_EQ(traces.getNumOfTraces(), LOG_FILE_RTS_04_1_TRACESNUM);

    // here we expect any trace unprepared
    ITrace& tr1 = *traces[1];          // a trace for case with ID == 1 (not simly the very first trace)
    std::string actName = tr1[0]->getActivityName();
    EXPECT_EQ(actName, "a");
}

