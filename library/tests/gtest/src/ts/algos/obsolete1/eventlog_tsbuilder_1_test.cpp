///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing BidiGraph with fixture 1
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/ts/algos/obsolete1/eventlogts_builder.h"
#include "xi/ldopa/eventlog/obsolete1/csvlog.h"
#include "xi/ldopa/eventlog/obsolete1/csvlog_events.h"

// test settings file
#include "tsbuilder_test_settings.h"

// Tests BidiGraph class 
class EventLogTsBuilder_1Test : public ::testing::Test {
public:
    //-----<Consts>-----
    static const char*  LOG_FILE_RTS_04_2;              // log04-2.csv with extended attributes
public:
    // constructor
    EventLogTsBuilder_1Test()
        : ::testing::Test(),
        log4_1_evCreator(),
        log4_1(EventLogTsBuilder_1Test::LOG_FILE_RTS_04_2, &log4_1_evCreator),
        //log4_1_evCreator(&log4_1),
        log4_1_traces(*log4_1.getTracesLocal())
    {
        using namespace xi::ldopa;

        // do mapping prior to prepare cases
        eventlog::obsolete1::CSVLogTraces* csvTraces = 
            dynamic_cast<eventlog::obsolete1::CSVLogTraces*>(log4_1.getTraces());
        csvTraces->mapMainAttributes(0, 2, 1);  // accordint to log04.csv structure
    }
protected:
    virtual void SetUp() 
    {
        SetUpLog4_1();
    } // virtual void SetUp() 

    virtual void TearDown() {}

    void SetUpLog4_1()
    {
    }

protected:
    xi::ldopa::eventlog::obsolete1::SimpleCSVLogEventCreator log4_1_evCreator;
    xi::ldopa::eventlog::obsolete1::CSVLog log4_1;    
    xi::ldopa::eventlog::obsolete1::CSVLogTraces& log4_1_traces;
}; // class TsBuilder_1Test

const char* EventLogTsBuilder_1Test::LOG_FILE_RTS_04_2 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-2.csv";

//===================================================================================


TEST_F(EventLogTsBuilder_1Test, simple)
{
    int a = 0;
}

TEST_F(EventLogTsBuilder_1Test, TsBuilderBasics1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    EXPECT_EQ(log4_1.open(), true);

    EventLogFixWsTsBuilder tsb(&log4_1);
    EventLogTs* resTs = tsb.getResTs();
    EXPECT_EQ(resTs, nullptr);
    EXPECT_EQ(tsb.isResDetached(), false);  
}

TEST_F(EventLogTsBuilder_1Test, stateId1)
{
    // one need to exclude this test later
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;
    using namespace xi::ldopa::eventlog::obsolete1;


    EXPECT_EQ(log4_1.open(), true);
    EventLogFixWsTsBuilder tsb(&log4_1);
    //log4_1_traces[]
    ITrace* tr1 = log4_1_traces[1];
    
    TsPtrStateId stId1 = EventLogFixWsTsBuilder::makeStateId(tr1,
        EventLogFixWsTsBuilder::WS_INF, 2, log4_1.getActivitiesPoolLocal());           
}

TEST_F(EventLogTsBuilder_1Test, buildInfWs1)
{
    // one need to exclude this test later
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;
    using namespace xi::ldopa::eventlog::obsolete1;

    EXPECT_EQ(log4_1.open(), true);
    EventLogFixWsTsBuilder tsb(&log4_1);
    //log4_1_traces[]
    ITrace* tr1 = log4_1_traces[1];

    EventLogTs* resTs = tsb.build(EventLogFixWsTsBuilder::WS_INF);
    EXPECT_EQ(10, resTs->getStatesNum());
    EXPECT_EQ(9, resTs->getEdgesNum());

    // expect an accepting mark from the very last (accepting) states
    xi::strutils::SetStrPool& acts = *resTs->getActivitiesPool();
    TsPtrStateId stid { acts["a"], acts["b"], acts["c"] };

    // looking for a state with ID stid
    EventLogTs::StateRetResult stR = resTs->getState(stid);
    EXPECT_EQ(true, stR.second);        // should existing
    
    // states flags
    EventLogTs::StateFlags fl;
    EXPECT_EQ(true, resTs->getStateFlags(stR.first, fl));
    EXPECT_EQ(EventLogTs::StateFlags::triTrue, fl.getAccepting());
}

