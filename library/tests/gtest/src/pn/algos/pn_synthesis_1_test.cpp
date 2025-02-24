////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for PnRegSynthesizer.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.08.2018
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
#include "xi/ldopa/utils.h"
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"
#include "xi/ldopa/pn/models/base_ptnet.h"
#include "xi/ldopa/pn/algos/regions/pn_synthesis.h"
#include "xi/ldopa/ts/algos/grviz/evlog_ts_red_dotwriter.h"
#include "xi/ldopa/ts/algos/ts_sloops_converter.h"
                                                                // EventLog Petrinets
#include "xi/ldopa/pn/models/evlog_ptnets.h"                    // модель PN
#include "xi/ldopa/pn/algos/grviz/evlog_ptnets_dotwriter.h"     // записыватель в DOT для PN

#include "../pn_test_settings.h"

#include "pn_synthesis_ext.h"                           // расширение класса для тестирования закр. членов

//==============================================================================
// class PnRegSynthesizer1
//==============================================================================


TEST(PnRegSynthesizer1, simpleTs1)
{    
    // ts1: /0.1/docs/imgs/ts1.pdf 
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State st_init = ts.getInitState();

    // Creates a TS that is isomorphic to one depicted in Fig. 2 in [Cortadella, 98] (p. 862)
    // добавляем состояния
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);       
    ts.setAcceptingState(s6, true);

    // переходы
    TS::Transition t1 = ts.getOrAddTrans(st_init, s2, "a");
    TS::Transition t2 = ts.getOrAddTrans(st_init, s3, "b");
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, "c");
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, "c");
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, "b");
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, "a");

    EXPECT_EQ(6, ts.getStatesNum());
    EXPECT_EQ(6, ts.getTransitionsNum());

    // картинку дампнем
    EvLogTSWithFreqsDotWriter dw;
    dw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-simpleTs1.gv", ts);


    //=====[ синтезатор ]=====
    //typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    PNSynth synth(&ts);

    EXPECT_EQ(0, synth.getLbl2Transs().size());     // нет еще
    synth._t_mapTransitions();
    EXPECT_EQ(3, synth.getLbl2Transs().size());     // по числу классов активностей


    // переберем их и руками посмотрим (тут же ведь не создается копия value?!)
    for (const auto& par : synth.getLbl2Transs())
    {
        const PNSynth::SplitLabel& e = par.first;
        const PNSynth::TransSet& ter = par.second;   // соотве. ER(e)
        std::string evStr = e.label().toString();
        size_t erSize = ter.size();
    }

    // перебираем событи с помощью спец. итератора
    PNSynth::Lbl2TransSetMap::KeyIterator evIt, evEnd;    
    boost::tie(evIt, evEnd) = synth.getLbl2Transs().keys();
    for (; evIt != evEnd; ++evIt)
    {
        //const TS::TrLabel& e = *evIt;
        const PNSynth::SplitLabel& e = *evIt;
        std::string evStr = e.label().toString();
    }

    // теперь перебираем все те же события, полученные с помощью хелперного метода
    PNSynth::LblKeyTrSetIter evIt2, evEnd2;
    boost::tie(evIt2, evEnd2) = synth.getEvents2();
    for (; evIt2 != evEnd2; ++evIt2)
    {
        //const TS::TrLabel& e = *evIt2;
        const PNSynth::SplitLabel& e = *evIt2;
        std::string evStr = e.label().toString();
    }
}

//-----------------------------------------------------------------------------

TEST(PnRegSynthesizer1, isRegion1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 

    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();

    // Creates a TS that is isomorphic to one depicted in Fig. 2 in [Cortadella, 98] (p. 862)
    // добавляем состояния
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    ts.setAcceptingState(s6, true);

    // переходы
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, "a");
    TS::Transition t2 = ts.getOrAddTrans(s1, s3, "b");
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, "c");
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, "c");
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, "b");
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, "a");

    EXPECT_EQ(6, ts.getStatesNum());
    EXPECT_EQ(6, ts.getTransitionsNum());

    //=====[ синтезатор ]=====
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    PNSynth synth(&ts);

    synth._t_mapTransitions();
    EXPECT_EQ(3, synth.getLbl2Transs().size());     // по числу классов активностей
    
    // для системы переходов ts определено 8 регионов, см. ts1: /0.1/docs/imgs/ts1.pdf 
    // перечислим их    
    PNSynth::StateSet r1 { s1, s3, s5 };
    PNSynth::StateSet r2 { s1, s2, s4 };
    PNSynth::StateSet r3 { s2, s3, s6 };
    PNSynth::StateSet r4 { s1, s4, s5 };
    PNSynth::StateSet r5 { s1, s2, s3 };
    PNSynth::StateSet r6 { s4, s5, s6 };
    PNSynth::StateSet r7 { s2, s4, s6 };
    PNSynth::StateSet r8 { s3, s5, s6 };
    PNSynth::StateSet r10 { };                  // два тривиальных региона
    PNSynth::StateSet r11 { s1, s2, s3, s4, s5, s6 };

    EXPECT_TRUE(synth.isRegion(r1));
    EXPECT_TRUE(synth.isRegion(r2));
    EXPECT_TRUE(synth.isRegion(r3));
    EXPECT_TRUE(synth.isRegion(r4));
    EXPECT_TRUE(synth.isRegion(r5));
    EXPECT_TRUE(synth.isRegion(r6));
    EXPECT_TRUE(synth.isRegion(r7));
    EXPECT_TRUE(synth.isRegion(r8));
    EXPECT_TRUE(synth.isRegion(r10));           // тривиальные регионы
    EXPECT_TRUE(synth.isRegion(r11));

    // а теперь потестим нерегионы
    PNSynth::StateSet nr1 { s1, s3 };           //  "a" одновременно и выходит, и вовне
    PNSynth::StateSet nr2 { s1, s3, s5, s6 };   // "a" выходит и выходит, "b" внутри и входит
    PNSynth::StateSet nr3 { s1  };              // "a", "b" и выходят, и вовне
    PNSynth::StateSet nr4 { s2, s3, s4, s5  };  // "a", "b" и входят, и выходят
    PNSynth::StateSet nr5 { s6  };              // 
    PNSynth::StateSet nr6 { s5  };              // 
    PNSynth::StateSet nr7 { s5, s6 };           // 
    EXPECT_FALSE(synth.isRegion(nr1));
    EXPECT_FALSE(synth.isRegion(nr2));
    EXPECT_FALSE(synth.isRegion(nr3));
    EXPECT_FALSE(synth.isRegion(nr4));
    EXPECT_FALSE(synth.isRegion(nr5));
    EXPECT_FALSE(synth.isRegion(nr6));
    EXPECT_FALSE(synth.isRegion(nr7));
}

//-----------------------------------------------------------------------------

TEST(PnRegSynthesizer1, checkSubSuperSets1)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;            // для синтезатора надо

    AttrListStateIDsPool pool;
    TS ts(&pool);

    // нужны просто состояния, не более того
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);

    //=====[ синтезатор ]=====
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    PNSynth synth(&ts);

    // проверяем вложенность множеств
    PNSynth::StateSet r1 { s1, s2, s3, s4 };
    PNSynth::StateSet r2 { s1, s2, s3 };
    PNSynth::StateSet r3 { s1, s2, s4 };
    PNSynth::StateSet r4 { s1, s2, s3, s4 };
    PNSynth::StateSet r5 { s2, s3, s4 };
    PNSynth::StateSet r6 { s1, s3 };
    PNSynth::StateSet r7 { s1, s2, s5 };
    PNSynth::StateSet r8 { s1, s2, s3, s5 };

    // все множества r2 - r6 д.б. подмножествами множества r1
    EXPECT_TRUE(synth.isSubset(r1, r2));
    EXPECT_TRUE(synth.isSubset(r1, r3));
    EXPECT_TRUE(synth.isSubset(r1, r4));
    EXPECT_TRUE(synth.isSubset(r1, r5));
    EXPECT_TRUE(synth.isSubset(r1, r6));

    // для надмножеств непосредственно через множества
    EXPECT_TRUE(r1.isSupersetOf(r2));
    EXPECT_TRUE(r1.isSupersetOf(r3));
    EXPECT_TRUE(r1.isSupersetOf(r4));
    EXPECT_TRUE(r1.isSupersetOf(r5));
    EXPECT_TRUE(r1.isSupersetOf(r6));

    // а вот это уже не подмножество!
    EXPECT_FALSE(synth.isSubset(r1, r7));

    // теперь со строгим сравнением
    EXPECT_TRUE(synth.isStrictSubset(r1, r2));
    EXPECT_TRUE(synth.isStrictSubset(r1, r3));
    EXPECT_FALSE(synth.isStrictSubset(r1, r4));     // вот это уже не действительно
    EXPECT_TRUE(synth.isStrictSubset(r1, r5));
    EXPECT_TRUE(synth.isStrictSubset(r1, r6));
    EXPECT_FALSE(synth.isStrictSubset(r1, r7));     // это не подмножество
    EXPECT_FALSE(synth.isStrictSubset(r1, r8));     // и это не подмножество

    // для строгих надмножеств через множества
    EXPECT_TRUE (r1.isStrictSupersetOf(r2));
    EXPECT_TRUE (r1.isStrictSupersetOf(r3));
    EXPECT_FALSE(r1.isStrictSupersetOf(r4));        // вот это уже не действительно
    EXPECT_TRUE (r1.isStrictSupersetOf(r5));
    EXPECT_TRUE (r1.isStrictSupersetOf(r6));
    EXPECT_FALSE(r1.isStrictSupersetOf(r7));        // это не подмножество
    EXPECT_FALSE(r1.isStrictSupersetOf(r8));        // и это не подмножество

    EXPECT_TRUE (r2.isStrictSubsetOf(r1));
    EXPECT_TRUE (r3.isStrictSubsetOf(r1));
    EXPECT_FALSE(r4.isStrictSubsetOf(r1));          // вот это уже не действительно
    EXPECT_TRUE (r5.isStrictSubsetOf(r1));
    EXPECT_TRUE (r6.isStrictSubsetOf(r1));
    EXPECT_FALSE(r7.isStrictSubsetOf(r1));          // это не подмножество
    EXPECT_FALSE(r8.isStrictSubsetOf(r1));          // и это не подмножество

}

//-----------------------------------------------------------------------------

// проверка удаления из множества множеств состояний всех надмножеств некоторого множества
TEST(PnRegSynthesizer1, removeSubsets1)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;            // для синтезатора надо

    AttrListStateIDsPool pool;
    TS ts(&pool);

    // нужны просто состояния, не более того
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);

    //=====[ синтезатор ]=====
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    PNSynth synth(&ts);

    // эталонное множество, надмножества которых мы будем удалять
    PNSynth::StateSet r { s1, s2, s3 };
    PNSynth::StateSet r1 { s1, s2, s3 };            // нестрогое надмножество, оставить
    PNSynth::StateSet r2 { s1, s2, s3, s4 };        // надмножество, удалять
    PNSynth::StateSet r3 { s1, s2, s3, s5 };        // надмножество, удалять
    PNSynth::StateSet r4 { s1, s2, s3, s4, s5 };    // надмножество, удалять
    PNSynth::StateSet r5 { s2, s3, s4, s5 };        // не надмножество, оставить

    PNSynth::StateSet r6 { s1, s3 };                // 

    PNSynth::SetOfStateSets s{ r1, r2, r3, r4, r5 };// множество множеств состояний
    EXPECT_EQ(5, s.size());

    // проверка существования подмножества
    EXPECT_FALSE(synth.subsetExists(s, r6));        // в s нет подмножест r6, только его надмножества
    EXPECT_TRUE(synth.subsetExists(s, r));          // в s есть подмножество r, а именно r1 (такое же)
    EXPECT_TRUE(synth.subsetExists(s, r));          // в s есть подмножество r2, а именно r1, r2 (само)


    EXPECT_TRUE(synth.removeAllStrictSupersets(s, r) != 0);// д.б. удаления
    EXPECT_EQ(2, s.size());                         // д.б. удалено 3, оставлено 2
}


//-----------------------------------------------------------------------------


// нахождение пересечения неск. множеств
TEST(PnRegSynthesizer1, setIntersection1)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;            // для синтезатора надо

    AttrListStateIDsPool pool;
    TS ts(&pool);

    // нужны просто состояния, не более того
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);

    //=====[ синтезатор ]=====
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    PNSynth synth(&ts);

    // эталонное множество, надмножества которых мы будем удалять
    PNSynth::StateSet r{ s1, s2, s3 };
    PNSynth::StateSet r1{ s1, s2, s3 };            //
    PNSynth::StateSet r2{ s1, s2, s3, s4 };        //
    PNSynth::StateSet r3{ s1, s2, s3, s5 };        //
    PNSynth::StateSet r4{ s1, s2, s3, s4, s5 };    //
    PNSynth::StateSet r5{ s2, s3, s4, s5 };        //
    PNSynth::StateSet r6{ s1, s3 };                //

    PNSynth::SetOfStateSets ss1 { r1, r2, r3, r4, r5 };
    EXPECT_EQ(5, ss1.size());

    // ищем разные пересечения
    PNSynth::StateSet is1 = synth.intersection(ss1);
    PNSynth::StateSet is1_e{ s2, s3 };
    EXPECT_EQ(is1_e, is1);
}

//-----------------------------------------------------------------------------

// генерация минимальных пререгионов для первой системы переходов
TEST(PnRegSynthesizer1, genMinPreRegions1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 

    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    typedef std::string String;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();

    // Creates a TS that is isomorphic to one depicted in Fig. 2 in [Cortadella, 98] (p. 862)
    // добавляем состояния
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    ts.setAcceptingState(s6, true);

    // переходы, помечаем явным образом строками!!!!!
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, String("c"));
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, String("c"));
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("a"));

    EXPECT_EQ(6, ts.getStatesNum());
    EXPECT_EQ(6, ts.getTransitionsNum());


    //=====[ синтезатор ]=====
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    PNSynth synth(&ts);

    synth._t_mapTransitions();
    EXPECT_EQ(3, synth.getLbl2Transs().size());     // по числу классов активностей

    // для системы переходов ts определено 8 регионов, см. ts1: /0.1/docs/imgs/ts1.pdf 

    // начинаем рассчитывать мин пререгионы для события "a"
    const TS::TrLabel& la = ts.getTransLbl(t1);
    std::string la_s = la.toString();    
    EXPECT_EQ("a", la_s);
    la.asStr();                                 // чтобы прикомпилировала

    const TS::TrLabel& lb = ts.getTransLbl(t2);
    const TS::TrLabel& lc = ts.getTransLbl(t3);

    synth.genMinPreRegions(la);
    synth.genMinPreRegions(lb);
    synth.genMinPreRegions(lc);

    // проверим теперь EC-свойство
    EXPECT_TRUE(synth.isExcitationClosureHold(la));
    EXPECT_TRUE(synth.isExcitationClosureHold(lb));
    EXPECT_TRUE(synth.isExcitationClosureHold(lc));
}

//-----------------------------------------------------------------------------

// генерация минимальных пререгионов для системы переходов со стр. 871 [Cortadella et al.98]
TEST(PnRegSynthesizer1, genMinPreRegions2)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 

    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    typedef std::string String;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();

    // Creates a TS that is isomorphic to one depicted in Fig. 2 in [Cortadella, 98] (p. 862)
    // добавляем состояния
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    //ts.setAcceptingState(s6, true);

    // переходы, помечаем явным образом строками!!!!!
    TS::Transition t1  = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2  = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3  = ts.getOrAddTrans(s3, s4, String("b"));
    TS::Transition t4  = ts.getOrAddTrans(s2, s5, String("d"));
    TS::Transition t5  = ts.getOrAddTrans(s2, s7, String("b"));
    TS::Transition t6  = ts.getOrAddTrans(s5, s1, String("b"));
    TS::Transition t7  = ts.getOrAddTrans(s7, s1, String("d"));
    TS::Transition t8  = ts.getOrAddTrans(s3, s6, String("d"));
    TS::Transition t9  = ts.getOrAddTrans(s6, s1, String("b"));
    TS::Transition t10 = ts.getOrAddTrans(s4, s1, String("d"));
    TS::Transition t11 = ts.getOrAddTrans(s5, s6, String("c"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(11, ts.getTransitionsNum());

    // картинку дампнем
    EvLogTSWithFreqsDotWriter dw;
    dw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-genMinPreRegions2.gv", ts);

    //=====[ синтезатор ]=====
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    PNSynth synth(&ts);

    synth._t_mapTransitions();
    EXPECT_EQ(4, synth.getLbl2Transs().size());     // по числу классов активностей

    //// для системы переходов ts определено 8 регионов, см. ts1: /0.1/docs/imgs/ts1.pdf 

    // начинаем рассчитывать мин пререгионы для события "a"
    const TS::TrLabel& la = ts.getTransLbl(t1);
    const TS::TrLabel& lb = ts.getTransLbl(t3);
    const TS::TrLabel& lc = ts.getTransLbl(t2);
    const TS::TrLabel& ld = ts.getTransLbl(t4);

    synth.genMinPreRegions(la);
    synth.genMinPreRegions(lb);
    synth.genMinPreRegions(lc);
    synth.genMinPreRegions(ld);

    // проверим теперь EC-свойство
    EXPECT_TRUE(synth.isExcitationClosureHold(la));
    EXPECT_TRUE(synth.isExcitationClosureHold(lb));
    EXPECT_FALSE(synth.isExcitationClosureHold(lc));    // не выполняется для c!
    EXPECT_TRUE(synth.isExcitationClosureHold(ld));
}


// тестирование типа SplitLabel
TEST(PnRegSynthesizer1, splitLabel1)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel/*1*/ SplitLabel;

    typedef std::string String;
    
    const TS::TrLabel& la = String("a");
    SplitLabel sl(la);
    EXPECT_TRUE(sl.first/*.get()*/ == la);
    EXPECT_TRUE(sl.label() == la);          // shortcut for first.get()
    EXPECT_EQ(0, sl.splitInd());


    SplitLabel sl2(la, 42);
    EXPECT_TRUE(sl2.label() == la);
    EXPECT_EQ(42, sl2.splitInd());

    SplitLabel sl3 = sl2;
    EXPECT_TRUE(sl3.label() == la);
    EXPECT_EQ(42, sl3.splitInd());

    EXPECT_TRUE(sl3 == sl2);
    EXPECT_FALSE(sl2 < sl3);    // они равны
    EXPECT_TRUE(sl < sl2);      // а вот 0 меньше, чем 42
    EXPECT_TRUE(sl != sl2);     // 0 не равно 42

    // мапа работает?
    typedef XiMapDecorator< std::map < SplitLabel, PNSynth::StateSet > >  Lbl2StateSetMap;    
    Lbl2StateSetMap map1;

    PNSynth::Lbl2SetOfStateSetsMap map2;
    SplitLabel ml1("a");
    SplitLabel ml2("b");
    SplitLabel ml3("a", 1);
    PNSynth::SetOfStateSets& ss1 = map2[ml1];
    PNSynth::SetOfStateSets& ss2 = map2[ml2];
    PNSynth::SetOfStateSets& ss3 = map2[ml3];
}

//-----------------------------------------------------------------------------

// тестирование SplitLabel в виде меток системы переходов
TEST(PnRegSynthesizer1, splitLabelTs1)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    //typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;

    typedef std::string String;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();

    // Creates a TS that is isomorphic to one depicted in Fig. 2 in [Cortadella, 98] (p. 862)
    // добавляем состояния
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s5, String("d"));
    TS::Transition t5 = ts.getOrAddTrans(s2, s7, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s1, String("b"));
    TS::Transition t7 = ts.getOrAddTrans(s7, s1, String("d"));
    TS::Transition t8 = ts.getOrAddTrans(s3, s6, String("d"));
    TS::Transition t9 = ts.getOrAddTrans(s6, s1, String("b"));
    TS::Transition t10 = ts.getOrAddTrans(s4, s1, String("d"));
    TS::Transition t11 = ts.getOrAddTrans(s5, s6, String("c"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(11, ts.getTransitionsNum());

    //=====[ синтезатор ]=====  
    PNSynth synth(&ts);

    // проверим, что для транзиций пока что нет сплиттинга
    EXPECT_TRUE(synth.getTransSplitLbl(t1) == SplitLabel(String("a")) );
    EXPECT_TRUE(synth.getTransSplitLbl(t1) == SplitLabel(String("a"), 0));
    EXPECT_FALSE(synth.getTransSplitLbl(t1) == SplitLabel(String("a"), 42));
    EXPECT_TRUE(synth.getTransSplitLbl(t1) != SplitLabel(String("a"), 42));

    // теперь отдельно для b-ев, которые будут расспличены
    EXPECT_TRUE(synth.getTransSplitLbl(t3) == SplitLabel(String("b")) );
    EXPECT_TRUE(synth.getTransSplitLbl(t5) == SplitLabel(String("b")) );
    EXPECT_TRUE(synth.getTransSplitLbl(t6) == SplitLabel(String("b")) );
    EXPECT_TRUE(synth.getTransSplitLbl(t9) == SplitLabel(String("b")) );

    synth._t_mapTransitions();
    EXPECT_EQ(4, synth.getLbl2Transs().size());     // по числу классов активностей

    // делаем метки спличенными
    synth._t_setTrSplitIndex(t3, 2);
    synth._t_setTrSplitIndex(t5, 1);
    synth._t_setTrSplitIndex(t6, 1);
    synth._t_setTrSplitIndex(t9, 2);
    EXPECT_FALSE(synth.getTransSplitLbl(t3) == SplitLabel(String("b")   ));
    EXPECT_FALSE(synth.getTransSplitLbl(t5) == SplitLabel(String("b")   ));
    EXPECT_FALSE(synth.getTransSplitLbl(t6) == SplitLabel(String("b")   ));
    EXPECT_FALSE(synth.getTransSplitLbl(t9) == SplitLabel(String("b")   ));
    EXPECT_TRUE (synth.getTransSplitLbl(t3) == SplitLabel(String("b"), 2));
    EXPECT_TRUE (synth.getTransSplitLbl(t5) == SplitLabel(String("b"), 1));
    EXPECT_TRUE (synth.getTransSplitLbl(t6) == SplitLabel(String("b"), 1));
    EXPECT_TRUE (synth.getTransSplitLbl(t9) == SplitLabel(String("b"), 2));

    synth._t_mapTransitions();
    EXPECT_EQ(5, synth.getLbl2Transs().size());     // по числу классов активностей

    // еще досплитим
    synth._t_setTrSplitIndex(t9, 3);
    EXPECT_TRUE (synth.getTransSplitLbl(t9) == SplitLabel(String("b"), 3));

        //synth.genExcitationRegions();
        //EXPECT_EQ(6, synth.getExcRegions().size());
    synth._t_mapTransitions();
    EXPECT_EQ(6, synth.getLbl2Transs().size());     // по числу классов активностей

}

//-----------------------------------------------------------------------------

// генерация минимальных пререгионов для системы переходов со стр. 871 [Cortadella et al.98]
// с ручным сплитом
TEST(PnRegSynthesizer1, genMinPreRegions2WithManualSplit1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef std::string String;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s5, String("d"));
    TS::Transition t5 = ts.getOrAddTrans(s2, s7, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s1, String("b"));
    TS::Transition t7 = ts.getOrAddTrans(s7, s1, String("d"));
    TS::Transition t8 = ts.getOrAddTrans(s3, s6, String("d"));
    TS::Transition t9 = ts.getOrAddTrans(s6, s1, String("b"));
    TS::Transition t10 = ts.getOrAddTrans(s4, s1, String("d"));
    TS::Transition t11 = ts.getOrAddTrans(s5, s6, String("c"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(11, ts.getTransitionsNum());

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts);

    synth._t_mapTransitions();
    EXPECT_EQ(4, synth.getLbl2Transs().size());     // по числу классов активностей

    // начинаем рассчитывать мин пререгионы для события "a"
    const TS::TrLabel& la = ts.getTransLbl(t1);
    const TS::TrLabel& lb = ts.getTransLbl(t3);
    const TS::TrLabel& lc = ts.getTransLbl(t2);
    const TS::TrLabel& ld = ts.getTransLbl(t4);

    synth.genMinPreRegions(la);    
    synth.genMinPreRegions(lb);
    synth.genMinPreRegions(lc);
    synth.genMinPreRegions(ld);

    // проверим теперь EC-свойство
    EXPECT_TRUE(synth.isExcitationClosureHold(la));
    EXPECT_TRUE(synth.isExcitationClosureHold(lb));
    EXPECT_FALSE(synth.isExcitationClosureHold(lc));    // не выполняется для c!
    EXPECT_TRUE(synth.isExcitationClosureHold(ld));

    // теперь делаем сплит b, как если бы знали, как он д.б. сделан
    synth._t_setTrSplitIndex(t3, 2);
    synth._t_setTrSplitIndex(t5, 1);
    synth._t_setTrSplitIndex(t6, 1);
    synth._t_setTrSplitIndex(t9, 2);
    EXPECT_TRUE(synth.getTransSplitLbl(t3) == SplitLabel(String("b"), 2));
    EXPECT_TRUE(synth.getTransSplitLbl(t5) == SplitLabel(String("b"), 1));
    EXPECT_TRUE(synth.getTransSplitLbl(t6) == SplitLabel(String("b"), 1));
    EXPECT_TRUE(synth.getTransSplitLbl(t9) == SplitLabel(String("b"), 2));

    synth._t_mapTransitions();
    EXPECT_EQ(5, synth.getLbl2Transs().size());     // по числу классов активностей

    synth.genMinPreRegions(la);    
    synth.genMinPreRegions(SplitLabel(String("b"), 1));
    synth.genMinPreRegions(SplitLabel(String("b"), 2));
    synth.genMinPreRegions(lc);
    synth.genMinPreRegions(ld);

    // проверим теперь EC-свойство для регионов с учетом рассплита
    EXPECT_TRUE(synth.isExcitationClosureHold(la));
    EXPECT_TRUE(synth.isExcitationClosureHold(SplitLabel(String("b"), 1)));
    EXPECT_TRUE(synth.isExcitationClosureHold(SplitLabel(String("b"), 2)));
    EXPECT_TRUE(synth.isExcitationClosureHold(lc));    // должен теперь выполняться и для c!
    EXPECT_TRUE(synth.isExcitationClosureHold(ld));

    // разберемся со счетчиками сплит-индексов напоследок
    EXPECT_EQ(0, synth.getLblSplitIndex(la));
    EXPECT_EQ(0, synth.getLblSplitIndex(lb));
    EXPECT_EQ(0, synth.getLblSplitIndex(lc));
    EXPECT_EQ(0, synth.getLblSplitIndex(ld));

    // выше там для b до 2-х досплитили, установим индекс! (через закрытый впоследствии метод...)
    synth._t_setLblSplitIndex(lb, 2);
    EXPECT_EQ(2, synth.getLblSplitIndex(lb));
}

//-----------------------------------------------------------------------------

// генерация минимальных пререгионов для системы переходов со стр. 871 [Cortadella et al.98]
// с автоматическим сплитом
TEST(PnRegSynthesizer1, genMinPreRegions2WithAutoSplit1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    typedef PnRegSynthesizer_Tester<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef std::string String;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s5, String("d"));
    TS::Transition t5 = ts.getOrAddTrans(s2, s7, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s1, String("b"));
    TS::Transition t7 = ts.getOrAddTrans(s7, s1, String("d"));
    TS::Transition t8 = ts.getOrAddTrans(s3, s6, String("d"));
    TS::Transition t9 = ts.getOrAddTrans(s6, s1, String("b"));
    TS::Transition t10 = ts.getOrAddTrans(s4, s1, String("d"));
    TS::Transition t11 = ts.getOrAddTrans(s5, s6, String("c"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(11, ts.getTransitionsNum());

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts);

    synth._t_mapTransitions();
    EXPECT_EQ(4, synth.getLbl2Transs().size());     // по числу классов активностей

    // сплит-метки!!!!
    const PNSynth::SplitLabel la = ts.getTransLbl(t1);
    const PNSynth::SplitLabel lb = ts.getTransLbl(t3);
    const PNSynth::SplitLabel lc = ts.getTransLbl(t2);
    const PNSynth::SplitLabel ld = ts.getTransLbl(t4);

    PNSynth::SetOfStateSets expla, explb, explc, expld;
    
    synth.genMinPreRegions(la, expla);
    synth.genMinPreRegions(lb, explb);      // explb пусто, т.к. равно ER(b) = (s2, s3, s5, s6)
    synth.genMinPreRegions(lc, explc);
    synth.genMinPreRegions(ld, expld);      // expld пусто, т.к. равно ER(d) = (s2, s3, s4, s7)

    EXPECT_EQ(2, expla.size());
    EXPECT_EQ(0, explb.size());
    EXPECT_EQ(5, explc.size());
    EXPECT_EQ(0, expld.size());

    // проверим теперь EC-свойство
    PNSynth::StateSet mrIntera, mrInterb, mrInterc, mrInterd;
    EXPECT_TRUE (synth.isExcitationClosureHold(la, mrIntera));
    EXPECT_TRUE (synth.isExcitationClosureHold(lb, mrInterb));
    EXPECT_FALSE(synth.isExcitationClosureHold(lc, mrInterc));    // не выполняется для c, ему сплит
    EXPECT_TRUE (synth.isExcitationClosureHold(ld, mrInterd));

    // можем проверить равенства пересечений
    PNSynth::StateSet mrIntera_exp { s1 };
    PNSynth::StateSet mrInterb_exp { s2, s3, s5, s6 };
    PNSynth::StateSet mrInterc_exp { s2, s3, s5, s6 };
    PNSynth::StateSet mrInterd_exp { s2, s3, s4, s7 };
    EXPECT_EQ(mrIntera_exp, mrIntera);
    EXPECT_EQ(mrInterb_exp, mrInterb);
    EXPECT_EQ(mrInterc_exp, mrInterc);
    EXPECT_EQ(mrInterd_exp, mrInterd);

    // надо делать сплит по событию "c", нарушающему EC-свойство
    synth.splitLabels(lc, explc, mrInterc);

    EXPECT_EQ(5, synth.getLbl2Transs().size());     // по числу классов активностей

    const PNSynth::SplitLabel lb1 = synth._t_getTransSplitLblInt(t3);    // теперь помечен как b1
    const PNSynth::SplitLabel lb2 = synth._t_getTransSplitLblInt(t5);    // теперь помечен как b2    

    PNSynth::SetOfStateSets explb1, explb2;
    expla.clear();                                  // важно, т.к. иначе остатки 
    explc.clear();                                  // с прошлого раза все испортят!
    expld.clear();
    
    synth.genMinPreRegions(la, expla);
    synth.genMinPreRegions(lb1, explb1);
    synth.genMinPreRegions(lb2, explb2);
    synth.genMinPreRegions(lc, explc);
    synth.genMinPreRegions(ld, expld);

    // еще раз рассматриваем excitation closure condition
    mrIntera.clear();                               // важно, т.к. иначе остатки 
    mrInterc.clear();                               // с прошлого раза все испортят!
    mrInterd.clear();
    PNSynth::StateSet mrInterb1, mrInterb2;
    EXPECT_TRUE(synth.isExcitationClosureHold(la, mrIntera));
    EXPECT_TRUE(synth.isExcitationClosureHold(lb1, mrInterb1));
    EXPECT_TRUE(synth.isExcitationClosureHold(lb2, mrInterb2));
    EXPECT_TRUE(synth.isExcitationClosureHold(lc, mrInterc));    // теперь выполняется!
    EXPECT_TRUE(synth.isExcitationClosureHold(ld, mrInterd));
}

//-----------------------------------------------------------------------------

// генерация валидных минимальных пререгионов для системы переходов 
// со стр. 871 [Cortadella et al.98] с автоматическим циклическим сплитом
TEST(PnRegSynthesizer1, genValidPreRegions1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 

    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    typedef EvLogTSWithFreqs TS;
    typedef MapLabeledPetriNet<> PN;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef std::string String;

    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s5, String("d"));
    TS::Transition t5 = ts.getOrAddTrans(s2, s7, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s1, String("b"));
    TS::Transition t7 = ts.getOrAddTrans(s7, s1, String("d"));
    TS::Transition t8 = ts.getOrAddTrans(s3, s6, String("d"));
    TS::Transition t9 = ts.getOrAddTrans(s6, s1, String("b"));
    TS::Transition t10 = ts.getOrAddTrans(s4, s1, String("d"));
    TS::Transition t11 = ts.getOrAddTrans(s5, s6, String("c"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(11, ts.getTransitionsNum());

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts);
    synth.genPreRegions();                          // все пререгионы с посл. сплитом
    //EXPECT_EQ(5, synth.getExcRegions().size());     // (a, b1, b2, c, d)
    EXPECT_EQ(5, synth.getLbl2Transs().size());     // по числу классов активностей
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов со стр. 871 [Cortadella et al.98]
TEST(PnRegSynthesizer1, synthesis1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 

    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;
    
    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
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
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s5, String("d"));
    TS::Transition t5 = ts.getOrAddTrans(s2, s7, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s1, String("b"));
    TS::Transition t7 = ts.getOrAddTrans(s7, s1, String("d"));
    TS::Transition t8 = ts.getOrAddTrans(s3, s6, String("d"));
    TS::Transition t9 = ts.getOrAddTrans(s6, s1, String("b"));
    TS::Transition t10 = ts.getOrAddTrans(s4, s1, String("d"));
    TS::Transition t11 = ts.getOrAddTrans(s5, s6, String("c"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(11, ts.getTransitionsNum());

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts);
    synth.synthesize();

    PN* pn = synth.getPn();


    // записыватель
    EventLogPetriNetDotWriter dw;
    dw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-synthesis1.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов  Fig. 2 in [Cortadella, 98] (p. 862)
TEST(PnRegSynthesizer1, synthesis2)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 
    // здесь будет проблема с начальной разметкой и принимающим состоянием. вот так, да!
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
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
    ts.setAcceptingState(s6, true);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, String("c"));
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, String("c"));
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("a"));
    
    EXPECT_EQ(6, ts.getStatesNum());
    EXPECT_EQ(6, ts.getTransitionsNum());

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts);
    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter dw;
    dw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-synthesis2.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов  Fig. 2 in [Cortadella, 98] (p. 862)
// добавляем искуственное конечное состояние  в автомат
TEST(PnRegSynthesizer1, synthesis2_1)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 
    // здесь будет проблема с начальной разметкой и принимающим состоянием. вот так, да!

    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
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
    TS::State send = ts.getOrAddState(pool[{ "send" }]);
    ts.setAcceptingState(send, true);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, String("c"));
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, String("c"));
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("a"));
    TS::Transition t7 = ts.getOrAddTrans(s6, send, String("-"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(7, ts.getTransitionsNum());


    //=====[ синтезатор ]=====    
    PNSynth synth(&ts);
    synth.synthesize();

    PN* pn = synth.getPn();


    // записыватель
    EventLogPetriNetDotWriter dw;
    dw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-synthesis2_1.gv", 
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов  Fig. 2 in [Cortadella, 98] (p. 862)
// добавляем искуственное конечное состояние в автомат и устанавливаем
// опцию — генерировать WF-net
TEST(PnRegSynthesizer1, synthesis2_2)
{
    // ts1: /0.1/docs/imgs/ts1.pdf 
    // здесь будет проблема с начальной разметкой и принимающим состоянием. вот так, да!
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef PNSynth::Label Label;
    typedef std::string String;

    Label wfAccepting;                   // пометка финальной транзиции
    
    //----> Null Labels
    Label lblNull;                      // тестовый объект
    EXPECT_TRUE(wfAccepting.getType() == Label::AType::tBlank);

    // проверим, что нули сравниваться умеют
    EXPECT_FALSE(wfAccepting < lblNull);
    EXPECT_TRUE(wfAccepting == lblNull);

    SplitLabel splLblNull(lblNull, 0);

    PNSynth::Lbl2StateSetMap m;
    PNSynth::StateSet& er = m[splLblNull];

    Label lbl_a(String("a"));
    SplitLabel splLblb_a(lbl_a, 0);
    bool cond1 = splLblNull < splLblb_a;
    cond1 = splLblb_a < splLblNull;             // эксцепция!

    //<---- Null Labels


    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State send = ts.getOrAddState(pool[{ "send" }]);
    ts.setAcceptingState(send, true);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, String("c"));
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, String("c"));
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("a"));
    TS::Transition t7 = ts.getOrAddTrans(s6, send, wfAccepting);

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(7, ts.getTransitionsNum());

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, PNSynth::Options::F_MAKE_WFNET);    
    synth.setWFTransLabel(wfAccepting);
    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter dw;
    dw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-synthesis2_2.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов  Fig. 2 in [Cortadella, 98] (p. 862)
// добавляем искуственное конечное состояние в автомат и устанавливаем
// опцию — генерировать WF-net + цикл
TEST(PnRegSynthesizer1, synthesisLoop2_3)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef PNSynth::Label Label;
    typedef std::string String;

    Label wfAccepting;                   // пометка финальной транзиции

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s1 = ts.getInitState();
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    TS::State s4 = ts.getOrAddState(pool[{ "s4" }]);
    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);
    TS::State send = ts.getOrAddState(pool[{ "send" }]);
    ts.setAcceptingState(send, true);
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t3 = ts.getOrAddTrans(s2, s4, String("c"));
    TS::Transition t4 = ts.getOrAddTrans(s3, s5, String("c"));
    TS::Transition t5 = ts.getOrAddTrans(s4, s6, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("a"));
    TS::Transition t7 = ts.getOrAddTrans(s6, send, wfAccepting);

    // петля, помеченная отдельной буквой
    TS::Transition t8 = ts.getOrAddTrans(s6, s6, String("d"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(8, ts.getTransitionsNum());

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-syntWithLoop2_3-ts.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, PNSynth::Options::F_MAKE_WFNET | PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.setWFTransLabel(wfAccepting);
    
    // поиграемся с настройками обработки дуг
    EXPECT_EQ(PNSynth::Options::slIgnore, synth.options().getSelfLoopPolicy());
    synth.options().setSelfLoopPolicy(PNSynth::Options::slIgnore);
    EXPECT_EQ(PNSynth::Options::slIgnore, synth.options().getSelfLoopPolicy());
    synth.options().setSelfLoopPolicy(PNSynth::Options::slBreak);
    EXPECT_EQ(PNSynth::Options::slBreak, synth.options().getSelfLoopPolicy());
    
    // остаемся на опции slBreak, т.к. TS содержит петлю, должна полететь эксцепция
    EXPECT_THROW(synth.synthesize(), xi::ldopa::LdopaException);

    // теперь включим опцию по воссозданию петель
    synth.options().setSelfLoopPolicy(PNSynth::Options::slReestablish);
    EXPECT_EQ(PNSynth::Options::slReestablish, synth.options().getSelfLoopPolicy());
    // synth.options().setSelfLoopPolicy(PNSynth::Options::slIgnore);
    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-syntWithLoop2_3-pn.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------
#pragma region Self-loops Problem


// генерация синтезированной сети с redundant регионами
// для системы переходов [Shershakov-techreport-20180823]
// добавляем искуственное конечное состояние в автомат 
// цикл, который НЕ раскрывается на этапе TS
TEST(PnRegSynthesizer1, synthesisLoop2_4_2)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef PNSynth::Label Label;
    typedef std::string String;

    typedef TsSLoopsConverter<TS> TsSLoopsEliminator;   // удалятель петель

    Label wfAccepting;                   // пометка финальной транзиции

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s0 = ts.getInitState();
    TS::State s1 = ts.getOrAddState(pool[{ "s1" }]);
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);

    TS::Transition t1 = ts.getOrAddTrans(s0, s1, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s0, s2, String("b"));

    TS::Transition t3 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s3, String("a"));

    // петля, помеченная отдельной буквой
    TS::Transition t4_ = ts.getOrAddTrans(s2, s2, String("x"));

    EXPECT_EQ(4, ts.getStatesNum());
    EXPECT_EQ(5, ts.getTransitionsNum());

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_2-ts.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, /*PNSynth::Options::F_MAKE_WFNET |*/ PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.setWFTransLabel(wfAccepting);

    // поиграемся с настройками обработки дуг  
    synth.options().setSelfLoopPolicy(PNSynth::Options::slIgnore);
    EXPECT_EQ(PNSynth::Options::slIgnore, synth.options().getSelfLoopPolicy());
    synth.options().setSelfLoopPolicy(PNSynth::Options::slBreak);
    EXPECT_EQ(PNSynth::Options::slBreak, synth.options().getSelfLoopPolicy());
    synth.options().setSelfLoopPolicy(PNSynth::Options::slReestablish);
    EXPECT_EQ(PNSynth::Options::slReestablish, synth.options().getSelfLoopPolicy());
    synth.options().setSelfLoopPolicy(PNSynth::Options::slProcess);
    EXPECT_EQ(PNSynth::Options::slProcess, synth.options().getSelfLoopPolicy());

    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_2-pn.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов [Shershakov-techreport-20180823]
// добавляем искуственное конечное состояние в автомат 
// цикл, который НЕ раскрывается на этапе TS + WF
TEST(PnRegSynthesizer1, synthesisLoop2_4_5)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef PNSynth::Label Label;
    typedef std::string String;

    typedef TsSLoopsConverter<TS> TsSLoopsEliminator;   // удалятель петель

    Label wfAccepting;                   // пометка финальной транзиции

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s0 = ts.getInitState();
    TS::State s1 = ts.getOrAddState(pool[{ "s1" }]);
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);
    //ts.setAcceptingState(s3, true);

    TS::Transition t1 = ts.getOrAddTrans(s0, s1, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s0, s2, String("b"));

    TS::Transition t3 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s3, String("a"));

    // спец. принимающее
    TS::State send = ts.getOrAddState(pool[{ "send" }]);
    ts.setAcceptingState(send, true);
    TS::Transition tend1 = ts.getOrAddTrans(s3, send, wfAccepting);

    // петля, помеченная отдельной буквой
    TS::Transition t4_ = ts.getOrAddTrans(s2, s2, String("x"));

    EXPECT_EQ(4 + 1, ts.getStatesNum());
    EXPECT_EQ(5 + 1, ts.getTransitionsNum());

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_5-ts.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, PNSynth::Options::F_MAKE_WFNET | PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.setWFTransLabel(wfAccepting);

    // поиграемся с настройками обработки дуг  
    synth.options().setSelfLoopPolicy(PNSynth::Options::slProcess);
    EXPECT_EQ(PNSynth::Options::slProcess, synth.options().getSelfLoopPolicy());

    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_5-pn.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов [Shershakov-techreport-20180823]
// добавляем искуственное конечное состояние в автомат 
// цикл, который раскрывается ЕЩЕ на этапе TS
TEST(PnRegSynthesizer1, synthesisLoop2_4_3)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef PNSynth::Label Label;
    typedef std::string String;

    typedef TsSLoopsConverter<TS> TsSLoopsEliminator;   // удалятель петель

    Label wfAccepting;                   // пометка финальной транзиции

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s0 = ts.getInitState();
    TS::State s1 = ts.getOrAddState(pool[{ "s1" }]);
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);

    TS::Transition t1 = ts.getOrAddTrans(s0, s1, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s0, s2, String("b"));


    TS::Transition t3 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s3, String("a"));

    // петля, помеченная отдельной буквой
    TS::Transition t4_ = ts.getOrAddTrans(s2, s2, String("x"));

    EXPECT_EQ(4, ts.getStatesNum());
    EXPECT_EQ(5, ts.getTransitionsNum());

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_3-ts-a.gv", ts);

    TsSLoopsEliminator conv;
    EXPECT_EQ(1, conv.convert(&ts));            // 1 петлю должна убрать
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_3-ts-b.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, /*PNSynth::Options::F_MAKE_WFNET |*/ PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.setWFTransLabel(wfAccepting);

    // поиграемся с настройками обработки дуг  
    synth.options().setSelfLoopPolicy(PNSynth::Options::slProcess);
    EXPECT_EQ(PNSynth::Options::slProcess, synth.options().getSelfLoopPolicy());
    synth.synthesize();
    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_3-pn.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

// генерация синтезированной сети с redundant регионами
// для системы переходов [Shershakov-techreport-20180823]
// добавляем искуственное конечное состояние в автомат 
// цикл, который искусствено добавляется в PN после синтеза
TEST(PnRegSynthesizer1, synthesisLoop2_4_4)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef PNSynth::Label Label;
    typedef std::string String;

    typedef TsSLoopsConverter<TS> TsSLoopsEliminator;   // удалятель петель

    Label wfAccepting;                   // пометка финальной транзиции

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);
    TS::State s0 = ts.getInitState();
    TS::State s1 = ts.getOrAddState(pool[{ "s1" }]);
    TS::State s2 = ts.getOrAddState(pool[{ "s2" }]);
    TS::State s3 = ts.getOrAddState(pool[{ "s3" }]);

    TS::Transition t1 = ts.getOrAddTrans(s0, s1, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s0, s2, String("b"));


    TS::Transition t3 = ts.getOrAddTrans(s1, s3, String("b"));
    TS::Transition t4 = ts.getOrAddTrans(s2, s3, String("a"));

    // петля, помеченная отдельной буквой
    TS::Transition t4_ = ts.getOrAddTrans(s2, s2, String("x"));

    EXPECT_EQ(4, ts.getStatesNum());
    EXPECT_EQ(5, ts.getTransitionsNum());

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_4-ts-a.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, /*PNSynth::Options::F_MAKE_WFNET |*/ PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.setWFTransLabel(wfAccepting);

    // поиграемся с настройками обработки дуг  
    synth.options().setSelfLoopPolicy(PNSynth::Options::slReestablish);
    EXPECT_EQ(PNSynth::Options::slReestablish, synth.options().getSelfLoopPolicy());
    synth.synthesize();
    PN* pn = synth.getPn();

    // записыватель
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/sloops/PnRegSynthesizer1-syntWithLoop2_4_4-pn.gv",
        *pn, synth.getInitMarking());
}

//-----------------------------------------------------------------------------

#pragma endregion Self-loops Problem


// генерация синтезированной сети с redundant регионами
// для системы переходов  Fig. 11 in [Cortadella, 98] (p. 870)
// добавляем искуственное конечное состояние
TEST(PnRegSynthesizer1, synthesis3)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;

    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
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
    
    TS::Transition t1 = ts.getOrAddTrans(s1, s2, String("a"));
    TS::Transition t2 = ts.getOrAddTrans(s2, s3, String("c"));
    TS::Transition t3 = ts.getOrAddTrans(s3, s4, String("d"));
    TS::Transition t4 = ts.getOrAddTrans(s4, s1, String("e"));
    TS::Transition t5 = ts.getOrAddTrans(s1, s5, String("b"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, String("d"));
    TS::Transition t7 = ts.getOrAddTrans(s6, s7, String("c"));
    TS::Transition t8 = ts.getOrAddTrans(s7, s1, String("f"));

    EXPECT_EQ(7, ts.getStatesNum());
    EXPECT_EQ(8, ts.getTransitionsNum());

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-synthesis3-ts.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель PN
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/PnRegSynthesizer1-synthesis3-pn.gv",
        *pn, synth.getInitMarking());
}

// Zubkova Case 1 / 03.04.2019
TEST(PnRegSynthesizer1, synthZubkova1)
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;

    typedef EvLogTSWithFreqs TS;
    typedef EventLogPetriNet<> PN;
    typedef PN::Attribute Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel SplitLabel;
    typedef std::string String;

    // исходная система переходов
    AttrListStateIDsPool pool;
    TS ts(&pool);

    TS::State s5 = ts.getOrAddState(pool[{ "s5" }]);
    TS::State s6 = ts.getOrAddState(pool[{ "s6" }]);

    TS::State s4 = ts.getInitState();

    TS::State s7 = ts.getOrAddState(pool[{ "s7" }]);
    TS::State s8 = ts.getOrAddState(pool[{ "s8" }]);
    TS::State s9 = ts.getOrAddState(pool[{ "s9" }]);
    TS::State s10 = ts.getOrAddState(pool[{ "s10" }]);
    TS::Transition t5 = ts.getOrAddTrans(s4, s5, std::string("book_flight"));
    TS::Transition t6 = ts.getOrAddTrans(s5, s6, std::string("send_email"));
    TS::Transition t7 = ts.getOrAddTrans(s6, s7, std::string("choose_payment_type"));
    TS::Transition t9 = ts.getOrAddTrans(s7, s8, std::string("pay_by_web_money"));
    TS::Transition t10 = ts.getOrAddTrans(s8, s10, std::string("complete_booking"));
    TS::Transition t12 = ts.getOrAddTrans(s7, s9, std::string("cancel"));
    TS::Transition t13 = ts.getOrAddTrans(s9, s10, std::string("send_email"));

    ts.setAcceptingState(s10, true);

    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(PN_TEST_MODELS_BASE_DIR "pn/regs/cases/Zubkova1-ts.gv", ts);

    //=====[ синтезатор ]=====    
    PNSynth synth(&ts, PNSynth::Options::F_OUT_PLACE_TITLES);
    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель PN
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(PN_TEST_MODELS_BASE_DIR "pn/regs/cases/Zubkova1-pn.gv",
        *pn, synth.getInitMarking());
}
