////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for Labeled Transision Systems #1.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      14.09.2017
/// \copyright (c) xidv.ru 2014–2017.
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

#include "xi/ldopa/ts/models/labeledts.h"

// Tests SQLiteLog class. Part 1
/** \brief Test class for testing labeled ts #1. */
class LabeledTS_1_Test : public ::testing::Test {
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

using namespace xi::ldopa::ts;
// система переходов, где состояния идентифицируются целыми, а пометки — C-строковые константы
typedef LabeledTS<int, const char*> Lts01;

//-----------------------------------------------------------------------------

TEST_F(LabeledTS_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}

//-----------------------------------------------------------------------------

TEST_F(LabeledTS_1_Test, Lts1AddStates1)
{
    typedef Lts01 Ts;

    Ts ts;
    ASSERT_EQ(0, ts.getStatesNum());            // изначально должно быть по нулям
    ASSERT_EQ(0, ts.getTransitionsNum());

    // добавим пару состояний
    Ts::State s1 = ts.getOrAddState(1);     // идент — 1
    ASSERT_EQ(1, ts.getStatesNum());

    Ts::State s2 = ts.getOrAddState(2);     // идент — 2
    ASSERT_EQ(2, ts.getStatesNum());

    // теперь еще раз пытаемся добавить состояние с ид, которое уже использовалось
    Ts::State s1_2 = ts.getOrAddState(1);   // идент — 1
    ASSERT_EQ(2, ts.getStatesNum());            // число состояний не должно измениться

    ASSERT_EQ(s1, s1_2);                        // состояния должны быть эквивалентны

    // теперь возьмем ранее добавленный
    Ts::StateRes s1_g = ts.getState(1);
    ASSERT_TRUE(s1_g.second);
    ASSERT_EQ(s1, s1_g.first);

    // попробует взять недобавленный
    Ts::StateRes s42_g = ts.getState(42);
    ASSERT_FALSE(s42_g.second);
}


//-----------------------------------------------------------------------------

// тестируем выводимые типы при разных параметрах типов кастомных свойств для состояний
// для специального int-треита
TEST_F(LabeledTS_1_Test, StatePropertyTypes1)
{
    // без явного указания доп. свойства вершины — д.б. только целочисленный ИД
    typedef LabeledTS<int, const char*> Lts1;
    Lts1::StPropBundle sp1;

    ASSERT_TRUE((std::is_same<int, Lts1::StPropBundle>::value));
    ASSERT_TRUE((std::is_same<int, Lts1::StateIDCArg>::value));                 // так будет со спец. трэитами для инта 
    ASSERT_TRUE((std::is_same<int, Lts1::StateIDCRes>::value));             
    
    ASSERT_TRUE((std::is_same<int, Lts1::StPropBundleCArg>::value));
    ASSERT_TRUE((std::is_same<int&, Lts1::StPropBundleRef>::value));
    ASSERT_TRUE((std::is_same<int, Lts1::StPropBundleCRes>::value));
    ASSERT_TRUE((std::is_same<boost::no_property, Lts1::StDataRes>::value));   // нет кастомных свойст у вершин

    // пользуемся знанием о sp1 в compile-time
    sp1 = 42;
    ASSERT_EQ(42, Lts1::extractStateID(sp1));

    // с явным указанием доп. свойства вершины (double), должна появиться структура
    // введем тип для посл. сравнения

    typedef LabeledTS<int, const char*, double> Lts2;
    //int s2 = sizeof(Lts2::StateProperty);
    Lts2::StPropBundle sp2;

    ASSERT_TRUE((std::is_same<StateProperty_traits<int, double>::StPropBundle, 
        Lts2::StPropBundle>::value));
    ASSERT_TRUE((std::is_same<int, Lts2::StateIDCArg>::value));                 // так со спец. трэитами
    ASSERT_TRUE((std::is_same<int, Lts2::StateIDCRes>::value));

    ASSERT_TRUE((std::is_same<const StateProperty_traits<int, double>::StPropBundle &, 
        Lts2::StPropBundleCArg>::value));

    ASSERT_TRUE((std::is_same<StateProperty_traits<int, double>::StPropBundle &,
        Lts2::StPropBundleRef>::value));

    ASSERT_TRUE((std::is_same<const StateProperty_traits<int, double>::StPropBundle &,
        Lts2::StPropBundleCRes>::value));

    ASSERT_TRUE((std::is_same<double&, Lts2::StDataRes>::value));   // кастомное с пл. точкой, по ссылке

    sp2.id = 42;
    sp2.prop = 3.14;
    ASSERT_EQ(42, Lts2::extractStateID(sp2));
}

//-----------------------------------------------------------------------------

// тестируем выводимые типы при разных параметрах типов кастомных свойств для состояний
// для общего string-треита
TEST_F(LabeledTS_1_Test, StatePropertyTypes2)
{
    typedef std::string Str;

    // без явного указания доп. свойства вершины — д.б. только строка
    typedef LabeledTS<Str, int> Lts1;   
    Lts1::StPropBundle sp1;                // свойство вершины — строка

    ASSERT_TRUE((std::is_same<Str, Lts1::StPropBundle>::value));
    ASSERT_TRUE((std::is_same<const Str &, Lts1::StateIDCArg>::value));         
    ASSERT_TRUE((std::is_same<const Str &, Lts1::StateIDCRes>::value));         
    ASSERT_TRUE((std::is_same<const Str &, Lts1::StPropBundleCArg>::value));
    ASSERT_TRUE((std::is_same< Str &, Lts1::StPropBundleRef>::value));
    ASSERT_TRUE((std::is_same<const Str &, Lts1::StPropBundleCRes>::value));

    ASSERT_TRUE((std::is_same<boost::no_property, Lts1::StDataRes>::value));   // нет кастомных свойст у вершин

    // пользуемся знанием о sp1 в compile-time
    sp1 = Str("Xyz");
    ASSERT_EQ(Str("Xyz"), Lts1::extractStateID(sp1));

    // с явным указанием доп. свойства вершины (double), должна появиться структура
    // введем тип для посл. сравнения
    typedef LabeledTS<Str, int, double> Lts2;
    Lts2::StPropBundle sp2;

    ASSERT_TRUE((std::is_same<StateProperty_traits<Str, double>::StPropBundle, 
        Lts2::StPropBundle>::value));
    ASSERT_TRUE((std::is_same<const Str &, Lts2::StateIDCArg>::value));
    ASSERT_TRUE((std::is_same<const Str &, Lts2::StateIDCRes>::value));

    ASSERT_TRUE((std::is_same<const StateProperty_traits<Str, double>::StPropBundleCArg &,
        Lts2::StPropBundleCArg>::value));
    ASSERT_TRUE((std::is_same< StateProperty_traits<Str, double>::StPropBundle &,
        Lts2::StPropBundleRef>::value));
    ASSERT_TRUE((std::is_same<const StateProperty_traits<Str, double>::StPropBundle &,
        Lts2::StPropBundleCRes>::value));
    ASSERT_TRUE((std::is_same<double&, Lts2::StDataRes>::value));   // пл. точка по ссылке

    sp2.id = Str("Abc");
    sp2.prop = 3;
    ASSERT_EQ(Str("Abc"), Lts2::extractStateID(sp2));
}

//-----------------------------------------------------------------------------

// тестируем свойства, привязанные к состоянию
TEST_F(LabeledTS_1_Test, StateProperties1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, const char*> Lts1;
    Lts1 ts1;
    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);      // идент — 1
    ASSERT_EQ(1, ts1.getStatesNum());
    ASSERT_EQ(1, ts1.getStateID(s1));               // ид = 1
    
    Lts1::State s1_2 = ts1.getOrAddState(1);    // идент — 1
    ASSERT_EQ(1, ts1.getStatesNum());               // число состояний не должно измениться

    ASSERT_EQ(s1, s1_2);                            // состояния должны быть эквивалентны

    // обращаемся к свойству id вершины 1
    int s1_id = ts1.getGraph()[s1];
    Lts1::StPropBundle s1_prop = ts1.getGraph()[s1];
    ASSERT_EQ(1, s1_id);

    // теперь по-другому тоже самой
    Lts1::StPropBundle s1_prop2 = ts1.getBundle(s1);    // тут будет копия?
    Lts1::StPropBundleCRes s1_prop3 = ts1.getBundle(s1);    

    // кастомные свойства к вершине
    Lts1::StDataRes s1_data = ts1.getData(s1);      // для "неданных" тут будет не ссылка, а по значению
}

//-----------------------------------------------------------------------------

// тестируем свойства, привязанные к состоянию
TEST_F(LabeledTS_1_Test, StateProperties2)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, const char*, double> Lts2;
    Lts2 ts2;
    // добавим пару состояний
    Lts2::State s1 = ts2.getOrAddState(1);      // идент — 1
    ASSERT_EQ(1, ts2.getStatesNum());
    ASSERT_EQ(1, ts2.getStateID(s1));               // ид = 1

    Lts2::State s1_2 = ts2.getOrAddState(1);    // идент — 1
    ASSERT_EQ(1, ts2.getStatesNum());               // число состояний не должно измениться

    ASSERT_EQ(s1, s1_2);                            // состояния должны быть эквивалентны

    // обращаемся к свойству id вершины 1
    Lts2::StPropBundle s1_prop = ts2.getGraph()[s1];

    // еще разок на типы посмотрим
    ASSERT_TRUE((std::is_same<const StateProperty_traits<int, double>::StPropBundle &,
        Lts2::StPropBundleCRes>::value));

    // теперь по-другому тоже самой
    Lts2::StPropBundle s1_prop2 = ts2.getBundle(s1);        // тут будет копия?
    Lts2::StPropBundleCRes s1_prop3 = ts2.getBundle(s1);    // это честная конст. ссылка!!! см. ниже

    s1_prop2.id = 2;        // это копия — на объект не влияет
    ASSERT_EQ(1, ts2.getStateID(s1));               // ид = 1
    
    //  s1_prop3.id = 2;    // правильно не должно работать (попр. испортить в отладчике) — 
    ASSERT_EQ(1, ts2.getStateID(s1));               // ид = 1


    // кастомные свойства к вершине — double
    Lts2::StDataRes s1_data = ts2.getData(s1);      // привязываем ссылку
    s1_data = 3.14159258;
    ASSERT_EQ(3.14159258, s1_prop3.prop);           // здесь может быть "то еще" сравнение с учетом проблем точности

    // добавим анонимные состояния
    Lts2::State sa1 = ts2.addAnonState();
    ASSERT_EQ(2, ts2.getStatesNum());
    ASSERT_TRUE(ts2.getStatesNum() == ts2.getRegStatesNum() + ts2.getAnonStatesNum());
    Lts2::StDataRes sa1_data = ts2.getData(sa1);      // привязываем ссылку
    Lts2::StPropBundleCRes sa1_b = ts2.getBundle(sa1);// ID анонимного состояния м.б. любым!   
}



//-----------------------------------------------------------------------------

// тестируем выводимые типы при разных параметрах типов кастомных свойств для переходов
// для специального int-треита
TEST_F(LabeledTS_1_Test, TransPropertyTypes1)
{
    // без явного указания доп. свойства вершины — д.б. только целочисленный ИД
    typedef LabeledTS<int, const char*> Lts1;
    Lts1::TrPropBundle sp1;                     // const char*

    ASSERT_TRUE((std::is_same<const char*, Lts1::TrPropBundle>::value));
    ASSERT_TRUE((std::is_same<const char*, Lts1::LabelCArg>::value));       // так будет со спец. трэитами для const char*
    ASSERT_TRUE((std::is_same<const char*, Lts1::LabelCRes>::value));
    ASSERT_TRUE((std::is_same<const char*, Lts1::TrPropBundleCArg>::value));
    
    ASSERT_TRUE((std::is_same<const char* &, Lts1::TrPropBundleRef>::value));
    ASSERT_TRUE((std::is_same<const char*, Lts1::TrPropBundleCRes>::value));    // спецтрэит

    ASSERT_TRUE((std::is_same<boost::no_property, Lts1::TrDataRes>::value));   // нет кастомных свойст у вершин

    // пользуемся знанием о sp1 в compile-time
    sp1 = "Hello World";
    ASSERT_EQ("Hello World", Lts1::extractTransLbl(sp1));

    // с явным указанием доп. свойства перехода (double), должна появиться структура
    // введем тип для посл. сравнения

    typedef LabeledTS<int, const char*, boost::no_property, double> Lts2;    
    Lts2::TrPropBundle sp2;

    ASSERT_TRUE((std::is_same<TransProperty_traits<const char*, double>::TrPropBundle,
        Lts2::TrPropBundle>::value));

    ASSERT_TRUE((std::is_same<const char*, Lts2::LabelCArg>::value));                 // так со спец. трэитами
    ASSERT_TRUE((std::is_same<const char*, Lts2::LabelCRes>::value));

    ASSERT_TRUE((std::is_same<const TransProperty_traits<const char*, double>::TrPropBundle &,
        Lts2::TrPropBundleCArg>::value));

    ASSERT_TRUE((std::is_same<TransProperty_traits<const char*, double>::TrPropBundle &,
        Lts2::TrPropBundleRef>::value));

    ASSERT_TRUE((std::is_same<const TransProperty_traits<const char*, double>::TrPropBundle &,
        Lts2::TrPropBundleCRes>::value));

    ASSERT_TRUE((std::is_same<double&, Lts2::TrDataRes>::value));   // кастомное с пл. точкой, по ссылке

    sp2.lbl = "Abc";
    sp2.prop = 3.14;
    ASSERT_EQ("Abc", Lts2::extractTransLbl(sp2));
}

//-----------------------------------------------------------------------------

TEST_F(LabeledTS_1_Test, TransPropertyTypes2)
{
    // без явного указания доп. свойства вершины — д.б. только целочисленный ИД
    typedef LabeledTS<int, double> Lts1;

    ASSERT_TRUE((std::is_same<double, Lts1::LabelCArg>::value));     // со спецтрэитом
}


//-----------------------------------------------------------------------------

// тестируем свойства, привязанные к переходу
TEST_F(LabeledTS_1_Test, TransProperties1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;
    
    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);      // идент — 1
    ASSERT_EQ(1, ts1.getStatesNum());
    ASSERT_EQ(1, ts1.getStateID(s1));               // ид = 1

    Lts1::State s2 = ts1.getOrAddState(2);
    ASSERT_EQ(2, ts1.getStatesNum());
    ASSERT_EQ(2, ts1.getStateID(s2));

    Lts1::State s3 = ts1.getOrAddState(3);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(3, ts1.getStateID(s3));

    // пока нет переходов
    ASSERT_EQ(0, ts1.getTransitionsNum());

    // добавляем первый переход 1-2, помеченный 3.14
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    ASSERT_EQ(1, ts1.getTransitionsNum());

    // добавляем второй переход 1-3, помеченный 2.4
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    ASSERT_EQ(2, ts1.getTransitionsNum());

    // еще один переход 1-2, пометка 2.4
    Lts1::Transition t12_2 = ts1.getOrAddTrans(s1, s2, 2.4);
    ASSERT_EQ(3, ts1.getTransitionsNum());

    // и вот, наконец, еще раз 1-2, помеченный тем же 3.14 — должно быть повтор
    Lts1::Transition t12_3 = ts1.getOrAddTrans(s1, s2, 3.14);
    ASSERT_EQ(3, ts1.getTransitionsNum());
    ASSERT_TRUE(t12 == t12_3);

    // попробуем к свойству транзиции обратиться
    Lts1::TrPropBundle t12_prop = ts1.getBundle(t12);
    ASSERT_EQ(3.14, t12_prop);

    Lts1::TrDataRes t12_data = ts1.getData(t12);
    Lts1::TransRes trRes1 = ts1.getFirstOutTrans(s1, 3.14);
    ASSERT_TRUE(trRes1.second);
    ASSERT_TRUE(trRes1.first == t12);
    Lts1::StateRes stRes1 = ts1.getFirstOutState(s1, 3.14);
    ASSERT_TRUE(stRes1.second);
    ASSERT_TRUE(stRes1.first == s2);

    Lts1::TransRes trRes2 = ts1.getFirstOutTrans(s1, 3);    // такого не должно быть
    ASSERT_FALSE(trRes2.second);

    Lts1::TransRes trRes3 = ts1.getFirstOutTrans(s1, 2.4);
    ASSERT_TRUE(trRes3.second);
    ASSERT_TRUE(trRes3.first == t13);           // помеченные 2.4, и первая ведет в s3, а не в s2!
}

//-----------------------------------------------------------------------------

TEST_F(LabeledTS_1_Test, TransPropertyTypes3)
{
    typedef std::string Str;

    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, Str, boost::no_property, double> Lts2;

    ASSERT_TRUE((std::is_same<const Str&, Lts2::LabelCArg>::value));     // по дефолту
}

//-----------------------------------------------------------------------------

// тестируем свойства, привязанные к переходу
TEST_F(LabeledTS_1_Test, TransProperties2)
{
    typedef std::string Str;

    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, Str, boost::no_property, double> Lts2;
    Lts2 ts2;

    // добавим несколько состояний
    Lts2::State s1 = ts2.getOrAddState(1);      // идент — 1
    ASSERT_EQ(1, ts2.getStatesNum());
    ASSERT_EQ(1, ts2.getStateID(s1));               // ид = 1

    Lts2::State s2 = ts2.getOrAddState(2);
    ASSERT_EQ(2, ts2.getStatesNum());
    ASSERT_EQ(2, ts2.getStateID(s2));

    Lts2::State s3 = ts2.getOrAddState(3);
    ASSERT_EQ(3, ts2.getStatesNum());
    ASSERT_EQ(3, ts2.getStateID(s3));

    // пока нет переходов
    ASSERT_EQ(0, ts2.getTransitionsNum());

    // добавляем первый переход 1-2, помеченный 3.14
    Lts2::Transition t12 = ts2.getOrAddTrans(s1, s2, "a");
    ASSERT_EQ(1, ts2.getTransitionsNum());

    // добавляем второй переход 1-3, помеченный 2.4
    Lts2::Transition t13 = ts2.getOrAddTrans(s1, s3, "b");
    ASSERT_EQ(2, ts2.getTransitionsNum());

    // еще один переход 1-2, пометка 3.14
    Lts2::Transition t12_2 = ts2.getOrAddTrans(s1, s2, "c");
    ASSERT_EQ(3, ts2.getTransitionsNum());

    // и вот, наконец, еще раз 1-2, помеченный тем же 3.14 — должно быть повтор
    Lts2::Transition t12_3 = ts2.getOrAddTrans(s1, s2, "a");
    ASSERT_EQ(3, ts2.getTransitionsNum());
    ASSERT_TRUE(t12 == t12_3);

    // попробуем к свойству транзиции обратиться
    Lts2::TrPropBundle t12_prop = ts2.getBundle(t12);
    ASSERT_EQ("a", t12_prop.lbl);
    //t12_prop.prop = 3.14;

    Lts2::TrDataRes t12_data = ts2.getData(t12);
    t12_data = 3.14;
    t12_prop = ts2.getBundle(t12);
    ASSERT_EQ(3.14, t12_prop.prop);         // нет ничего стоящего, но проверить невозможно, т.е. нет оператора ==

    // добавим анонимные состояния
    Lts2::State sa1 = ts2.addAnonState();
    ASSERT_EQ(4, ts2.getStatesNum());       // 4 состояния стало
    ASSERT_TRUE(ts2.getStatesNum() == ts2.getRegStatesNum() + ts2.getAnonStatesNum());
    Lts2::Transition ta12 = ts2.getOrAddTrans(s1, sa1, "anon");
    ASSERT_EQ(4, ts2.getTransitionsNum());  // и 4 же перехода
}

//-----------------------------------------------------------------------------

// удаление переходов
TEST_F(LabeledTS_1_Test, RemoveTrans1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;

    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1); 
    Lts1::State s2 = ts1.getOrAddState(2);
    Lts1::State s3 = ts1.getOrAddState(3);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(0, ts1.getTransitionsNum());

    // добавляем неск. переходов
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    Lts1::Transition t12_2 = ts1.getOrAddTrans(s1, s2, 2.4);
    ASSERT_EQ(3, ts1.getTransitionsNum());

    // удаляем переходв через валидный дескриптор
    ts1.removeTrans(t12_2);
    ASSERT_EQ(2, ts1.getTransitionsNum());

    ASSERT_TRUE(ts1.removeTrans(s1, s3, 2.4));      // t13
    ASSERT_EQ(1, ts1.getTransitionsNum());

    // еще раз по тем же координатам пытаемся удалить, не должно получиться
    ASSERT_FALSE(ts1.removeTrans(s1, s3, 2.4));      // t13 — уже не существует
    ASSERT_EQ(1, ts1.getTransitionsNum());

    // добавим анонимные состояния
    Lts1::State sa1 = ts1.addAnonState();
    ASSERT_EQ(4, ts1.getStatesNum());       // 4 состояния стало
    ASSERT_TRUE(ts1.getStatesNum() == ts1.getRegStatesNum() + ts1.getAnonStatesNum());
    Lts1::Transition ta12 = ts1.getOrAddTrans(s1, sa1, 1.23);
    ASSERT_EQ(2, ts1.getTransitionsNum());  // и еще один переход добавился

    ts1.removeTrans(ta12);
    ASSERT_EQ(1, ts1.getTransitionsNum());  // ушел переход
}


//-----------------------------------------------------------------------------

// удаление переходов 2
TEST_F(LabeledTS_1_Test, RemoveTrans2)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;

    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);
    Lts1::State s2 = ts1.getOrAddState(2);
    Lts1::State s3 = ts1.getOrAddState(3);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(0, ts1.getTransitionsNum());

    // добавляем неск. переходов
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    Lts1::Transition t12_2 = ts1.getOrAddTrans(s1, s2, 2.4);
    Lts1::Transition t23 = ts1.getOrAddTrans(s2, s3, 1);
    Lts1::Transition t33 = ts1.getOrAddTrans(s3, s3, 2);
    Lts1::Transition t31 = ts1.getOrAddTrans(s3, s1, 5);
    ASSERT_EQ(6, ts1.getTransitionsNum());

    // убираем все исходящие из s1 (их 3)
    ts1.clearOutTransitions(s1);
    ASSERT_EQ(3, ts1.getTransitionsNum());

    // убираем единственную входящую в s1
    ts1.clearInTransitions(s1);
    ASSERT_EQ(2, ts1.getTransitionsNum());

    // убираем все s3, и ничего не осталось
    ts1.clearTransitions(s3);
    ASSERT_EQ(0, ts1.getTransitionsNum());

    // добавим анонимные состояния
    Lts1::State sa1 = ts1.addAnonState();
    ASSERT_EQ(4, ts1.getStatesNum());                           // 4 состояния стало
    ASSERT_TRUE(ts1.getStatesNum() == ts1.getRegStatesNum() + ts1.getAnonStatesNum());
    Lts1::Transition t1a = ts1.getOrAddTrans(s1, sa1, 1.23);
    Lts1::Transition t2a = ts1.getOrAddTrans(s2, sa1, 2.23);
    ASSERT_EQ(2, ts1.getTransitionsNum());                      // стало два перехода

    ts1.clearInTransitions(sa1);
    ASSERT_EQ(0, ts1.getTransitionsNum());                      // опять не переходов
}


//-----------------------------------------------------------------------------

// удаление вершины 1
TEST_F(LabeledTS_1_Test, RemoveState1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;

    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);
    Lts1::State s2 = ts1.getOrAddState(2);
    Lts1::State s3 = ts1.getOrAddState(3);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(0, ts1.getTransitionsNum());

    // еще раз похожий на s1 (д.б. дубликат)
    Lts1::State s1_1 = ts1.getOrAddState(1);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_TRUE(s1 == s1_1);

    // добавляем неск. переходов
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    Lts1::Transition t12_2 = ts1.getOrAddTrans(s1, s2, 2.4);
    Lts1::Transition t23 = ts1.getOrAddTrans(s2, s3, 1);
    Lts1::Transition t33 = ts1.getOrAddTrans(s3, s3, 2);
    Lts1::Transition t31 = ts1.getOrAddTrans(s3, s1, 5);
    ASSERT_EQ(6, ts1.getTransitionsNum());

    // убираем s1 (минус 4 дуги)
    ts1.removeState(s1);
    ASSERT_EQ(2, ts1.getStatesNum());
    ASSERT_EQ(2, ts1.getTransitionsNum());

    // еще раз похожий на s1 (уже НЕ дубликат)
    Lts1::State s1_2 = ts1.getOrAddState(1);
    ASSERT_EQ(3, ts1.getStatesNum());
    
    // добавим анонимные состояния
    Lts1::State sa1 = ts1.addAnonState();
    ASSERT_EQ(4, ts1.getStatesNum());                           // 4 состояния стало   
    ASSERT_TRUE(ts1.getStatesNum() == ts1.getRegStatesNum() + ts1.getAnonStatesNum());
    Lts1::Transition t2a = ts1.getOrAddTrans(s2, sa1, 2.23);
    Lts1::Transition t3a = ts1.getOrAddTrans(s3, sa1, 3.23);
    ASSERT_EQ(4, ts1.getTransitionsNum());                      // еще два перехода

    // убираем анонимное состояние
    ts1.removeState(sa1);
    ASSERT_EQ(3, ts1.getStatesNum());                           // 3 состояния стало снова
    ASSERT_EQ(2, ts1.getTransitionsNum());                      // и на два ведущих к нему перехода меньше
}


TEST_F(LabeledTS_1_Test, ParallelTrans1)
{
    // тестируем добавление параллельных транзиций (без пометок), выбор, фильтр-итератор
    typedef LabeledTS<int, double> Lts1;

    Lts1::Graph g;
    Lts1::State s1 = boost::add_vertex(g);
    Lts1::State s2 = boost::add_vertex(g);
    Lts1::State s3 = boost::add_vertex(g);
    ASSERT_EQ(3, boost::num_vertices(g));
    ASSERT_EQ(0, boost::num_edges(g));

    Lts1::TransRes tr12 = boost::add_edge(s1, s2, g);
    Lts1::TransRes tr13 = boost::add_edge(s1, s3, g);    
    EXPECT_TRUE(tr12.second);
    EXPECT_TRUE(tr13.second);
    ASSERT_EQ(2, boost::num_edges(g));

    // добавляем параллельную дугу s1-s2
    Lts1::TransRes tr12_2 = boost::add_edge(s1, s2, g);
    EXPECT_TRUE(tr12_2.second);
    ASSERT_EQ(3, boost::num_edges(g));

    // "обычный" итератор — все выходные дуги из s1    
    Lts1::OtransIterPair s1oedges = boost::out_edges(s1, g);
    int count;
    for (count = 0; s1oedges.first != s1oedges.second; ++s1oedges.first)
        ++count;
    ASSERT_EQ(3, count);

    // теперь пытаемся фильтр сделать
    s1oedges = boost::out_edges(s1, g);
    Lts1::TargetStateFilter f(g, s2);         // предикат
    Lts1::TargStateTransIter first(f, s1oedges.first, s1oedges.second);
    Lts1::TargStateTransIter last(f, s1oedges.second, s1oedges.second);
    for (count = 0; first != last; ++first)
        ++count;
    ASSERT_EQ(2, count);

    // фильтр 1-3
    s1oedges = boost::out_edges(s1, g);
    f = Lts1::TargetStateFilter(g, s3);
    first = Lts1::TargStateTransIter(f, s1oedges.first, s1oedges.second);
    last = Lts1::TargStateTransIter(f, s1oedges.second, s1oedges.second);
    for (count = 0; first != last; ++first)
        ++count;
    ASSERT_EQ(1, count);
}


// добавение анонимных (непомеченных) переходов
TEST_F(LabeledTS_1_Test, AnonTrans1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;

    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);
    Lts1::State s2 = ts1.getOrAddState(2);
    Lts1::State s3 = ts1.getOrAddState(3);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(0, ts1.getTransitionsNum());

    // добавляем неск. помеченных переходов
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    ASSERT_EQ(2, ts1.getTransitionsNum());

    // добавляем 3 непомеченные дуги между верш 2 и 3
    Lts1::Transition ta23_1 = ts1.addAnonTrans(s2, s3);
    Lts1::Transition ta23_2 = ts1.addAnonTrans(s2, s3);
    Lts1::Transition ta23_3 = ts1.addAnonTrans(s2, s3);
    ASSERT_EQ(5, ts1.getTransitionsNum());                  // всего 5 дуг

    // добавляем 2 непомеченные дуги между верш 1 и 2
    Lts1::Transition ta12_1 = ts1.addAnonTrans(s1, s2);
    Lts1::Transition ta12_2 = ts1.addAnonTrans(s1, s2);
    ASSERT_EQ(7, ts1.getTransitionsNum());                  // всего 5 дуг

    // а какая в реальности пометка
    Lts1::TrPropBundle t13_prop = ts1.getBundle(t13);       // для помеченной
    Lts1::LabelCRes t13_lbl = ts1.extractTransLbl(t13);

    Lts1::TrPropBundle ta23_1_prop = ts1.getBundle(ta23_1); // для непомеченной
    Lts1::LabelCRes ta23_1_lbl = ts1.extractTransLbl(ta23_1);

    // убираем s2 (минус 6 дуг)
    ts1.removeState(s2);
    ASSERT_EQ(2, ts1.getStatesNum());
    ASSERT_EQ(1, ts1.getTransitionsNum());
}

//-----------------------------------------------------------------------------

// копирование СП — конструктор копирования
TEST_F(LabeledTS_1_Test, CopyConstrTS1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;

    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);
    Lts1::State s2 = ts1.getOrAddState(2);
    Lts1::State s3 = ts1.getOrAddState(3);
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    Lts1::Transition t12_2 = ts1.getOrAddTrans(s1, s2, 2.4);
    Lts1::Transition t23 = ts1.getOrAddTrans(s2, s3, 1);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(4, ts1.getTransitionsNum());

    // добавим анонимные состояния
    Lts1::State sa1 = ts1.addAnonState();
    Lts1::Transition t1a = ts1.getOrAddTrans(s1, sa1, 1.23);
    ASSERT_EQ(4, ts1.getStatesNum());                           // 4 состояния стало
    ASSERT_EQ(5, ts1.getTransitionsNum());                      // плюс еще один переход
    ASSERT_TRUE(ts1.getStatesNum() == ts1.getRegStatesNum() + ts1.getAnonStatesNum());

    // КК
    Lts1 ts2 = ts1;
    ASSERT_EQ(4, ts2.getStatesNum());           //ASSERT_EQ(3, ts2.getStatesNum());
    ASSERT_EQ(5, ts2.getTransitionsNum());      //ASSERT_EQ(4, ts2.getTransitionsNum());     
    ASSERT_TRUE(ts2.getStatesNum() == ts2.getRegStatesNum() + ts2.getAnonStatesNum());

    // проверяем, что по тем же состояниям берутся стейты в новой СП
    Lts1::StateRes ts2_s1r = ts2.getState(1);
    Lts1::StateRes ts2_s2r = ts2.getState(2);
    Lts1::StateRes ts2_s3r = ts2.getState(3);
    ASSERT_TRUE(ts2_s1r.second);
    ASSERT_TRUE(ts2_s2r.second);
    ASSERT_TRUE(ts2_s2r.second);

    Lts1::TransRes ts2_t12r = ts2.getTrans(ts2_s1r.first, ts2_s2r.first, 3.14);
    ASSERT_TRUE(ts2_t12r.second);
    // можно и для остальных, в принципе, проверить
}


//-----------------------------------------------------------------------------

// операция копирования
TEST_F(LabeledTS_1_Test, CopyOperTS1)
{
    // идентификаторы — целые, доп. свойств нет
    typedef LabeledTS<int, double> Lts1;
    Lts1 ts1;

    // добавим пару состояний
    Lts1::State s1 = ts1.getOrAddState(1);
    Lts1::State s2 = ts1.getOrAddState(2);
    Lts1::State s3 = ts1.getOrAddState(3);
    Lts1::Transition t12 = ts1.getOrAddTrans(s1, s2, 3.14);
    Lts1::Transition t13 = ts1.getOrAddTrans(s1, s3, 2.4);
    Lts1::Transition t12_2 = ts1.getOrAddTrans(s1, s2, 2.4);
    Lts1::Transition t23 = ts1.getOrAddTrans(s2, s3, 1);
    ASSERT_EQ(3, ts1.getStatesNum());
    ASSERT_EQ(4, ts1.getTransitionsNum());

    // добавим анонимные состояния
    Lts1::State sa1 = ts1.addAnonState();
    Lts1::Transition t1a = ts1.getOrAddTrans(s1, sa1, 1.23);
    ASSERT_EQ(4, ts1.getStatesNum());                           // 4 состояния стало
    ASSERT_EQ(5, ts1.getTransitionsNum());                      // плюс еще один переход
    ASSERT_TRUE(ts1.getStatesNum() == ts1.getRegStatesNum() + ts1.getAnonStatesNum());

    // другой объект
    Lts1 ts2;
    Lts1::State ts2_s1 = ts2.getOrAddState(10);
    Lts1::State ts2_s2 = ts2.getOrAddState(20);    
    Lts1::Transition ts2_t12 = ts2.getOrAddTrans(ts2_s1, ts2_s2, 2.718281828);
    ASSERT_EQ(2, ts2.getStatesNum());
    ASSERT_EQ(1, ts2.getTransitionsNum());

    // копируем!
    ts2 = ts1;
    ASSERT_EQ(4, ts2.getStatesNum());               //ASSERT_EQ(3, ts2.getStatesNum());
    ASSERT_EQ(5, ts2.getTransitionsNum());          //ASSERT_EQ(4, ts2.getTransitionsNum());
    ASSERT_TRUE(ts2.getStatesNum() == ts2.getRegStatesNum() + ts2.getAnonStatesNum());

    // проверяем, что теперь в ts2 то же самое, что и в ts1
    Lts1::StateRes ts2_s1r = ts2.getState(1);
    Lts1::StateRes ts2_s2r = ts2.getState(2);
    Lts1::StateRes ts2_s3r = ts2.getState(3);
    ASSERT_TRUE(ts2_s1r.second);
    ASSERT_TRUE(ts2_s2r.second);
    ASSERT_TRUE(ts2_s2r.second);

    Lts1::TransRes ts2_t12r = ts2.getTrans(ts2_s1r.first, ts2_s2r.first, 3.14);
    ASSERT_TRUE(ts2_t12r.second);
}

//==============================================================================
// тестирование некоторых побочных моментов
//==============================================================================


TEST_F(LabeledTS_1_Test, BglTypes1)
{
    typedef LabeledTS<int, const char*> Lts1;
    
    ASSERT_TRUE((std::is_same<  boost::listS, 
                                Lts1::Graph::out_edge_list_selector >::value));
 
    int ta = BGL_traits<Lts1::Graph>::a;
    ASSERT_EQ(1, ta);
}

