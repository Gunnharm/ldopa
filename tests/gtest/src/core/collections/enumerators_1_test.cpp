////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for xilib Enumerators.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      29.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// HOW TO SET A TEST ENVIRONMENT:
/// one need to define CSVLOG1_TEST_LOGS_BASE_DIR directive to set it to a
/// machine/user specific value
///
///
/// Notes on how to avoid using underscores in test case names: 
/// https://groups.google.com/forum/#!topic/googletestframework/N5A07bgEvp4
///
////////////////////////////////////////////////////////////////////////////////


#include <gtest/gtest.h>


// std
#include <memory>


// xilib
#include "xi/collections/enumerators.hpp"


#include "custom_enums.h"


#pragma region Test Class

// Tests SQLiteLog class. Part 1
class Enumerators_1_Test : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        //SetUpGraph();
    } 

    virtual void TearDown()
    {
        //delete _gr1;
    }

}; // class SQLiteLog_1_Test


#pragma endregion // Test Class


//===================================================================================

using namespace xi::collections;



//------------- Tests themselves --------------


TEST_F(Enumerators_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}

//-----------------------------------------------------------------------------

TEST_F(Enumerators_1_Test, IntVectorIter1)
{
    using xi::collections::tests::IntVectorEnumerator; 
    
    std::vector<int> v1 = {10, 20, 30};
    IntVectorEnumerator e(v1);

    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(10, e.getNext());
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(20, e.getNext());
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(30, e.getNext());
        
    // no more
    EXPECT_FALSE(e.hasNext());
    ASSERT_THROW(e.getNext(), std::range_error);   // нет такого элемента


    // reset and has again
    e.reset();
    EXPECT_TRUE(e.hasNext());
}


//-----------------------------------------------------------------------------

TEST_F(Enumerators_1_Test, AltIntVectorIter1)
{
    // вместо специально определенного с нуля типа используем удобный шаблон

    //using xi::collections::tests::IntVectorEnumerator;
    typedef Enumerator4Iterator<std::vector<int>::iterator> AltIntVectorEnumerator;


    std::vector<int> v1 = { 10, 20, 30 };
    AltIntVectorEnumerator e(v1.begin(), v1.end());
    
    //IntVectorEnumerator e(v1);

    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(10, e.getNext());
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(20, e.getNext());
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(30, e.getNext());

    // no more
    EXPECT_FALSE(e.hasNext());
    ASSERT_THROW(e.getNext(), std::range_error);   // нет такого элемента


    // reset and has again
    e.reset();
    EXPECT_TRUE(e.hasNext());
}


//-----------------------------------------------------------------------------

TEST_F(Enumerators_1_Test, StrVectorIter1)
{
    using xi::collections::tests::VectorOfStringsEnumerator;

    std::vector<std::string> v1 = { "Abc", "Xyz" };
    VectorOfStringsEnumerator e(v1);

    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ("Abc", e.getNext());
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ("Xyz", e.getNext());


    // no more
    EXPECT_FALSE(e.hasNext());
    ASSERT_THROW(e.getNext(), std::range_error);   // нет такого элемента


    // reset and has again
    e.reset();
    EXPECT_TRUE(e.hasNext());
}

//-----------------------------------------------------------------------------

TEST_F(Enumerators_1_Test, AltStrVectorIter1)
{
    typedef std::vector < std::string > StrVector;
    typedef StrVector::iterator StrVectorIter;

    typedef Enumerator4Iterator<StrVectorIter> StrVectorEnumerator;


    std::vector<std::string> v1 = { "Abc", "Xyz" };
    StrVectorEnumerator e(v1.begin(), v1.end());


    EXPECT_TRUE(e.hasNext());
    std::string& s = e.getNext();
    //EXPECT_EQ("Abc", e.getNext());
    EXPECT_EQ("Abc", s);

    // специально меняем, чтобы после reset-а проверить
    s = "ABC";


    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ("Xyz", e.getNext());


    // no more
    EXPECT_FALSE(e.hasNext());
    ASSERT_THROW(e.getNext(), std::range_error);   // нет такого элемента


    // reset and has again
    e.reset();
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ("ABC", e.getNext());      // проверяем, что удалось изменить
}

//-----------------------------------------------------------------------------

TEST_F(Enumerators_1_Test, StrVectorConstIter1)
{

    // здесь используем константное перечисление

    typedef std::vector < std::string > StrVector;
    typedef StrVector::const_iterator StrVectorCIter;

    typedef Enumerator4ConstIterator<StrVectorCIter> StrVectorConstEnumerator;
    

    std::vector<std::string> v1 = { "Abc", "Xyz" };
    StrVectorConstEnumerator e(v1.cbegin(), v1.cend());


    EXPECT_TRUE(e.hasNext());
    const std::string& s = e.getNext();
    ////EXPECT_EQ("Abc", e.getNext());
    EXPECT_EQ("Abc", s);

    // тут конст. ссылочка — менять нельзя!
    //s = "ABC";


    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ("Xyz", e.getNext());


    // no more
    EXPECT_FALSE(e.hasNext());
    ASSERT_THROW(e.getNext(), std::range_error);    // нет такого элемента


    // reset and has again
    e.reset();
    EXPECT_TRUE(e.hasNext());
    //EXPECT_EQ("ABC", e.getNext());                // ничего не поменяли ж!
}

//-----------------------------------------------------------------------------

TEST_F(Enumerators_1_Test, IntVectorConstIter1)
{
    
    // здесь используем константное перечисление

    typedef std::vector < int > IntVector;
    typedef IntVector::const_iterator IntVectorCIter;


    // далее допустимы обе формы, специально использую [2], чтобы показать, что это НЕ (конст.) ссылка
    //typedef Enumerator4ConstIterator<IntVectorCIter> IntVectorConstEnumerator;    // [1]
    typedef BaseEnumerator4Iterator<IntVectorCIter,
        IConstEnumerator_traits<int> > //IntVectorCIter::value_type> >
        IntVectorConstEnumerator;                                                   // [2]


    std::vector<int> v1 = { 10, 20 };
    IntVectorConstEnumerator e(v1.cbegin(), v1.cend());

    //IntVectorEnumerator e(v1);

    EXPECT_TRUE(e.hasNext());
    const int& i = e.getNext();     // вообще-т такая привязка не должна получаться! но получается,
                                    // видимо, из-за того, что допустима конструкция: const int& a = 10;
                                    // а так-то, похоже, тут по значению приходит, т.к. допустимо
                                    // определение типа вида [2], а там int явно идет,
                                    // IConstEnumerator_traits<int> явно идет, в котором TRet совершенно
                                    // явно написан, как int, т.е. ссылке там появиться неоткуда!
    EXPECT_EQ(10, i);
    //EXPECT_EQ(10, e.getNext());
    EXPECT_TRUE(e.hasNext());
    EXPECT_EQ(20, e.getNext());


    // no more
    EXPECT_FALSE(e.hasNext());
    ASSERT_THROW(e.getNext(), std::range_error);   // нет такого элемента


    // reset and has again
    e.reset();
    EXPECT_TRUE(e.hasNext());
}