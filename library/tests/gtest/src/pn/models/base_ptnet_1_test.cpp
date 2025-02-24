////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests (series 1) for Base P/T net classes.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      15.04.2018
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

#include <type_traits>  // https://stackoverflow.com/questions/16924168/compile-time-function-for-checking-type-equality
#include <string>

#include "xi/ldopa/utils.h"
#include "xi/ldopa/pn/models/base_ptnet.h"

/** \brief Test class for testing Base P/T net class. */
class BasePTNet_1_Test : public ::testing::Test {
public:
    //-----<Types>-----
public:
protected:
    virtual void SetUp()
    {
        //SetUpGraph();
    } 

    virtual void TearDown()
    {
        //delete _gr1;
    }

}; // class LabeledTS_1_Test

//===================================================================================

using namespace xi::ldopa::pn;

TEST_F(BasePTNet_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}

//-----------------------------------------------------------------------------

TEST_F(BasePTNet_1_Test, marking1)
{
    typedef BasePetriNet<> PN;
    PN pn;
    PN::Position p0 = pn.addPosition();
    PN::Position p1 = pn.addPosition();

    // разметка
    PN::Marking m1;

    xi::ldopa::UInt m1_w = m1(p0);          // r-value
    m1[p0] = 2;                             // l-value
    xi::ldopa::UInt m1_w2 = m1(p0);         // r-value
    //m1(p0) = 3;                           // illegal
    xi::ldopa::UInt m1_w3 = m1[p0];         // also possible

    // const overload
    const PN::Marking& m1c = m1;

    xi::ldopa::UInt m1c_w = m1c(p0);        // r-value
    //m1c[p0] = 2;                          // illegal now
    xi::ldopa::UInt m1c_w2 = m1c(p0);       // r-value
    xi::ldopa::UInt m1c_w3 = m1c[p0];       // also possible due to presence of const overloading
}

//-----------------------------------------------------------------------------

// входные/выходные дуги для позиций/транзиций
// структура сети на рисунке /docs/imgs/tfig-0001.gif
//TEST_F(BasePTNet_1_Test, checkingActiveTransitions1)
TEST_F(BasePTNet_1_Test, enabledTransition1)
{
    typedef BasePetriNet<> PN;
    PN pn;
    PN::Position p0 = pn.addPosition();
    PN::Position p1 = pn.addPosition();
    PN::Position p2 = pn.addPosition();
    PN::Position p3 = pn.addPosition();
    PN::Position p4 = pn.addPosition();
    PN::Transition t1 = pn.addTransition();
    PN::Transition t2 = pn.addTransition();

    // дуги
    PN::PTArc a1 = pn.addArc(p0, t1);
    PN::PTArc a2 = pn.addArc(p0, t2);
    PN::PTArc a3 = pn.addArcW(p1, t2, 3);               // пометим весом 3 для начала!
    PN::TPArc a4 = pn.addArc(t1, p2);
    PN::TPArc a5 = pn.addArc(t1, p3);
    PN::TPArc a6 = pn.addArc(t2, p4);

    EXPECT_EQ(7, pn.getVerticesNum());
    EXPECT_EQ(5, pn.getPositionsNum());
    EXPECT_EQ(2, pn.getTransitionsNum());
    EXPECT_EQ(6, pn.getArcsNum());

    // веса дуг (единички)
    EXPECT_EQ(1, pn.getArcWeight(a1));
    EXPECT_EQ(1, pn.getArcWeight(a2));
    EXPECT_EQ(3, pn.getArcWeight(a3));

    // переиначим вес дуги 3 в 2
    pn.setArcWeight(a3, 2);
    EXPECT_EQ(2, pn.getArcWeight(a3));
    
    pn.setArcWeight(a3, 1);         // обратно вес в 1

    // создадим разметку
    PN::Marking m1;
    m1[p0] = 1;                     // одна фишка в позицию p0
    //m1.marking[p0] = 1;             // одна фишка в позицию p0

    // [t1] д.б. разрешен
    EXPECT_TRUE(pn.isEnabled(t1, m1));
    
    // [t2] не д.б. разрешен
    EXPECT_FALSE(pn.isEnabled(t2, m1));

    // добавляем фишку в (p1), теперь и [t2] должен стать разрешенным
    m1[p1] = 1;
    EXPECT_TRUE(pn.isEnabled(t2, m1));

    // изменим вес дуги a3 с дефолтной единички до двойки, опять [t2] неразрешенный
    pn.setArcWeight(a3, 2);         // и опять - в 2
    EXPECT_FALSE(pn.isEnabled(t2, m1));

    // добавим еще фишечку в p1
    m1[p1] = 2;
    EXPECT_TRUE(pn.isEnabled(t2, m1));


    // добавим к [t2] входную ингибиторную дугу и еще одну позицию (p11) БЕЗ фишек
    PN::Position p11 = pn.addPosition();
    PN::PTArc a11 = pn.addArc(p11, t2, PN::ArcType::atInhib);
    
    // т.к. в a11 нет фишек, должна остаться активной
    EXPECT_TRUE(pn.isEnabled(t2, m1));          

    // теперь добавим в (p11) фишечку, и ингибиторная дуга должна закрыть переход
    m1[p11] = 1;
    EXPECT_FALSE(pn.isEnabled(t2, m1));  
}

//-----------------------------------------------------------------------------
