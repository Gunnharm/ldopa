///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing BidiGraph with fixture 1
///
/// HOW TO SET A TEST ENVIRONMENT:
/// one need to define CSVLOG1_TEST_LOGS_BASE_DIR directive to set it to a
/// machine/user specific value
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// xilib
#include "xi/strutils/substr.h"




TEST(substr1, appendSubstr1)
{
    using namespace xi::strutils;
    
    std::string ss;

    appendSubstr("asd;xyz;klm", std::make_pair(0, 3), ss);
    EXPECT_EQ(ss, std::string("asd"));

    ss.clear();
    appendSubstr("asd;xyz;klm", 0, 3, ss);
    EXPECT_EQ(ss, std::string("asd"));

    ss.clear();
    appendSubstr("asd;xyz;klm", 9, 11, ss);
    EXPECT_EQ(ss, std::string("lm"));
}


TEST(substr1, extractSubstr1)
{
    using namespace xi::strutils;

    std::string ss;

    ss = extractSubstr("asd;xyz;klm", std::make_pair(0, 3));
    EXPECT_EQ(ss, std::string("asd"));

    ss.clear();
    ss = extractSubstr("asd;xyz;klm", 0, 3);
    EXPECT_EQ(ss, std::string("asd"));

    ss.clear();
    ss = extractSubstr("asd;xyz;klm", 9, 11);
    EXPECT_EQ(ss, std::string("lm"));
}


TEST(substr1, splitStr1)
{
    using namespace xi::strutils;

    VectorOfPairOfIndices pr;
    //std::string s1 = "asd;xyz;klm";

    //size_t num = splitStr(s1, ';', pr);

    size_t num = splitStr("asd;xyz;klm", ';', pr);
    EXPECT_EQ(num, 3);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 3);
    EXPECT_TRUE(pr[1].first == 4 && pr[1].second == 7);
    EXPECT_TRUE(pr[2].first == 8 && pr[2].second == 11);

    // ";a"
    pr.clear();
    num = splitStr(";a", ';', pr);
    EXPECT_EQ(num, 2);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 2);

    // "a;"
    pr.clear();
    num = splitStr("a;", ';', pr);
    EXPECT_EQ(num, 2);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
    EXPECT_TRUE(pr[1].first == 2 && pr[1].second == 2);

    // "a;;"
    pr.clear();
    num = splitStr("a;;", ';', pr);
    EXPECT_EQ(num, 3);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
    EXPECT_TRUE(pr[1].first == 2 && pr[1].second == 2);
    EXPECT_TRUE(pr[2].first == 3 && pr[2].second == 3);

    // ";;"
    pr.clear();
    num = splitStr(";;", ';', pr);
    EXPECT_EQ(num, 3);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 1);
    EXPECT_TRUE(pr[2].first == 2 && pr[2].second == 2);

    // ";"
    pr.clear();
    num = splitStr(";", ';', pr);
    EXPECT_EQ(num, 2);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 1);

    // ""
    pr.clear();
    num = splitStr("", ';', pr);
    EXPECT_EQ(num, 0);

    // ";;a"
    pr.clear();
    num = splitStr(";;a", ';', pr);
    EXPECT_EQ(num, 3);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 1);
    EXPECT_TRUE(pr[2].first == 2 && pr[2].second == 3);

    // "a"
    pr.clear();
    num = splitStr("a", ';', pr);
    EXPECT_EQ(num, 1);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);

}


TEST(substr1, splitStr2)
{
    using namespace xi::strutils;

    VectorOfPairOfIndices pr;

    size_t num = splitStr("asd;xyz;klm", ';', pr);
    EXPECT_EQ(num, 3);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 3);
    EXPECT_TRUE(pr[1].first == 4 && pr[1].second == 7);
    EXPECT_TRUE(pr[2].first == 8 && pr[2].second == 11);

    // ";a"
    pr.clear();
    num = splitStr(";a", ';', pr, true);
    EXPECT_EQ(num, 1);
    EXPECT_TRUE(pr[0].first == 1 && pr[0].second == 2);

    // "a;"
    pr.clear();
    num = splitStr("a;", ';', pr, true);
    EXPECT_EQ(num, 1);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);

    // "a;;"
    pr.clear();
    num = splitStr("a;;", ';', pr, true);
    EXPECT_EQ(num, 1);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);

    // ";;"
    pr.clear();
    num = splitStr(";;", ';', pr, true);
    EXPECT_EQ(num, 0);

    // ";"
    pr.clear();
    num = splitStr(";", ';', pr, true);
    EXPECT_EQ(num, 0);

    // ""
    pr.clear();
    num = splitStr("", ';', pr, true);
    EXPECT_EQ(num, 0);

    // ";;a"
    pr.clear();
    num = splitStr(";;a", ';', pr, true);
    EXPECT_EQ(num, 1);
    EXPECT_TRUE(pr[0].first == 2 && pr[0].second == 3);

    // "a"
    pr.clear();
    num = splitStr("a", ';', pr, true);
    EXPECT_EQ(num, 1);
    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);

}

TEST(substr1, splitStr3)
{
    using namespace xi::strutils;

    VectorOfStrings sstrs;

    size_t num = splitStr("asd;xyz;klm", ';', sstrs);
    EXPECT_EQ(num, 3);
    EXPECT_EQ(sstrs[0], std::string("asd"));
    EXPECT_EQ(sstrs[1], std::string("xyz"));
    EXPECT_EQ(sstrs[2], std::string("klm"));

}

