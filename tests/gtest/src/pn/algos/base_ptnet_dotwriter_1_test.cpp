////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for Base Petri net DOT (graphviz) writer.
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
#include "xi/ldopa/pn/models/base_ptnet.h"
#include "xi/ldopa/pn/algos/grviz/base_ptnet_dotwriter.h"

#include "../pn_test_settings.h"

//==============================================================================
// class BasePetriNetDotWriter1
//==============================================================================

TEST(BasePetriNetDotWriter1, mapLabeledNet1)
{
    using namespace xi::ldopa::pn;
    typedef MapLabeledPetriNet<> PN;

    PN pn;
    PN::Position p0 = pn.addPosition("p0");
    PN::Position p1 = pn.addPosition("p1");
    PN::Position p2 = pn.addPosition("p2");
    PN::Position p3 = pn.addPosition("p3");
    //PN::Position p4 = pn.addPosition("p4");
    PN::Position p4 = pn.addPosition();
    PN::Transition t1 = pn.addTransition("t1");
    PN::Transition t2 = pn.addTransition("t2");

    // дуги
    PN::PTArc a1 = pn.addArc(p0, t1);
    PN::PTArc a2 = pn.addArc(p0, t2, PN::ArcType::atInhib); // ингибиторная
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

    // записыватель
    //typedef typename EvLogTSWithFreqsDotWriter TsDotWriter;
    MapLabeledPetriNetDotWriter dw;
    dw.write(PN_TEST_MODELS_BASE_DIR "pn/BasePetriNetDotWriter1-mapLabeledNet1.gv", pn);
}


TEST(BasePetriNetDotWriter1, mapLabeledNetWithMarking1)
{
    using namespace xi::ldopa::pn;
    typedef MapLabeledPetriNet<> PN;
    typedef PN::Marking Marking;

    PN pn;
    PN::Position p0 = pn.addPosition("p0");
    PN::Position p1 = pn.addPosition("p1");
    PN::Position p2 = pn.addPosition("p2");
    PN::Position p3 = pn.addPosition("p3");
    //PN::Position p4 = pn.addPosition("p4");
    PN::Position p4 = pn.addPosition();
    PN::Transition t1 = pn.addTransition("t1");
    PN::Transition t2 = pn.addTransition("t2");

    // дуги
    PN::PTArc a1 = pn.addArc(p0, t1);
    PN::PTArc a2 = pn.addArc(p0, t2, PN::ArcType::atInhib); // ингибиторная
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

    // разметка
    Marking m1;
    m1[p0] = 1;
    m1[p4] = 2;

    // записыватель
    //typedef typename EvLogTSWithFreqsDotWriter TsDotWriter;
    MapLabeledPetriNetDotWriter dw;
    dw.writePn(PN_TEST_MODELS_BASE_DIR "pn/BasePetriNetDotWriter1-mapLabeledNetWithMarking1.gv", 
        pn, m1);
}
