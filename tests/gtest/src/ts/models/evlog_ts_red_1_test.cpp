///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing EventogTs module
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"

//==============================================================================
// class EvLogTSWithFreqs
//==============================================================================

// копируем TS с несколькими состояниями и переходами
TEST(EventLogTsFreq1, simpleTSCopyConstr1)
{
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;

    AttrListStateIDsPool pool;    
    TS ts1(&pool);

    TS::State st_init = ts1.getInitState();

    // добавляем состояния
    TS::State st_a = ts1.getOrAddState(pool[{ "a" }]);
    TS::State st_b = ts1.getOrAddState(pool[{ "b" }]);
    ts1.setAcceptingState(st_b, true);                      // спец. свойство вершины

    // добавляем переходы
    TS::Transition ta = ts1.getOrAddTrans(st_init, st_a, "a");
    //TS::Transition tb = ts1.getOrAddTrans(st_init, st_b, "b");
    TS::Transition tb = ts1.getOrAddTransF(st_init, st_b, "b", 42);
    //ts1.setTransFreq(tb, 42);                               // спец. свойство дуги

    // доп. атрибуты
    ts1.setMaxWS(42);

    EXPECT_EQ(3, ts1.getStatesNum());           // 3 состояния
    EXPECT_EQ(2, ts1.getTransitionsNum());      // 2 перехода

    // создаем новый — КК
    TS ts2(ts1);
    EXPECT_EQ(3, ts2.getStatesNum());           // тоже 3 состояния
    EXPECT_EQ(2, ts2.getTransitionsNum());      // и 2 перехода

    // а проверим-ка структуру вершин-дуг: совпадает ли?!
    // состояния
    TS::LogTS::StateIter st1, st1End, st2, st2End;
    boost::tie(st1, st1End) = ts1.getStates();
    boost::tie(st2, st2End) = ts2.getStates();
    for (; st1 != st1End; ++st1, ++st2)
        EXPECT_TRUE(ts1.getStateID(*st1) == ts2.getStateID(*st2));

    // дуги
    TS::LogTS::TransIter tr1, tr1End, tr2, tr2End;
    boost::tie(tr1, tr1End) = ts1.getTransitions();
    boost::tie(tr2, tr2End) = ts2.getTransitions();
    for (; tr1 != tr1End; ++tr1, ++tr2)
    {
        EXPECT_TRUE(ts1.getStateID(ts1.getSrcState(*tr1)) == ts2.getStateID(ts2.getSrcState(*tr2)));
        EXPECT_TRUE(ts1.getStateID(ts1.getTargState(*tr1)) == ts2.getStateID(ts2.getTargState(*tr2)));
    }
    
    // проверяем спец. свойства (частотные) вершин и дуг
    TS::State st2_b = ts2.getState(pool[{ "b" }]).first;
    TS::Transition t2b = ts2.getTrans(ts2.getInitState(), st2_b, "b").first;
    
    EXPECT_TRUE(ts2.isStateAccepting(st2_b));           // этот должен быть принимающий
    EXPECT_TRUE(ts2.getTransFreq(t2b).second);
    EXPECT_EQ(42, ts2.getTransFreq(t2b).first);         // а у этой быть частота уст.

    // доп. параметры!
    EXPECT_TRUE(ts2.getMaxWS() == ts1.getMaxWS());
    EXPECT_EQ(42, ts2.getMaxWS());                      // а у этой быть частота уст.

    // операция присваивания
    TS ts3(&pool);
    EXPECT_EQ(1, ts3.getStatesNum());
    EXPECT_EQ(0, ts3.getTransitionsNum());

    ts3 = ts2;
    EXPECT_EQ(3, ts3.getStatesNum());           // тоже 3 состояния
    EXPECT_EQ(2, ts3.getTransitionsNum());      // и 2 перехода
}

//-----------------------------------------------------------------------------

// добавляем несколько раз одну и ту же транзицию, наблюдая, какая частота
TEST(EventLogTsFreq1, addMultipleTrans1)
{
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;

    AttrListStateIDsPool pool;
    TS ts1(&pool);

    TS::State st_init = ts1.getInitState();
    
    // добавляем состояния
    TS::State st_a = ts1.getOrAddState(pool[{ "a" }]);
    TS::State st_b = ts1.getOrAddState(pool[{ "b" }]);
    EXPECT_EQ(3, ts1.getStatesNum());                       // 3 состояния

    // добавляем переходы с частотами и без
    TS::Transition ta = ts1.getOrAddTransF(st_init, st_a, "a", 1);
    TS::Transition tb = ts1.getOrAddTrans(st_init, st_b, "b");    
    EXPECT_EQ(2, ts1.getTransitionsNum());                  // 2 перехода

    TS::IntRes ta_fr = ts1.getTransFreq(ta);                // для первого есть
    EXPECT_TRUE(ta_fr.second);
    EXPECT_EQ(1, ta_fr.first);
    
    TS::IntRes tb_fr = ts1.getTransFreq(tb);                // для второго нет
    EXPECT_FALSE(tb_fr.second);

    // еще раз с ними же
    ts1.getOrAddTransF(st_init, st_a, "a", 2);
    ts1.getOrAddTransF(st_init, st_b, "b", 2);
    EXPECT_EQ(2, ts1.getTransitionsNum());                  // 2 перехода по-прежнему

    ta_fr = ts1.getTransFreq(ta);                           // для первого 3
    EXPECT_TRUE(ta_fr.second);
    EXPECT_EQ(3, ta_fr.first);

    tb_fr = ts1.getTransFreq(tb);                           // для второго 2
    EXPECT_TRUE(tb_fr.second);
    EXPECT_EQ(2, tb_fr.first);
}

//-----------------------------------------------------------------------------

// добавляем несколько раз одну и ту же транзицию, наблюдая, какая частота
TEST(EventLogTsFreq1, acceptingState1)
{
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;

    AttrListStateIDsPool pool;
    TS ts1(&pool);

    TS::State st_init = ts1.getInitState();

    // добавляем состояния
    TS::State st_a = ts1.getOrAddState(pool[{ "a" }]);
    TS::State st_b = ts1.getOrAddState(pool[{ "b" }]);
    EXPECT_EQ(3, ts1.getStatesNum());                       // 3 состояния

    EXPECT_FALSE(ts1.isStateAccepting(st_a));
    EXPECT_FALSE(ts1.isStateAccepting(st_b));

    ts1.setAcceptingState(st_a, true);
    EXPECT_TRUE(ts1.isStateAccepting(st_a));
    ts1.setAcceptingState(st_a, false);
    EXPECT_FALSE(ts1.isStateAccepting(st_a));
}
