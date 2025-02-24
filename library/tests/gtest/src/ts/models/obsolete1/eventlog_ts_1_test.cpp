///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing EventogTs module
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "xi/ldopa/ts/models/obsolete1/basets.h"
#include "xi/ldopa/ts/models/obsolete1/eventlogts.h"

//==============================================================================
// class EventLogTs
//==============================================================================

TEST(EventLogTs1, add1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;
    
    EventLogTs ts1;

    EXPECT_EQ(0, ts1.getStIdVertBimap().size()); // initially no elements

    // adds 1 element
    int a = 12, b = 14, c = 15;

    TsPtrStateId stId1   { &a, &b };
    BaseListBGraph::Vertex v1 = ts1.getOrAddNewStateById(stId1);
    EXPECT_EQ(1, ts1.getStIdVertBimap().size()); 

    // no new element should be added
    BaseListBGraph::Vertex v1_1 = ts1.getOrAddNewStateById(stId1);
    EXPECT_EQ(1, ts1.getStIdVertBimap().size());
    EXPECT_EQ(v1, v1_1);                        // should be the same

    TsPtrStateId stId2{ &a, &b, &c };
    BaseListBGraph::Vertex v2 = ts1.getOrAddNewStateById(stId2);
    EXPECT_EQ(2, ts1.getStIdVertBimap().size()); // 2 distinguished elemets are expected
    EXPECT_NE(v1, v2);                          // should not be the same
}


TEST(EventLogTs1, defState1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    EventLogTs ts;
    EXPECT_EQ(0, ts.getStIdVertBimap().size());      // initially no elements

    BaseListBGraph::VertexRetResult initStR = ts.getInitState();
    EXPECT_EQ(initStR.second, false);               // should not be an initial state

    // creates a new init state
    BaseListBGraph::Vertex initSt = ts.getOrAddNewInitState();
    initStR = ts.getInitState();
    EXPECT_EQ(initStR.second, true);               // should be the initial state
    EXPECT_EQ(initStR.first, initSt);

    // tests if a state with default ID is the initial one
    TsPtrStateId defStId;
    BaseListBGraph::VertexRetResult initStR2 = ts.getState(defStId);
    EXPECT_EQ(initStR2.second, true);               
    EXPECT_EQ(initStR.first, initStR2.first);

    // gets init states secondary
    initSt = ts.getOrAddNewInitState();
    EXPECT_EQ(initStR.first, initSt);               // should be the same as above
}


TEST(EventLogTs1, addTrans2)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    EventLogTs ts;
    EXPECT_EQ(0, ts.getStIdVertBimap().size());      // initially no elements

    // create a new init state
    BaseListBGraph::Vertex initSt = ts.getOrAddNewInitState();
    BaseListBGraph::VertexRetResult initStR = ts.getInitState();
    EXPECT_EQ(initStR.second, true);               // should be the initial state
    EXPECT_EQ(initStR.first, initSt);

    // creates IDs
    const std::string* st1s = ts.getActivitiesPool()->insert("st1");//*(ts.getActivitiesPool())["st1"];
    const std::string* st2s = ts.getActivitiesPool()->insert("st2");//*(ts.getActivitiesPool())["st2"];


    // create a new state "1"
    BaseListBGraph::Vertex st1 = ts.getOrAddNewStateById(TsPtrStateId(st1s));
    EXPECT_EQ(2, ts.getStIdVertBimap().size());      // 2 elements expected

    // there are still no transitions
    EXPECT_EQ(0, ts.getTransActivitiesMap().size());

    BaseListBGraph::Edge tr1_1 = ts.getOrAddNewTransition(initSt, st1, st1s);
    EXPECT_EQ(1, ts.getTransActivitiesMap().size());
    const std::string* tr1_1_act = ts.getTransActivity(tr1_1);
    EXPECT_EQ(st1s, tr1_1_act);


    // trying one more for the same pair of vertices, no overwriting symbol
    BaseListBGraph::Edge tr1_2 = ts.getOrAddNewTransition(initSt, st1, st2s);
    EXPECT_EQ(1, ts.getTransActivitiesMap().size());
    const std::string* tr1_2_act = ts.getTransActivity(tr1_2);
    EXPECT_EQ(st1s, tr1_2_act);


    // trying one more for the same pair of vertices, do overwriting symbol
    BaseListBGraph::Edge tr1_3 = ts.getOrAddNewTransition(initSt, st1, st2s, true);
    EXPECT_EQ(1, ts.getTransActivitiesMap().size());
    const std::string* tr1_3_act = ts.getTransActivity(tr1_3);
    EXPECT_EQ(st2s, tr1_3_act);

}

TEST(EventLogTs1, freqAttribute1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    EventLogTs ts;
    EXPECT_EQ(0, ts.getStIdVertBimap().size());      // initially no elements


    // creates IDs
    const std::string* st1s = ts.getActivitiesPool()->insert("st1");//*(ts.getActivitiesPool())["st1"];
    const std::string* st2s = ts.getActivitiesPool()->insert("st2");//*(ts.getActivitiesPool())["st2"];
    const std::string* lambdaS = ts.getActivitiesPool()->insert("lambda");


    // create a new state "st1"
    BaseListBGraph::Vertex st1 = ts.getOrAddNewStateById(TsPtrStateId(st1s));
    EXPECT_EQ(1, ts.getStIdVertBimap().size());      // 2 elements expected

    // create a new state "st2"
    BaseListBGraph::Vertex st2 = ts.getOrAddNewStateById(TsPtrStateId(st2s));
    EXPECT_EQ(2, ts.getStIdVertBimap().size());      // 2 elements expected

    // add a transition between st1 and st2
    BaseListBGraph::Edge tr1_1 = ts.getOrAddNewTransition(st1, st2, lambdaS);
    EXPECT_EQ(1, ts.getTransActivitiesMap().size());

    // frequency attribute
    EventLogTs::IntRetResult res = ts.getTransFreq(tr1_1);
    EXPECT_EQ(false, res.second);

    // add new freq
    ts.setTransFreq(tr1_1, 42);
    res = ts.getTransFreq(tr1_1);
    EXPECT_EQ(true, res.second);
    EXPECT_EQ(42, res.first);
}


TEST(EventLogTs1, stateFlags1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    EventLogTs::StateFlags fl;
    size_t flSize = sizeof(fl);
    
    EventLogTs::StateFlags::TriState flSt = fl.getAccepting();
    EXPECT_EQ(EventLogTs::StateFlags::triNotSet, flSt);

    // set to true
    fl.setAccepting(true);
    flSt = fl.getAccepting();
    EXPECT_EQ(EventLogTs::StateFlags::triTrue, flSt);

    // set to false
    fl.setAccepting(false);
    flSt = fl.getAccepting();
    EXPECT_EQ(EventLogTs::StateFlags::triFalse, flSt);

    // clear flag
    fl.clearAccepting();
    flSt = fl.getAccepting();
    EXPECT_EQ(EventLogTs::StateFlags::triNotSet, flSt);
}

TEST(EventLogTs1, stateFlagsAttribute1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    EventLogTs ts;
    EXPECT_EQ(0, ts.getStIdVertBimap().size());      // initially no elements

    // creates IDs
    const std::string* st1s = ts.getActivitiesPool()->insert("st1");
    const std::string* st2s = ts.getActivitiesPool()->insert("st2");

    // create a new state "st1"
    EventLogTs::State st1 = ts.getOrAddNewStateById(TsPtrStateId(st1s));
    EXPECT_EQ(1, ts.getStatesNum());      // 1 element expected

    EventLogTs::StateFlags fl;
    EXPECT_EQ(false, ts.getStateFlags(st1, fl));
    

    // set to true
    fl.setAccepting(true);
    ts.setStateFlags(st1, fl);
    EXPECT_EQ(true, ts.getStateFlags(st1, fl));
    EventLogTs::StateFlags::TriState flSt = fl.getAccepting();
    EXPECT_EQ(EventLogTs::StateFlags::triTrue, flSt);



    //---state 2---
    // create a new state "st2"
    EventLogTs::State st2 = ts.getOrAddNewStateById(TsPtrStateId(st2s));
    EXPECT_EQ(2, ts.getStatesNum());      // 2 element expected
    
    EventLogTs::StateFlags& fl2 = ts.getOrAddNewStateFlags(st2);
    fl2.setAccepting(false);
    EXPECT_EQ(true, ts.getStateFlags(st2, fl));     // mind, that is fl, not fl2!
    
    flSt = fl.getAccepting();
    EXPECT_EQ(EventLogTs::StateFlags::triFalse, flSt);
}
