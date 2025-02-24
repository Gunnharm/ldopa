///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing EventLogTsGrVizExporter module
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>


// test settings file
#include "tsbuilder_test_settings.h"
//#include "trss/tsbuilder_test_settings.h"

// ldopa
#include "xi/ldopa/ts/algos/obsolete1/eventlogts_grviz_exporter.h"      // module under test
#include "xi/ldopa/utils.h"
//===================================================================================

const char* GRVIZ_FILE1 = CSVLOG1_TEST_LOGS_BASE_DIR "GrViz/of1-1.gv";
const char* GRVIZ_FILE2 = CSVLOG1_TEST_LOGS_BASE_DIR "GrViz/of1-2.gv";

//==============================================================================
// class EventLogTsDotWriter
//==============================================================================

TEST(GraphDotWriter, open1)
{
    using namespace xi::ldopa;

    // default constructor
    GraphDotWriter dw1;
    EXPECT_EQ(false, dw1.isFileOpen());
    ASSERT_THROW(dw1.openFile(), LdopaException);

    // open with filename
    GraphDotWriter dw2(nullptr, GRVIZ_FILE1);
    EXPECT_EQ(true, dw2.openFile());
    EXPECT_EQ(true, dw2.isFileOpen());
    dw2.closeFile();
    EXPECT_EQ(false, dw2.isFileOpen());
}


TEST(GraphDotWriter, write1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    EventLogTs ts;
    ts.setName("Test TS");
    xi::strutils::SetStrPool& acts = *(ts.getActivitiesPool());

    // create a new init state
    BaseListBGraph::Vertex initSt = ts.getOrAddNewInitState();

    // create a new state "st1"
    BaseListBGraph::Vertex st1 = ts.getOrAddNewStateById(TsPtrStateId(acts["a"]));
    BaseListBGraph::Edge tr1 = ts.getOrAddNewTransition(initSt, st1, acts["a"]);

    // create a new state "st2"
    BaseListBGraph::Vertex st2 = ts.getOrAddNewStateById(TsPtrStateId(acts["b"]));
    BaseListBGraph::Edge tr2 = ts.getOrAddNewTransition(initSt, st2, acts["b"]);
    
    // accepting for st2
    EventLogTs::StateFlags& st2fl = ts.getOrAddNewStateFlags(st2);
    st2fl.setAccepting(true);

    // create a transition st1 -> st2
    BaseListBGraph::Edge tr1_2 = ts.getOrAddNewTransition(st1, st2, acts["lambda"]);

    //--------------------------
    EventLogTsGrVisitor grvi(&ts);
    GraphDotWriter dw(&grvi, GRVIZ_FILE2);

    dw.write();
}

