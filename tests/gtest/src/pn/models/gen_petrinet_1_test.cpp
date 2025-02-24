////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests (series 1) for Generic Petri Net template.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      4.04.2018
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

#include "xi/ldopa/pn/models/gen_petrinet.h"

/** \brief Test class for testing Generic Petri net template. */
class GenPetriNet_1_Test : public ::testing::Test {
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

TEST_F(GenPetriNet_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}

//-----------------------------------------------------------------------------

// выравнивание структур
TEST_F(GenPetriNet_1_Test, createEmptyPn1)
{
    GenPetriNet<> pn;
}

//-----------------------------------------------------------------------------

// создаем много позиций/транзиций и итерируем только позиции или транзиции
TEST_F(GenPetriNet_1_Test, getPositionsTransitions1)
{
    typedef GenPetriNet<> PN;

    PN pn;
    // изначально число вершин, позиций и транзиций по нулям д.б.
    EXPECT_EQ(0, pn.getVerticesNum());
    EXPECT_EQ(0, pn.getPositionsNum());
    EXPECT_EQ(0, pn.getTransitionsNum());

    // создаем 3 позиции и 4 транзиции — вперемешку
    PN::Position p1 = pn.addPosition();
    PN::Transition t1 = pn.addTransition();
    PN::Position p2 = pn.addPosition();
    PN::Position p3 = pn.addPosition();    
    PN::Transition t2 = pn.addTransition();
    PN::Transition t3 = pn.addTransition();
    PN::Transition t4 = pn.addTransition();
    EXPECT_EQ(7, pn.getVerticesNum());
    EXPECT_EQ(3, pn.getPositionsNum());
    EXPECT_EQ(4, pn.getTransitionsNum());
    
    // итерируем позиции
    int count = 0;
    PN::PosIter pCur, pEnd;
    for (boost::tie(pCur, pEnd) = pn.getPositions(); pCur != pEnd; ++pCur)
    {
        const PN::Position& p = *pCur;
        ++count;
    }
    EXPECT_EQ(3, count);

    // итерируем транзиции
    count = 0;
    PN::TransIter tCur, tEnd;
    for (boost::tie(tCur, tEnd) = pn.getTransitions(); tCur != tEnd; ++tCur)
    {
        const PN::Transition& t = *tCur;
        ++count;
    }
    EXPECT_EQ(4, count);
}

//-----------------------------------------------------------------------------

// создание позиций-транзиций-дуг
TEST_F(GenPetriNet_1_Test, addPosTransArcs1)
{
    typedef GenPetriNet<> PN;
    
    PN pn;

    // изначально число вершин, позиций и транзиций по нулям д.б.
    EXPECT_EQ(0, pn.getVerticesNum());
    EXPECT_EQ(0, pn.getPositionsNum());
    EXPECT_EQ(0, pn.getTransitionsNum());

    //PN::Position p = PN::Position::addPositionTo(pn);
    PN::Position p = pn.addPosition();
    EXPECT_FALSE(p.isTrans(pn));                        // не переход (позиция!)
    EXPECT_EQ(1, pn.getVerticesNum());
    EXPECT_EQ(1, pn.getPositionsNum());
    EXPECT_EQ(0, pn.getTransitionsNum());


    //PN::Transition t = PN::Transition::addTransitionTo(pn);
    PN::Transition t = pn.addTransition();
    EXPECT_TRUE(t.isTrans(pn));                         // переход (не позиция!)
    EXPECT_EQ(2, pn.getVerticesNum());
    EXPECT_EQ(1, pn.getPositionsNum());
    EXPECT_EQ(1, pn.getTransitionsNum());

    // прокинем пару дуг: туда и обратно
    EXPECT_EQ(0, pn.getArcsNum());
    PN::PTArc a1 = pn.addArc(p, t);
    EXPECT_EQ(1, pn.getArcsNum());
    //PN::TPArc a2 = pn.addArc(t, p, PN::ArcType::atInhib);   // это нонсенс! Ингибиторная дуга м.б. только PT!
    PN::TPArc a2 = pn.addArc(t, p);
    EXPECT_EQ(2, pn.getArcsNum());

    // для ингибиторной добавим еще вершину и дугу К позиции
    PN::Position p2 = pn.addPosition();
    PN::PTArc a3 = pn.addArc(p2, t, PN::ArcType::atInhib);

    // проверим типы дуг
    EXPECT_EQ(PN::ArcType::atReg, pn.getArcType(a1));
    EXPECT_EQ(PN::ArcType::atReg, pn.getArcType(a2));
    EXPECT_EQ(PN::ArcType::atInhib, pn.getArcType(a3));

    // поменяем и проверим еще раз
    pn.setArcType(a1, PN::ArcType::atInhib);                    // можно
    //pn.setArcType(a2, PN::ArcType::atInhib);                    // нельзя
    pn.setArcType(a3, PN::ArcType::atReg);                    // нельзя
    
    EXPECT_EQ(PN::ArcType::atInhib, pn.getArcType(a1));
    EXPECT_EQ(PN::ArcType::atReg, pn.getArcType(a3));

    // проверяем исходники-таргетники для дуги a3
    PN::Position p2_ = pn.getSrcPos(a3);
    EXPECT_EQ(p2, p2_);
    PN::Transition t_ = pn.getTargTrans(a3);
    EXPECT_EQ(t, t_);

    // то же самое, только неприведенные вертексы теперь
    PN::PnVertex p2__ = pn.getSrcVert(a3);
    EXPECT_EQ(p2, p2__);
    PN::PnVertex t__ = pn.getTargVert(a3);
    EXPECT_EQ(t, t__);
}


//-----------------------------------------------------------------------------

// создает ПН структурой /docs/imgs/tfig-0001.gif без нагрузки на вершинах-дугах
void makePN1(GenPetriNet<>& pn)
{
    //PN::Position p0 = pn.addPosition();
    //PN::Position p1 = pn.addPosition();
    //PN::Position p2 = pn.addPosition();
    //PN::Position p3 = pn.addPosition();
    //PN::Position p4 = pn.addPosition();
    //PN::Transition t1 = pn.addTransition();
    //PN::Transition t2 = pn.addTransition();

    //// дуги
    //PN::Arc a1 = pn.addArc(p0, t1);
    //PN::Arc a2 = pn.addArc(p0, t2);
    //PN::Arc a3 = pn.addArc(p1, t2);
    //PN::Arc a4 = pn.addArc(t1, p2);
    //PN::Arc a5 = pn.addArc(t1, p3);
    //PN::Arc a6 = pn.addArc(t2, p4);

    //EXPECT_EQ(7, pn.getVerticesNum());
    //EXPECT_EQ(5, pn.getPositionsNum());
    //EXPECT_EQ(2, pn.getTransitionsNum());
    //EXPECT_EQ(6, pn.getArcsNum());
}


// входные/выходные дуги для позиций/транзиций
// структура сети на рисунке /docs/imgs/tfig-0001.gif
TEST_F(GenPetriNet_1_Test, inOutArcs1)
{
    typedef GenPetriNet<> PN;

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
    PN::PTArc a3 = pn.addArc(p1, t2);
    PN::TPArc a4 = pn.addArc(t1, p2);
    PN::TPArc a5 = pn.addArc(t1, p3);
    PN::TPArc a6 = pn.addArc(t2, p4);

    EXPECT_EQ(7, pn.getVerticesNum());
    EXPECT_EQ(5, pn.getPositionsNum());
    EXPECT_EQ(2, pn.getTransitionsNum());
    EXPECT_EQ(6, pn.getArcsNum());
    //makePN1(pn);

    //-----------------<типизированные дуги>-------------
    
    //===   (p0)->   ===
    // у p0 две выходные дуги, ведущие к транзициям
    int p0_out_num = 0;
    PN::PoTArcIterPair p0_out2 = pn.getOutArcs(p0);
    for (; p0_out2.first != p0_out2.second; ++p0_out2.first)
    {
        PN::PTArc ca = *p0_out2.first;
        PN::BaseGraph::Edge e = ca.edge;
        
        //PN::Arc a = ca.arc;

        PN::Position srcPos = pn.getSrcPos(ca);
        EXPECT_TRUE(srcPos == p0);

        ++p0_out_num;
    }
    EXPECT_EQ(2, p0_out_num);
    p0_out2 = pn.getOutArcs(p0);                               // итераторы оживить надо!
    p0_out_num = std::distance(p0_out2.first, p0_out2.second);
    EXPECT_EQ(2, p0_out_num);


    // теперь сами выходные транзиции (t1, t2) для позиции p0
    int p0_outtrans_num = 0;
    PN::OutTransIterPair p0_out_trans = pn.getPostset(p0);
    for (; p0_out_trans.first != p0_out_trans.second; ++p0_out_trans.first)
    {
        PN::Transition trans = (*p0_out_trans.first).first;     // вершина и...
        PN::PTArc arc = (*p0_out_trans.first).second;           // ...инцидентная дуга

        ++p0_outtrans_num;
    }
    EXPECT_EQ(2, p0_outtrans_num);


    // у p0 нет входных
    PN::TPiArcIterPair p0_in = pn.getInArcs(p0);
    int p0_in_num = std::distance(p0_in.first, p0_in.second);
    EXPECT_EQ(0, p0_in_num);


    // у p3 выходных вообще нет
    PN::PoTArcIterPair p3_out = pn.getOutArcs(p3);
    int p3_out_num = std::distance(p3_out.first, p3_out.second);
    EXPECT_EQ(0, p3_out_num);

    //===   ->[t2]   ===
    // у t2 две входные позиции, возьмем ведущие от них дуги
    int t2_in_num = 0;
    PN::PTiArcIterPair t2_in2 = pn.getInArcs(t2);
    for (; t2_in2.first != t2_in2.second; ++t2_in2.first)
    {
        PN::PTArc ca = *t2_in2.first;
        PN::BaseGraph::Edge e = ca.edge;
        //PN::Arc a = ca.arc;

        PN::Transition targTrans = pn.getTargTrans(ca);
        EXPECT_TRUE(targTrans == t2);

        ++t2_in_num;
    }
    EXPECT_EQ(2, t2_in_num);

    // альтернативный подход
    t2_in2 = pn.getInArcs(t2);
    t2_in_num = std::distance(t2_in2.first, t2_in2.second);
    EXPECT_EQ(2, t2_in_num);


    // теперь сами входные позиции для транзиции t2
    int t2_inpos_num = 0;
    //PN::InPosIter1Pair t2_in_pos = pn.getPreset(t2);
    PN::InPosIterPair t2_in_pos = pn.getPreset(t2);
    for (; t2_in_pos.first != t2_in_pos.second; ++t2_in_pos.first)
    {
        PN::Position pos = (*t2_in_pos.first).first;        // вершина и...
        PN::PTArc arc = (*t2_in_pos.first).second;          // ...инцидентная дуга

        ++t2_inpos_num;
    }
    EXPECT_EQ(2, t2_inpos_num);

    

    //===   [t2]->   ===
    // у t2 — одна выходная дуга
    PN::ToPArcIterPair t2_out2 = pn.getOutArcs(t2);
    int t2_out_num = std::distance(t2_out2.first, t2_out2.second);
    EXPECT_EQ(1, t2_out_num);


    //===   [t1]->   ===
    // у t1 две выходные дуги
    PN::ToPArcIterPair t1_out2 = pn.getOutArcs(t1);
    for (; t1_out2.first != t1_out2.second; ++t1_out2.first)
    {
        PN::TPArc ca = *t1_out2.first;
        PN::BaseGraph::Edge e = ca.edge;
        //PN::Arc a = ca.arc;

        PN::Transition srcTrans = pn.getSrcTrans(ca);
        EXPECT_TRUE(srcTrans == t1);

        int i = 0;
    }

    // теперь сами выходные позиции для транзиции t1
    int t1_outpos_num = 0;           
    PN::OutPosIterPair t1_out_pos = pn.getPostset(t1);
    for (; t1_out_pos.first != t1_out_pos.second; ++t1_out_pos.first)
    {
        PN::Position pos = (*t1_out_pos.first).first;       // вершина и...
        PN::TPArc arc = (*t1_out_pos.first).second;         // ...инцидентная дуга

        ++t1_outpos_num;
    }
    EXPECT_EQ(2, t1_outpos_num);




    // у p2 одна входная дуга от входной транзиции
    PN::TPiArcIterPair p2_in2 = pn.getInArcs(p2);
    for (; p2_in2.first != p2_in2.second; ++p2_in2.first)
    {
        PN::TPArc ca = *p2_in2.first;
        PN::BaseGraph::Edge e = ca.edge;
        //PN::Arc a = ca.arc;

        PN::Position targPos = pn.getTargPos(ca);
        EXPECT_TRUE(targPos == p2);

        int i = 0;
    }


    //===   ->(p2)   ===
    // теперь сами входные транзиции (1 шт) для позиции p2
    int p2_intrans_num = 0;
    PN::InTransIterPair p2_in_trans = pn.getPreset(p2);
    for (; p2_in_trans.first != p2_in_trans.second; ++p2_in_trans.first)
    {
        PN::Transition trans = (*p2_in_trans.first).first;  // вершина и...
        PN::TPArc arc = (*p2_in_trans.first).second;        // ...инцидентная дуга

        ++p2_intrans_num;
    }
    EXPECT_EQ(1, p2_intrans_num);



    // у p3 одна входная
    PN::TPiArcIterPair p3_in = pn.getInArcs(p3);
    int p3_in_num = std::distance(p3_in.first, p3_in.second);
    EXPECT_EQ(1, p3_in_num);



    //-----<Общее число дуг>-----
    // добавочка от 21.07.2018: итерируем дуги
    PN::BaseArcIterPair pnArcs = pn.getArcs();
    int pnArcs_num = std::distance(pnArcs.first, pnArcs.second);
    EXPECT_EQ(6, pnArcs_num);


    //-----<Дуги между индивидуальными вершинами>-----
    // добавочка от 17.08.2018
    PN::PTArcRes ar1 = pn.getArc(p0, t1);
    EXPECT_TRUE(ar1.second);                    // существует
    EXPECT_TRUE(ar1.first == a1);               // и это a1
    EXPECT_FALSE(ar1.first == a2);              // и это не a2

    ar1 = pn.getArc(p0, t1, PN::ArcType::atInhib);
    EXPECT_FALSE(ar1.second);                   // ингибиторной не существует

    // от транзиции
    PN::TPArcRes ar2 = pn.getArc(t1, p0);       // нет такой
    EXPECT_FALSE(ar2.second);

    ar2 = pn.getArc(t1, p2);
    EXPECT_TRUE(ar2.second);                    // существует
    EXPECT_TRUE(ar2.first == a4);               // первая по порядку дуга
}

