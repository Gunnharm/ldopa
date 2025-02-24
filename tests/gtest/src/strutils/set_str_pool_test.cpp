///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing string pool class based on set
///
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// xilib
#include "xi/strutils/set_str_pool.h"
//#include "xi/strutils/setStrPool.h"




TEST(setStrPool, testInsert1)
{
    using namespace xi::strutils;

    SetStrPool strPool;
    SetStrPool::StrsSet& poolSet = strPool.getPool();

    //std::string ss;
    const std::string* sp1 = strPool.insert(std::string("Abc"));
    const std::string* sp2 = strPool.insert(std::string("Xyz"));
    const std::string* sp3 = strPool.insert(std::string("Abc"));
    const std::string* sp4 = strPool.insert(std::string("abc"));

    EXPECT_EQ(sp1, sp3);

    size_t setSize = poolSet.size();
    EXPECT_EQ(setSize, 3);

}


TEST(setStrPool, testSubscriptOperator1)
{
    using namespace xi::strutils;

    SetStrPool strPool;
    SetStrPool::StrsSet& poolSet = strPool.getPool();

    //std::string ss;
    const std::string* sp1 = strPool["Abc"];
    const std::string* sp2 = strPool["Xyz"]; 
    const std::string* sp3 = strPool["Abc"]; 
    const std::string* sp4 = strPool["abc"]; 

    EXPECT_EQ(sp1, sp3);

    size_t setSize = poolSet.size();
    EXPECT_EQ(setSize, 3);

    strPool.clear();
    setSize = poolSet.size();
    EXPECT_EQ(setSize, 0);

}