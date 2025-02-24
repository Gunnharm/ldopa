///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing EventogTs module
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/ts/models/eventlog_ts.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"

//==============================================================================
// class BaseEventLogTS
//==============================================================================

// идентификатор состояния как список атрибутов
TEST(EventLogTs2, AttrListStateId1)
{
    using namespace xi::ldopa::ts;

    // empty
    AttrListStateId s1, s2;

    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 != s2);

    s1.append(42);
    EXPECT_FALSE(s1 == s2);
    s2.append(42);
    EXPECT_TRUE(s1 == s2);

    // разные типы
    s1.append(10);
    s2.append("10");
    EXPECT_FALSE(s1 == s2);
}

//-----------------------------------------------------------------------------

// конструирует TS с дефолтным начальным состоянием
TEST(EventLogTs2, simpleWithInit)
{
    using namespace xi::ldopa::ts;

    AttrListStateIDsPool pool;

    BaseEventLogTS ts1(&pool);
    EXPECT_EQ(1, ts1.getStatesNum());
    EXPECT_EQ(0, ts1.getTransitionsNum());
    EXPECT_TRUE(pool.getInitStateId() == ts1.getInitStateID());   // вот теперь такое соотношение   
}

//-----------------------------------------------------------------------------

// проверяет, что дефолтное начальное будет соответствовать пустому ид
TEST(EventLogTs2, simpleAddEmpty1)
{
    using namespace xi::ldopa::ts;

    AttrListStateIDsPool pool;
    BaseEventLogTS ts1(&pool);
    EXPECT_EQ(1, ts1.getStatesNum());
    EXPECT_EQ(1, pool.getSize());

    // ид состояний
    AttrListStateId emptyStID;                              // новый пустой ИД состояния
    EXPECT_EQ(0, emptyStID.getAttrs().size());              // в нем нет составл. атрибутов
    const AttrListStateId* stId_empty = pool[emptyStID];    // добавляем в лист, должен 
                                                            // соответствовать начальному
    const IStateId* stId_init = ts1.getInitStateID();
    EXPECT_TRUE(stId_empty == stId_init);                   // пустой и нач. ид должны быть равны
    
    // непосредственно состояния
    BaseEventLogTS::State st_empty = ts1.getOrAddState(stId_empty);
    BaseEventLogTS::State st_init = ts1.getInitState();
    EXPECT_TRUE(st_init == st_empty);                       // постое и нач. состояния д.б. равны
}

//-----------------------------------------------------------------------------

// конструирует TS с несколькими состояниями и переходами
TEST(EventLogTs2, simpleTS1)
{
    using namespace xi::ldopa::ts;

    AttrListStateIDsPool pool;
    BaseEventLogTS ts1(&pool);

    //AttrListStateId si
    const AttrListStateId* stId_a = pool[{ "a" }];
    const AttrListStateId* stId_b = pool[{ "b" }];
    const AttrListStateId* stId_ac = pool[{ "a", "c" }];
    
    // тестируем текстовое представление меток
    std::string st_a_s = stId_a->toString();
    std::string st_b_s = stId_b->toString();
    std::string st_ac_s = stId_ac->toString();

    EXPECT_EQ("[a]", st_a_s);
    EXPECT_EQ("[b]", st_b_s);
    EXPECT_EQ("[a, c]", st_ac_s);

    EXPECT_EQ(4, pool.getSize());               // в пуле
    EXPECT_EQ(1, ts1.getStatesNum());           // в TS

    BaseEventLogTS::State st_a = ts1.getOrAddState(stId_a);
    BaseEventLogTS::State st_b = ts1.getOrAddState(stId_b);
    BaseEventLogTS::State st_ac = ts1.getOrAddState(stId_ac);
    EXPECT_EQ(4, ts1.getStatesNum());           // в TS теперь тоже 4

    // теперь те же метки состояний, только через TS
    EXPECT_EQ("[a]", ts1.makeStateIDStr(st_a));
    EXPECT_EQ("[b]", ts1.makeStateIDStr(st_b));
    EXPECT_EQ("[a, c]", ts1.makeStateIDStr(st_ac));

    // попробуем независимо получить три добавленных состояния
    BaseEventLogTS::StateRes st_a_r = ts1.getState(stId_a);
    BaseEventLogTS::StateRes st_b_r = ts1.getState(stId_b);
    BaseEventLogTS::StateRes st_ac_r = ts1.getState(stId_ac);
    EXPECT_TRUE(st_a_r.second);
    EXPECT_TRUE(st_b_r.second);
    EXPECT_TRUE(st_ac_r.second);
    EXPECT_TRUE(st_a_r.first == st_a);
    EXPECT_TRUE(st_b_r.first == st_b);
    EXPECT_TRUE(st_ac_r.first == st_ac);

    BaseEventLogTS::State st_init = ts1.getInitState();

    // наводим дуги!
    EXPECT_EQ(0, ts1.getTransitionsNum());      // пока их нет
    BaseEventLogTS::Transition ta = ts1.getOrAddTrans(st_init, st_a, "a");
    BaseEventLogTS::Transition tb = ts1.getOrAddTrans(st_init, st_b, "b");
    BaseEventLogTS::Transition ta_ac = ts1.getOrAddTrans(st_a, st_ac, "c");
    EXPECT_EQ(3, ts1.getTransitionsNum());      // три новодобавленные

    // получение состояний, для которых наведена дуга (10/11/2017)
    EXPECT_EQ(st_a, ts1.getSrcState(ta_ac));
    EXPECT_EQ(st_ac, ts1.getTargState(ta_ac));


    // получение меток на дугах через TS (10.11.2017)
    EXPECT_EQ("a", ts1.getTransLbl(ta).toString());
    EXPECT_EQ("b", ts1.getTransLbl(tb).toString());
    EXPECT_EQ("c", ts1.getTransLbl(ta_ac).toString());

    // число образующих трасс
    EXPECT_EQ(0, ts1.getTracesNum());
    ts1.setTracesNum(42);
    EXPECT_EQ(42, ts1.getTracesNum());
}


//-----------------------------------------------------------------------------

// копируем TS с несколькими состояниями и переходами
TEST(EventLogTs2, simpleTSCopyConstr1)
{
    using namespace xi::ldopa::ts;

    AttrListStateIDsPool pool;
    BaseEventLogTS ts1(&pool);

    BaseEventLogTS::State st_init = ts1.getInitState();

    // добавляем состояния
    BaseEventLogTS::State st_a = ts1.getOrAddState(pool[{ "a" }]);
    BaseEventLogTS::State st_b = ts1.getOrAddState(pool[{ "b" }]);
    BaseEventLogTS::State st_ac = ts1.getOrAddState(pool[{ "a", "c" }]);
    
    // добавляем переходы
    BaseEventLogTS::Transition ta = ts1.getOrAddTrans(st_init, st_a, "a");
    BaseEventLogTS::Transition tb = ts1.getOrAddTrans(st_init, st_b, "b");
    BaseEventLogTS::Transition ta_ac = ts1.getOrAddTrans(st_a, st_ac, "c");
    
    EXPECT_EQ(4, ts1.getStatesNum());           // 4 состояния
    EXPECT_EQ(3, ts1.getTransitionsNum());      // 3 перехода
    EXPECT_EQ(&pool, ts1.getStateIDsPool());

    //--<создаем новый — КК>--
    BaseEventLogTS ts2(ts1);
    EXPECT_EQ(4, ts2.getStatesNum());           // тоже 4 состояния
    EXPECT_EQ(3, ts2.getTransitionsNum());      // и 3 перехода

    // проверяем пул
    EXPECT_EQ(&pool, ts2.getStateIDsPool());
    EXPECT_TRUE(ts2.getStateIDsPool() == ts1.getStateIDsPool());

    // операция присваивания
    BaseEventLogTS ts3(&pool);
    EXPECT_EQ(1, ts3.getStatesNum());
    EXPECT_EQ(0, ts3.getTransitionsNum());

    ts3 = ts2;
    EXPECT_EQ(4, ts3.getStatesNum());           // тоже 4 состояния
    EXPECT_EQ(3, ts3.getTransitionsNum());      // и 3 перехода
}

//-----------------------------------------------------------------------------

// дублирование атрибутов 1
TEST(EventLogTs2, duplicateAttrsInPool1)
{
    using namespace xi::ldopa::ts;
    using xi::ldopa::eventlog::IEventLog;
    using namespace xi::attributes;

    AttrListStateIDsPool pool;
    BaseEventLogTS ts1(&pool);

    BaseEventLogTS::State st_init = ts1.getInitState();

    // сперва непосредственно ИДшники
    EXPECT_EQ(1, pool.getSize());
    
    AttrListStateId av1;
    av1.getAttrs().push_back(IEventLog::Attribute(StringSharedPtr(new std::string("Abc"))));
    const AttrListStateId* aid_a1 = pool[av1]; // pool[{ "a" }];
    EXPECT_EQ(2, pool.getSize());

    AttrListStateId av2;
    av2.getAttrs().push_back(IEventLog::Attribute(StringSharedPtr(new std::string("Abc"))));
    const AttrListStateId* aid_a2 = pool[av2]; //pool[{ "a" }];            // д.б. тот же самый!!!
    EXPECT_EQ(2, pool.getSize());
}

//==============================================================================
// class AttrListStateIDsPool
//==============================================================================


// конструирует TS с дефолтным начальным состоянием
TEST(EventLogTs2, attrListStateIDsPool1)
{
    using namespace xi::ldopa::ts;

    AttrListStateIDsPool pool;
}