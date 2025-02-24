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
//#include "xi/strutils/searchAlgos.h"


//// ldopa
//#include "xi/ldopa/log/CSVLog.h"
//#include "xi/ldopa/utils.h"
//
//
//// test settings file
//#include "xi/ldopa/log/CSVLog_Test_Settings.h"
//
//
//// std
//#include <fstream>




//TEST(searchAlgos1, test1)
//{
//    int a = 1;
//    EXPECT_EQ(a, 1); // << "Some Text";
//    
//}

//
//TEST(searchAlgos1, splitStr1)
//{
//    using namespace xi::strutils;
//    
//    VectorOfPairOfIndices pr;
//    //std::string s1 = "asd;xyz;klm";
//    
//    //size_t num = splitStr(s1, ';', pr);
//    
//    size_t num = splitStr("asd;xyz;klm", ';', pr);   
//    EXPECT_EQ(num, 3);   
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 3);
//    EXPECT_TRUE(pr[1].first == 4 && pr[1].second == 7);
//    EXPECT_TRUE(pr[2].first == 8 && pr[2].second == 11);
//        
//    // ";a"
//    pr.clear();
//    num = splitStr(";a", ';', pr);
//    EXPECT_EQ(num, 2);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
//    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 2);
//
//    // "a;"
//    pr.clear();
//    num = splitStr("a;", ';', pr);
//    EXPECT_EQ(num, 2);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
//    EXPECT_TRUE(pr[1].first == 2 && pr[1].second == 2);
//
//    // "a;;"
//    pr.clear();
//    num = splitStr("a;;", ';', pr);
//    EXPECT_EQ(num, 3);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
//    EXPECT_TRUE(pr[1].first == 2 && pr[1].second == 2);
//    EXPECT_TRUE(pr[2].first == 3 && pr[2].second == 3);
//
//    // ";;"
//    pr.clear();
//    num = splitStr(";;", ';', pr);
//    EXPECT_EQ(num, 3);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
//    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 1);
//    EXPECT_TRUE(pr[2].first == 2 && pr[2].second == 2);
//
//    // ";"
//    pr.clear();
//    num = splitStr(";", ';', pr);
//    EXPECT_EQ(num, 2);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
//    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 1);
//
//    // ""
//    pr.clear();
//    num = splitStr("", ';', pr);
//    EXPECT_EQ(num, 0);
//
//    // ";;a"
//    pr.clear();
//    num = splitStr(";;a", ';', pr);
//    EXPECT_EQ(num, 3);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 0);
//    EXPECT_TRUE(pr[1].first == 1 && pr[1].second == 1);
//    EXPECT_TRUE(pr[2].first == 2 && pr[2].second == 3);
//
//    // "a"
//    pr.clear();
//    num = splitStr("a", ';', pr);
//    EXPECT_EQ(num, 1);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
//
//}
//
//
//TEST(searchAlgos1, splitStr2)
//{
//    using namespace xi::strutils;
//
//    VectorOfPairOfIndices pr;
//
//    size_t num = splitStr("asd;xyz;klm", ';', pr);
//    EXPECT_EQ(num, 3);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 3);
//    EXPECT_TRUE(pr[1].first == 4 && pr[1].second == 7);
//    EXPECT_TRUE(pr[2].first == 8 && pr[2].second == 11);
//
//    // ";a"
//    pr.clear();
//    num = splitStr(";a", ';', pr, true);
//    EXPECT_EQ(num, 1);
//    EXPECT_TRUE(pr[0].first == 1 && pr[0].second == 2);
//
//    // "a;"
//    pr.clear();
//    num = splitStr("a;", ';', pr, true);
//    EXPECT_EQ(num, 1);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
//
//    // "a;;"
//    pr.clear();
//    num = splitStr("a;;", ';', pr, true);
//    EXPECT_EQ(num, 1);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
//
//    // ";;"
//    pr.clear();
//    num = splitStr(";;", ';', pr, true);
//    EXPECT_EQ(num, 0);
//
//    // ";"
//    pr.clear();
//    num = splitStr(";", ';', pr, true);
//    EXPECT_EQ(num, 0);
//
//    // ""
//    pr.clear();
//    num = splitStr("", ';', pr, true);
//    EXPECT_EQ(num, 0);
//
//    // ";;a"
//    pr.clear();
//    num = splitStr(";;a", ';', pr, true);
//    EXPECT_EQ(num, 1);
//    EXPECT_TRUE(pr[0].first == 2 && pr[0].second == 3);
//
//    // "a"
//    pr.clear();
//    num = splitStr("a", ';', pr, true);
//    EXPECT_EQ(num, 1);
//    EXPECT_TRUE(pr[0].first == 0 && pr[0].second == 1);
//
//}









//// Tests CSVLog class. Part 1
//class CSVLog_1_Test : public ::testing::Test {
//public:
//    // base log files directory: need to be set precisely according to individual user settings
//    // add / to the end of the dir name!
//    //static const char* LOGS_BASE_DIR;
//    
//    // test log file 1
//    static const char*  LOG_FILE1;
//    static const char*  LOG_FILE_RTS_04;            // log04.csv from reduce transition systems topic
//    static const size_t LOG_FILE_RTS_04_TRACESNUM;  // number of traces
//
//
//protected:
//    virtual void SetUp()
//    {
//        //SetUpGraph();
//    } // virtual void SetUp() 
//
//    virtual void TearDown()
//    {
//
//        //delete _gr1;
//    }
//
//
//    //// special setups
//    //void SetUpGraph()
//    //{
//    //    using namespace xi::ldopa;
//
//    //    _gr1_Mvs = new MapNamedVerticesStorage(nullptr);    // do not del manually!
//    //    _gr1 = new BidiGraph(_gr1_Mvs, nullptr);
//
//    //    _gr1_Vs = _gr1->getVertexStorage();
//    //    //_gr1_Lvs = dynamic_cast<xi::ldopa::ListVerticesStorage*>(_gr1_Vs);
//    //    _gr1_Es = _gr1->getEdgeStorage();
//    //    _gr1_Mes = dynamic_cast<xi::ldopa::OutInMMapEdgeStorage*>(_gr1_Es);
//
//
//    //    ////xi::ldopa::BidiGraphVertex* v1 
//    //    ////_gr1_v1 = _gr1.addVertex();
//    //    //_gr1_v1 = _gr1_Lvs->addNewVertex();
//
//
//
//    //    ////xi::ldopa::BidiGraphVertex* v2 
//    //    ////_gr1_v2 = _gr1.addVertex();
//    //    //_gr1_v2 = _gr1_Lvs->addNewVertex();
//    //}
//
//
//    //// BidiGraph objects
//    //xi::ldopa::BidiGraph* _gr1;
//
//    //xi::ldopa::IVerticesStorage* _gr1_Vs;
//    //xi::ldopa::MapNamedVerticesStorage* _gr1_Mvs;
//    //xi::ldopa::IEdgesStorage* _gr1_Es;
//    //xi::ldopa::OutInMMapEdgeStorage* _gr1_Mes;
//
//
//}; // class CSVLog_1_Test
//
//// ---<Set it here!>---
////const char* CSVLog_1_Test::LOGS_BASE_DIR = "";
//
//
//
//// ---<do not change followings>
//const char* CSVLog_1_Test::LOG_FILE1        = CSVLOG1_TEST_LOGS_BASE_DIR "logs/simple.csv";
//const char* CSVLog_1_Test::LOG_FILE_RTS_04  = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04.csv";
//const size_t CSVLog_1_Test::LOG_FILE_RTS_04_TRACESNUM = 12; // excluding 1 more for header
//
//
////===================================================================================
//
////------------- Tests itselves --------------
//
////
//TEST_F(CSVLog_1_Test, simplyOpenFile)
//{
//    using namespace xi::ldopa;
//    
//    CSVLog log1(CSVLog_1_Test::LOG_FILE1);
//    
//    EXPECT_EQ(log1.open(), true);
//    EXPECT_EQ(log1.isOpen(), true);
//
//    log1.close();
//    EXPECT_EQ(log1.isOpen(), false);
//    EXPECT_EQ(log1.open(), true);
//    EXPECT_EQ(log1.isOpen(), true);
//
//
//
//    //EXPECT_EQ(_gr1_Vs, _gr1_Mvs);
//    //EXPECT_EQ(_gr1_Mvs->getLinkedGraph(), _gr1);
//}
//
//TEST_F(CSVLog_1_Test, openFileThatNotExists)
//{
//    using namespace xi::ldopa;
//
//    CSVLog log1("that_file_should_not_exists.ext");
//    ASSERT_THROW(log1.open(), LdopaException);
//}
//
//
//TEST_F(CSVLog_1_Test, openWithExistingStream)
//{
//    using namespace xi::ldopa;
//
//    std::ifstream file(CSVLog_1_Test::LOG_FILE1);
//    CSVLog log1(&file);
//
//    EXPECT_EQ(log1.isOpen(), true);
//    EXPECT_EQ(log1.open(), true);
//
//}
//
//TEST_F(CSVLog_1_Test, defSettingsInit)
//{
//    using namespace xi::ldopa;
//
//    //CSVLog log1(CSVLog_1_Test::LOG_FILE1);       // TODO: need to use default constructor instead here!
//    CSVLog log1;
//    EXPECT_EQ(log1.isFirstLineAsHeader(), true);
//}
//
//
//TEST_F(CSVLog_1_Test, setGetFileName)
//{
//    using namespace xi::ldopa;
//
//    //CSVLog log1(CSVLog_1_Test::LOG_FILE1);       // TODO: need to use default constructor instead here!
//    CSVLog log1;
//    EXPECT_EQ(log1.getFileName(), std::string());
//
//    log1.setFileName(CSVLog_1_Test::LOG_FILE1);
//    EXPECT_EQ(log1.getFileName(), std::string(CSVLog_1_Test::LOG_FILE1));
//
//    // open with a newly set filename
//    EXPECT_EQ(log1.open(), true);
//
//    // try to change a filename while open (an exc should be)
//    ASSERT_THROW(log1.setFileName("newfilename.txt"), LdopaException);    
//
//    // try to change a filename while closed
//    log1.close();
//    log1.setFileName("newfilename1.txt");
//    EXPECT_EQ(log1.getFileName(), std::string("newfilename1.txt"));
//
//}
//
//
//TEST_F(CSVLog_1_Test, checkForOpenWithExc1)
//{
//    using namespace xi::ldopa;
//
//    CSVLog log1;
//    EXPECT_NE(log1.isOpen(), true);
//
//    // using tracer to simulate addressing a not accesible unless log not open method
//    ITraces* trcs = log1.getTraces();
//    EXPECT_NE(trcs, nullptr);
//
//    ASSERT_THROW(trcs->getNumOfTraces(), LdopaException);   
//}
//
//
//
//TEST_F(CSVLog_1_Test, numOfTraces1)
//{
//    using namespace xi::ldopa;
//
//    CSVLog log1(CSVLog_1_Test::LOG_FILE_RTS_04);
//    EXPECT_EQ(log1.open(), true);
//
//    // count a number of traces 
//    ITraces* trcs = log1.getTraces();
//    EXPECT_NE(trcs, nullptr);
//    EXPECT_EQ(trcs->getNumOfTraces(), CSVLog_1_Test::LOG_FILE_RTS_04_TRACESNUM); 
//    
//
//}