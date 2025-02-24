////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests (series 1) for EventLog Petrinet model types.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      15.08.2018
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
#include "xi/ldopa/pn/models/evlog_ptnets.h"                    // модель PN
#include "xi/ldopa/pn/algos/grviz/evlog_ptnets_dotwriter.h"     // записыватель в DOT

#include "../pn_test_settings.h"

/** \brief Test class for testing EventLogPetriNet class. */
class EventLogPetriNet_1_Test : public ::testing::Test {
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

TEST_F(EventLogPetriNet_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}

//-----------------------------------------------------------------------------

// входные/выходные дуги для позиций/транзиций
// структура сети на рисунке /docs/imgs/tfig-0001.gif
TEST_F(EventLogPetriNet_1_Test, pn1)
{
    typedef EventLogPetriNet<> PN;
    typedef PN::Attribute Attribute;
    typedef std::string String;

    PN pn;
    PN::Position p0 = pn.addPosition();
    PN::Position p1 = pn.addPosition();
    PN::Position p2 = pn.addPosition();
    PN::Position p3 = pn.addPosition();
    PN::Position p4 = pn.addPosition();
    PN::Transition t1 = pn.addTransition(Attribute(String("t1")));
    PN::Transition t2 = pn.addTransition(Attribute(String("t2")));

    // дуги
    PN::PTArc a1 = pn.addArc(p0, t1);
    PN::PTArc a2 = pn.addArc(p0, t2);
    PN::PTArc a3 = pn.addArcW(p1, t2);
    PN::TPArc a4 = pn.addArc(t1, p2);
    PN::TPArc a5 = pn.addArc(t1, p3);
    PN::TPArc a6 = pn.addArc(t2, p4);

    EXPECT_EQ(7, pn.getVerticesNum());
    EXPECT_EQ(5, pn.getPositionsNum());
    EXPECT_EQ(2, pn.getTransitionsNum());
    EXPECT_EQ(6, pn.getArcsNum());

    // записыватель
    EventLogPetriNetDotWriter dw;
    dw.write(PN_TEST_MODELS_BASE_DIR "pn/EventLogPetriNet_1_Test-pn1.gv", pn);
}

//-----------------------------------------------------------------------------

// входные/выходные дуги для позиций/транзиций + разметка
// структура сети на рисунке /docs/imgs/tfig-0001.gif
TEST_F(EventLogPetriNet_1_Test, pnWithMarking1)
{
    typedef EventLogPetriNet<> PN;
    typedef PN::Marking Marking;
    typedef PN::Attribute Attribute;
    typedef std::string String;

    PN pn;
    PN::Position p0 = pn.addPosition();
    PN::Position p1 = pn.addPosition();
    PN::Position p2 = pn.addPosition();
    PN::Position p3 = pn.addPosition();
    PN::Position p4 = pn.addPosition();
    PN::Transition t1 = pn.addTransition(Attribute(String("t1")));
    PN::Transition t2 = pn.addTransition(Attribute(String("t2")));

    // дуги
    PN::PTArc a1 = pn.addArc(p0, t1);
    PN::PTArc a2 = pn.addArc(p0, t2);
    PN::PTArc a3 = pn.addArcW(p1, t2);
    PN::TPArc a4 = pn.addArc(t1, p2);
    PN::TPArc a5 = pn.addArc(t1, p3);
    PN::TPArc a6 = pn.addArc(t2, p4);

    EXPECT_EQ(7, pn.getVerticesNum());
    EXPECT_EQ(5, pn.getPositionsNum());
    EXPECT_EQ(2, pn.getTransitionsNum());
    EXPECT_EQ(6, pn.getArcsNum());

    // разметка
    Marking m1;
    m1[p0] = 1;
    m1[p1] = 2;

    // записыватель
    EventLogPetriNetDotWriter dw;
    dw.writePn(PN_TEST_MODELS_BASE_DIR "pn/EventLogPetriNet_1_Test-pnWithMarking1.gv", pn, m1);
}
