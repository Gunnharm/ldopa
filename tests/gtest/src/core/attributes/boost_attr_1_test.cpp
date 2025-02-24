////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for xilib boost_attr lib.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      05.09.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
///
///
/// Notes on how to avoid using underscores in test case names: 
/// https://groups.google.com/forum/#!topic/googletestframework/N5A07bgEvp4
///
////////////////////////////////////////////////////////////////////////////////


#include <gtest/gtest.h>
#include <exception>



// xilib
#include "xi/attributes/obsolete1/boost_attr.h"     


// boost
#include "boost/variant.hpp"            // boost::variant
#include "boost/any.hpp"                // для пробы



#pragma region Test Class


class BoostAttr_1_Test : public ::testing::Test {
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

//using namespace xi::collections;



//------------- Tests itselves --------------


TEST_F(BoostAttr_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}

//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, BoostVar1)
{
    using namespace xi::attributes;

    int s1 = sizeof(std::string);
    int s2 = sizeof(Bitset32);
    int s3 = sizeof(Bitset64);
    int s3_1 = sizeof(Bitset128);
    int s4 = sizeof(IDestructableObject);
    int s5 = sizeof(DestrObjSharedPtr);
    int s6 = sizeof(StringSharedPtr);
    
    int s7 = sizeof(BaseVarAttribute);
    int s8 = sizeof(VarAttribute);
    

    // теперь руками набустим...

    typedef boost::variant< // numbers
        char,
        unsigned char,
        int,
        unsigned int,
        double,

        // bitsets
        Bitset32,
        Bitset64,
        //Bitset128,

        // pointers
        void*,                                      // memory-unmanaged object pointer
        DestrObjSharedPtr,                          // memory-managed object pointer

        // shared pointers to some std objects
        StringSharedPtr//,

        // std::
        //std::string                                 // sizeof(std::string) = 32, adding it here makes sizeof(VarAttribute) = 40 instead od 16
    > BV1;

    int s10 = sizeof(BV1);


    typedef boost::variant< // numbers
        boost::blank,                               // для представление нетипа (не инициал.)
        char,
        unsigned char,
        int,
        unsigned int,
        double

        ,xi::types::TInt64,                              // 4-й теперь
        xi::types::TQword

    > BV2;

    int s11 = sizeof(BV2);


    typedef boost::variant< // numbers
        char
    > BV3;

    int s21 = sizeof(BV3);


    //
    //int s22 = sizeof(BaseVarAttribute1);
    int s22 = sizeof(VarAttribute1::BaseVariant);
    

    // boost::any

    boost::any a1 = 10;
    int s31 = sizeof(a1);

}

//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, VarAttribute1Types1)
{
    using namespace xi::attributes;
 
    VarAttribute1 va0;          // что тут по умолчанию?

    EXPECT_TRUE(va0.isEmpty());
    va0 = 42;
    EXPECT_FALSE(va0.isEmpty());
    va0.clear();
    EXPECT_TRUE(va0.isEmpty());

    VarAttribute1::AType va0_t = va0.getType();

    // наблюдаем за областью видимости
    {
        VarAttribute1 va1 = 42;     // int?
        VarAttribute1::AType va1_t = va1.getType();

        

        va1 = (char)(42);
        va1_t = va1.getType();

        va1 = (unsigned char)(42);
        va1_t = va1.getType();

        std::string* abcstr = new std::string("Abc");
        StringSharedPtr sp1(abcstr);
        va1 = sp1;
        va1_t = va1.getType();

        // const c-str
        char* ccs = "Const C-str";
        va1 = ccs;
        va1_t = va1.getType();          // тут charptr без const получился!

        va1 = (const char*)ccs;         // укажем явно
        va1_t = va1.getType();          

        // c-str
        char* cs = new char[10];
        cs[0] = 'X'; cs[1] = 'y'; cs[2] = 'z'; cs[3] = '\0';
        va1 = cs;
        va1_t = va1.getType();

        // теперь в автоуказател
        //CStrSharedPtr csshp(cs);
        CStrSharedArr csshp(cs);
        va1 = csshp;
        va1_t = va1.getType();
    }

}

//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, VarAttribute1ToStrVisitor1)
{
    using namespace xi::attributes;


    boost::variant < int, char, std::string > va1;
    VarAttributeToStrDefVisitor v;
    std::string s;

    va1 = 42;
    s = boost::apply_visitor(v, va1);
    
    va1 = (char)(42);
    s = boost::apply_visitor(v, va1);


}


//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, VarAttribute1ToStrVisitor2)
{
    using namespace xi::attributes;

    // наблюдаем за областью видимости
    {

        VarAttributeToStrDefVisitor v;
        std::string s;
        std::string s1;     // для встроенного метода

        VarAttribute1 va1 = 42;     // int?
        VarAttribute1::AType va1_t = va1.getType();

        // s = v(va1);      // так нельзя с визитором!
        // http://www.boost.org/doc/libs/1_64_0/doc/html/variant.html
        //s = boost::apply_visitor(VarAttribute1ToStrVisitor(), va1);     // вот так надо!
        //s = boost::apply_visitor(v, va1);                               // или вот так
        s = boost::apply_visitor(v, va1.getRef());                               // или вот так
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("42", s);

        va1 = (char)(42);
        //va1_t = va1.getType();
        //s = boost::apply_visitor(v, va1); 
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("*", s);                                  // здесь char как символ!

        va1 = (unsigned char)(42);
        //va1_t = va1.getType();
        //s = boost::apply_visitor(v, va1);
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("42", s);                                 // а здесь — как целое

        std::string* abcstr = new std::string("Abc");
        StringSharedPtr sp1(abcstr);
        va1 = sp1;
        //va1_t = va1.getType();
        //s = boost::apply_visitor(v, va1);
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("Abc", s);

        // const c-str
        char* ccs = "Const C-str";
        va1 = ccs;
        //va1_t = va1.getType();          // тут charptr без const получился!
        //s = boost::apply_visitor(v, va1);
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("Const C-str", s);

        va1 = (const char*)ccs;         // укажем явно
        //va1_t = va1.getType();
        //s = boost::apply_visitor(v, va1);
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("Const C-str", s);

        // c-str
        char* cs = new char[10];
        cs[0] = 'X'; cs[1] = 'y'; cs[2] = 'z'; cs[3] = '\0';
        va1 = cs;
        //va1_t = va1.getType();
        //s = boost::apply_visitor(v, va1);
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("Xyz", s);

        // теперь в автоуказател
        //CStrSharedPtr csshp(cs);
        CStrSharedArr csshp(cs);
        va1 = csshp;
        //va1_t = va1.getType();
        //s = boost::apply_visitor(v, va1);
        s = boost::apply_visitor(v, va1.getRef());
        s1 = va1.toString();
        EXPECT_EQ(s, s1);
        EXPECT_EQ("Xyz", s);
    }

}

//-----------------------------------------------------------------------------



TEST_F(BoostAttr_1_Test, VarAttribute1GetAs1)
{
    using namespace xi::attributes;

    VarAttribute1 va1 = 42;     // int?
    VarAttribute1::AType va1_t = va1.getType();
    EXPECT_EQ(VarAttribute1::tInt, va1_t);
    EXPECT_EQ(42, va1.asInt());
    EXPECT_EQ(nullptr, va1.asStrP());
    //ASSERT_THROW(va1.asStr(), std::runtime_error);


    // str
    std::string* abcstr = new std::string("Abc");
    StringSharedPtr sp1(abcstr);
    va1 = sp1;
    EXPECT_EQ(*abcstr, *(va1.asStrP()));
    EXPECT_EQ(*abcstr, va1.asStr());


    

}

//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, VarAttribute1Equality1)
{
    using namespace xi::attributes;

    VarAttribute1 va1 = 42;     // int?
    VarAttribute1 va2 = 42;     // int?

    bool r1 = (va1 == va2);
    EXPECT_TRUE(r1);

    va2 = 43;

    bool r2 = (va1 != va2);
    EXPECT_TRUE(r2);
}

//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, VarAttribute1Inequality1)
{
    using namespace xi::attributes;

    VarAttribute1 va1 = 42;     // int?
    VarAttribute1 va2 = 42;     // int?

    bool r1 = (va1 < va2);
    EXPECT_FALSE(r1);

    va2 = 43;

    bool r2 = (va1 < va2);
    EXPECT_TRUE(r2);
}



//-----------------------------------------------------------------------------

// равенство в более сложных случаях: std::string
TEST_F(BoostAttr_1_Test, VarAttribute1Equality2)
{
    using namespace xi::attributes;


    // умные указатели на std::string
    std::string* s1 = new std::string("Abc");
    std::string* s2 = new std::string("Abc");

    EXPECT_TRUE(*s1 == *s2);            // объекты равны
    EXPECT_FALSE(s1 == s2);             // а указатели — отнюдь!

    // оборачиваем в умные указатели
    xi::attributes::StringSharedPtr sp1(s1);
    xi::attributes::StringSharedPtr sp2(s2);

    // умные указатели совсем не обязаны быть равными
    EXPECT_FALSE(sp1 == sp2);
    EXPECT_TRUE(*sp1 == *sp2);         // а объекты через них — да

    VarAttribute1 va1 = sp1;
    VarAttribute1 va2 = sp2;
    EXPECT_TRUE(va1 == va2);           // TODO: тут должно получиться трюэ!


    // то же самое, только с c-строками
}

//-----------------------------------------------------------------------------

// равенство в более сложных случаях: c-строки
TEST_F(BoostAttr_1_Test, VarAttribute1Equality3)
{
    using namespace xi::attributes;


    // умные указатели на std::string
    char* s1 = _strdup("Abc");
    char* s2 = _strdup("Abc");

    //EXPECT_TRUE(*s1 == *s2);            // объекты равны
    EXPECT_TRUE(strcmp(s1, s2) == 0);
    EXPECT_FALSE(s1 == s2);             // а указатели — отнюдь! — нифига, тоже равны, т.к. мерджинг строк

    // оборачиваем в умные указатели
    xi::attributes::CStrSharedArr sp1(s1);
    xi::attributes::CStrSharedArr sp2(s2);

    // умные указатели совсем не обязаны быть равными
    EXPECT_FALSE(sp1 == sp2);
    EXPECT_TRUE(strcmp(sp1.get(), sp2.get()) == 0);

    VarAttribute1 va1 = sp1;
    VarAttribute1 va2 = sp2;
    EXPECT_EQ(VarAttribute1::tCStrSharedArr, va1.getType());
    EXPECT_EQ(VarAttribute1::tCStrSharedArr, va2.getType());
    EXPECT_TRUE(va1 == va2);           // TODO: тут должно получиться трюэ!
}


//-----------------------------------------------------------------------------

// сравнение на меньше в более сложных случаях: std::string
TEST_F(BoostAttr_1_Test, VarAttribute1LessThan2)
{
    using namespace xi::attributes;


    // умные указатели на std::string
    std::string* s1 = new std::string("A");
    std::string* s2 = new std::string("B");

    EXPECT_TRUE(*s1 < *s2);            
    EXPECT_TRUE(*s2 > *s1);           
    //EXPECT_FALSE(s1 == s2);             // а указатели — отнюдь!

    // оборачиваем в умные указатели
    xi::attributes::StringSharedPtr sp1(s1);
    xi::attributes::StringSharedPtr sp2(s2);

    // умные указатели - отношение между ними неизвестно
    //EXPECT_FALSE(sp1 == sp2);
    EXPECT_TRUE(*sp1 < *sp2);         // а c объектами через них — известно
    EXPECT_TRUE(*sp2 > *sp1);         // а c объектами через них — известно

    VarAttribute1 va1 = sp1;
    VarAttribute1 va2 = sp2;
    EXPECT_TRUE(va1 < va2);
    EXPECT_TRUE(va2 > va1);


    // то же самое, только с c-строками
}

//-----------------------------------------------------------------------------

// конструируем строковые атрибуты попроще
TEST_F(BoostAttr_1_Test, VarAttribute1StringsConstr1)
{
    using namespace xi::attributes;

    VarAttribute1 va1 = std::string("Abc");
    EXPECT_EQ(VarAttribute1::tStringSharedPtr, va1.getType());

    VarAttribute1 va2 = new std::string("Abc");
    EXPECT_EQ(VarAttribute1::tStringSharedPtr, va2.getType());

    EXPECT_TRUE(va1 == va2);


    // и статическим конструктором еще
    VarAttribute1 va3 = VarAttribute1::ss("Abc");
    EXPECT_EQ(VarAttribute1::tStringSharedPtr, va3.getType());
    EXPECT_TRUE(va3 == va1);
}


//-----------------------------------------------------------------------------

TEST_F(BoostAttr_1_Test, Types1Inequality1)
{

    using namespace xi::attributes;


    typedef boost::variant< // numbers
        boost::blank,                               // для представление нетипа (не инициал.)
        
        char,
        unsigned char,
        int,
        unsigned int,
        
        xi::types::TInt64,                          // 4-й теперь
        xi::types::TQword,

        double,

        // bitsets
        //Bitset32,                     // !!! вот эти ребята не дадут сравнение сделать!
        //Bitset64//,

        // pointers
        void*,                                      // memory-unmanaged object pointer
        const void*,
        char*,                                      // for c-strings
        const char* ,

        // shared_ptr<>
        DestrObjSharedPtr,                          // memory-managed object pointer
        CStrSharedPtr,                              // c-string
        StringSharedPtr                             // std::string
    > BV2;


    BV2 o1, o2;
    bool l = o1 < o2;



    Bitset64 bs;
    bs[0] = true;
    bs[1] = false;
    bs[2] = true;

    int iBs = (int)(bs.to_ulong());


}



