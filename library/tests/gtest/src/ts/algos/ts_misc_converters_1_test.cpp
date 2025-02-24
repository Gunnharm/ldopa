////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for misc TS converters.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      31.07.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
///
/// Notes on how to avoid using underscores in test case names: 
/// https://groups.google.com/forum/#!topic/googletestframework/N5A07bgEvp4
///
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"
#include "xi/ldopa/ts/algos/grviz/evlog_ts_red_dotwriter.h"

#include "xi/ldopa/ts/algos/ts_sloops_converter.h"

#include "../ts_test_settings.h"

// std
#include <iostream>     // std::endl

//==============================================================================
// class EventLogTsFreqDotWriter1
//==============================================================================

TEST(TsMiscConverters1, selfLoopEliminate1)
{
    using namespace xi::ldopa::ts;
    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Attribute;
    typedef std::string String;

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    TS::State s8 = ts.getOrAddState(pool[{ "s8" }]);
    TS::State s9 = ts.getOrAddState(pool[{ "s9" }]);

    //TS::State send = ts.getOrAddState(pool[{ "send" }]);    
    ts.setAcceptingState(s6, true);
    ts.setAcceptingState(s8, true);

    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("d"));
    TS::Transition t4 = ts.getOrAddTrans(s4, s8, String("e"));
    TS::Transition t5 = ts.getOrAddTrans(s1, s5, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("d"));
    TS::Transition t7 = ts.getOrAddTrans(s6, s7, String("c"));    
    TS::Transition t8 = ts.getOrAddTrans(s7, s8, String("f"));
    TS::Transition t9 = ts.getOrAddTrans(s6, s9, String("x"));

    // петли
    TS::Transition t10 = ts.getOrAddTrans(s1, s1, String("loop"));
    TS::Transition t11 = ts.getOrAddTrans(s3, s3, String("loop"));
    TS::Transition t12 = ts.getOrAddTrans(s6, s6, String("loop"));
    TS::Transition t13 = ts.getOrAddTrans(s8, s8, String("loop"));

    EXPECT_EQ(9, ts.getStatesNum());
    EXPECT_EQ(13, ts.getTransitionsNum());

    // записыватель
    EvLogTSWithFreqsDotWriter dw;
    dw.write(TS_TEST_MODELS_BASE_DIR "ts/MiscTsConverters1-selfLoopEliminate1-a.gv", ts);

    // удалятель петель
    typedef TsSLoopsConverter<TS> TsSLoopsEliminator;
    TsSLoopsEliminator conv;
    EXPECT_EQ(4, conv.convert(&ts));            // 4 петли должна убрать

    dw.write(TS_TEST_MODELS_BASE_DIR "ts/MiscTsConverters1-selfLoopEliminate1-b.gv", ts);
}
