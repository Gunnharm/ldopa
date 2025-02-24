////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for gen-ts-with-freq DOT (graphviz) writer.
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

#include "../ts_test_settings.h"

// std
#include <iostream>     // std::endl

//==============================================================================
// class EventLogTsFreqDotWriter1
//==============================================================================

TEST(EventLogTsFreqDotWriter1, simpleFreqTs1)
{
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;

    AttrListStateIDsPool pool;
    TS ts1(&pool);

    TS::State st_init = ts1.getInitState();

    // добавляем состояния
    TS::State st_a = ts1.getOrAddState(pool[{ "a" }]);
    TS::State st_b = ts1.getOrAddState(pool[{ "b" }]);
    ts1.setAcceptingState(st_a, true);

    // добавляем переходы с частотами и без
    TS::Transition ta = ts1.getOrAddTransF(st_init, st_a, "a", 1);
    TS::Transition tb = ts1.getOrAddTrans(st_init, st_b, "b");

    EXPECT_EQ(3, ts1.getStatesNum());                       // 3 состояния
    EXPECT_EQ(2, ts1.getTransitionsNum());                  // 2 перехода

    // записыватель
    //typedef typename EvLogTSWithFreqsDotWriter TsDotWriter;
    EvLogTSWithFreqsDotWriter dw;
    dw.write(TS_TEST_MODELS_BASE_DIR "ts/EventLogTsFreqDotWriter1-acceptingState1.gv", ts1);
}
