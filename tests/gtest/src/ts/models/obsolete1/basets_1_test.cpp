///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing BaseTs module
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "xi/ldopa/ts/models/obsolete1/basets.h"

//==============================================================================
// class TsPtrStateId
//==============================================================================

TEST(TsPtrStateId1, equality1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    TsPtrStateId st1;
    int ts1_c = st1.getPtrs().capacity();

    TsPtrStateId st2(3);        // set the size of vector
    int ts2_c = st2.getPtrs().capacity();
    EXPECT_EQ(ts2_c, 3);


    // adds ptrs
    st1.getPtrs().push_back(&a);
    st1.getPtrs().push_back(&b);
    st1.getPtrs().push_back(&c);

    st2.getPtrs().push_back(&a);
    st2.getPtrs().push_back(&b);
    st2.getPtrs().push_back(&c);

    bool eq = (st1 == st2);
    EXPECT_EQ(eq, true);


    // the same through the base ptrs
    ITsStateId* bst1 = &st1;
    ITsStateId* bst2 = &st2;

    bool eq2 = bst1->isEqualTo(bst2);    
    EXPECT_EQ(true, eq2);
}

TEST(TsPtrStateId1, append1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    TsPtrStateId st1(3);

    TsPtrStateId st2(3);        // set the size of vector

    // adds ptrs
    st1.append(&a);
    st1.append(&b);
    st1.append(&c);

    st2.append(&a);
    st2.append(&b);
    st2.append(&c);

    bool eq = (st1 == st2);
    EXPECT_EQ(eq, true);
}


xi::ldopa::ts::obsolete1::TsPtrStateId moveSemantics1_helper1()
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12;

    TsPtrStateId s;
    s.append(&a);

    return s;
}

xi::ldopa::ts::obsolete1::ITsStateId& moveSemantics1_helper2()
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12;

    TsPtrStateId s;
    s.append(&a);

    return s;
}


TEST(TsPtrStateId1, moveSemantics1)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    TsPtrStateId st1(3);

    // adds ptrs
    st1.append(&a);
    st1.append(&b);
    st1.append(&c);

    // copy constructor
    TsPtrStateId st2(st1);

    // copy constructor
    TsPtrStateId st3 = st1;
    st1.append(&a);

    // copy operation
    st3 = st1;


    // moving
    st2 = std::move(st1);

    //
    TsPtrStateId st5 = moveSemantics1_helper1();

    st5 = st1;
    st5 = moveSemantics1_helper1();

    // black magic
    ITsStateId& st6 = moveSemantics1_helper2();


    ITsStateId& st7 = moveSemantics1_helper1();
    bool eq = st5.isEqualTo(&st7);
}


TEST(TsPtrStateId1, lessThan1)
{
    // also tests initializers lists
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts::obsolete1;

    int a = 12, b = 14, c = 15;

    TsPtrStateId st1 {&a, &b};

    TsPtrStateId st2 { &a, &b, &c };;
    EXPECT_EQ(true, st1 < st2);
    EXPECT_EQ(false, st2 < st1);
    EXPECT_EQ(false, st1 > st2);        // syntax sugar
    EXPECT_EQ(false, st1 == st2);

    TsPtrStateId st3{ &a, &c };
    EXPECT_NE(true, st1 == st3);        // should not be equal
    
    bool bLc = st1 < st3;
    bool cLb = st3 < st1;
    EXPECT_NE(bLc, cLb);
}
