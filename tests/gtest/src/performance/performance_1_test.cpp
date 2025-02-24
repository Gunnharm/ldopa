///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Perfromance tests.
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <time.h>

#include "xi/ldopa/ts/algos/ts_simple_builder.h"
#include "xi/ldopa/eventlog/sqlite/sqlitelog.h"
#include "xi/ldopa/ts/algos/freq_condenser.h"
#include "xi/ldopa/ts/algos/varws_ts_rebuilder.h"
#include "xi/ldopa/ts/algos/ts_metrics_calc.h"

// test settings file
#include "performance_test_settings.h"

//==============================================================================
// Testbed class.
//==============================================================================

/** \brief Test class. */
class Performance_1_Test : public ::testing::Test {
public:
    // base log files directory: need to be set precisely according to individual user settings
    // add / to the end of the dir name!

    // test log file 4
    static const char*  LOG_FILE_RTS_04;                // log04.sq3 from reduce transition systems topic
    static const char*  LOG_FILE_RTS_04_1;              // log04-1.sq3 - 04 w/o <b d c e> trace
    static const char*  LOG_FILE_RTS_05;                // log05.sq3 from reduce transition systems topic
    static const char*  LOG_FILE_BPIC_15_1;             // BPI Chall. 1132 трассы
    static const char*  LOG_FILE_GOODRU_R2;             // good.ru_full_reduced2
    static const char*  LOG_FILE_BPIC17_FULL;           // BPIC-17-full
    static const char*  LOG_FILE_BPIC18_FULL;           // BPIC-18-full
    static const char*  LOG_FILE_CREREQ;                // Credit Requirements    
    static const char*  LOG_FILE_COSELOG1;              // CoSeLog1
protected:
    virtual void SetUp()
    {
    } 

    virtual void TearDown()
    {
    }
}; // class Performance_1_Test

const char* Performance_1_Test::LOG_FILE_BPIC_15_1 =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/bpic15-1132/bpic15-1132.sq3";
const char* Performance_1_Test::LOG_FILE_GOODRU_R2 =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/good.ru/good.ru_full_reduced2.sq3";
const char* Performance_1_Test::LOG_FILE_BPIC17_FULL =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/bpic17/bpic17-full.sq3";
const char* Performance_1_Test::LOG_FILE_BPIC18_FULL =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/bpic18/bpic18-full.sq3";
const char* Performance_1_Test::LOG_FILE_CREREQ =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/crereq/crereq.sq3";
const char* Performance_1_Test::LOG_FILE_COSELOG1 =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/coselog/coselog1.sq3";

//==============================================================================
// class EventLogTs
//==============================================================================

void instantiateNeedfulAttributeMembers()
{
    // обращаемся ко всем нужным членам шаблона, чтобы можно в отладчике смотреть
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;
    using xi::attributes::StringSharedPtr;

    eventlog::IEventLog::Attribute a = StringSharedPtr(new std::string("Abc"));
    a.getType();
    a.getRef();
    a.getPtr();
    a.toString();
    a.asStr();
    a.asStrP();
    
    // и заодно уже для пула ид.
    AttrListStateIDsPool pool;
    pool.getSize();
}


// строит префиксное дерево и проверяет, совпало ли число вершин/дуг
void buildPrefixTree(const char* fn, int estStatesNum, const char* conf = "SELECT * FROM DefConfig")
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;
    using eventlog::SQLiteLog;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Tsa;

    {
        // лог
        SQLiteLog* dlog = new SQLiteLog(fn);    // 1132 трассы, 18 МБ    
        SQLiteLog& log = *dlog;
        log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
        log.setAutoLoadConfigQry(conf);
        log.open();

        // пул состояний
        AttrListStateIDsPool pool;

        // функция состояния
        PrefixStateFunc fnc(&log, &pool);

        // билдер
        TsBuilder bldr(&log, &fnc, &pool);

        // время меряем чисто на построение
        clock_t start = clock();
        TS* ts = bldr.build(true);                  // true -- трекать активности по ходу дела
        clock_t stop = clock();
        double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        EXPECT_TRUE(false) << "Building of a prefix-tree complete. Time (ms): " << elapsed; 
        // ^ https://stackoverflow.com/questions/16491675/how-to-send-custom-message-in-google-c-testing-framework

        // отдельно убираем лог
        clock_t start2 = clock();
        // убьем лог отдельно от билдера, чтобы посмотреть в профайлере, как память освободится    
        delete dlog;

        clock_t stop2 = clock();
        double elapsed2 = (double)(stop2 - start2) * 1000.0 / CLOCKS_PER_SEC;
        EXPECT_TRUE(false) << "Log clearing complete. Time (ms): " << elapsed2;
        EXPECT_EQ(estStatesNum, ts->getStatesNum());          // для 10 трасс
        EXPECT_EQ(estStatesNum - 1, ts->getTransitionsNum());

        // отдельно убираем СП
        ts = bldr.detach();
        clock_t start3 = clock();
        delete ts;
        clock_t stop3 = clock();
        double elapsed3 = (double)(stop3 - start3) * 1000.0 / CLOCKS_PER_SEC;
        EXPECT_TRUE(false) << "TS clearing complete. Time (ms): " << elapsed3;
    }
}


// перебирает все вершины TS и получает текстовые метки — для отработки проблемы с
// 0WS состоянием
void iterateTsVertices(xi::ldopa::ts::EvLogTSWithFreqs* ts)
{
    using namespace xi::ldopa::ts;
    EvLogTSWithFreqs::StateIter cur, end;   //Pair sts = ts->getStates();
    for (boost::tie(cur, end) = ts->getStates(); cur != end; ++cur)
    {
        EvLogTSWithFreqs::State& s = *cur;
        if(!ts->isStateAnon(s))
            std::string sid = ts->makeStateIDStr(s);
    }
}


// строит префиксное дерево, конденсирует и восстанавливает
// doMetrics3 - если true, считает метрики для 3TS
void buildAndReduceTs(const char* fn, double threshold, double vwsc, bool doMetrics3 = false)
{
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    using eventlog::SQLiteLog;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Tsa;

    // лог
    SQLiteLog log(fn);
    log.setAutoLoadConfig(true);                        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    //----[Шаг 1: полная]----

    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    clock_t start = clock();
    TS* ts = bldr.build(true);                          // мн-во состояний пусть считает
    clock_t stop = clock();
    double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    EXPECT_TRUE(false) << "1TS Building complete. Time (ms): " << elapsed;

    //----[Шаг 2: жирная (редуцируем)]----
    CondensedTsBuilder reducer(ts);

    start = clock();
    TS* rts = reducer.build(threshold);                 // оставляем с частотностью threshold % от числа трасс
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    EXPECT_TRUE(false) << "2TS Building complete. Time (ms): " << elapsed;

    // редуцированная и исходная — д.б. разные сп!
    EXPECT_TRUE(rts != ts);

    //----[Шаг 3: восстановление]----
    VarWsTsBuilder rebuilder(&log, rts, &fnc);
    start = clock();
    TS* fts = rebuilder.build(vwsc, VarWsTsBuilder::zsaSpecState);
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    EXPECT_TRUE(false) << "3TS Building complete. Time (ms): " << elapsed;

    // проверяем возможность взятия всех идентификаторов
    iterateTsVertices(fts);
    
    //====={Метрики считаем}=====
    
    // Для 3TS
    if (doMetrics3)
    {
        start = clock();
        TsMetricsCalc mCalc(&log, ts);              // не забудь: 2-м параметром идет 1TS для симуляции!
        double simpl = mCalc.calcSimplicity(fts);
        double prec = mCalc.calcPrecision(fts);
        double genr = mCalc.calcGeneralization(fts);
        stop = clock();
        elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        EXPECT_TRUE(false) << "Metrics of 3TS calculated. Time (ms): " << elapsed;
    }
}

// построение префиксного дерева для лога goodru_full_reduced2
TEST_F(Performance_1_Test, buildPrefix_goodru_full_reduced2)
{
    instantiateNeedfulAttributeMembers();
    buildPrefixTree(LOG_FILE_GOODRU_R2, 1, "select * from config where persp = 0");
}

// построение префиксного дерева для лога BPIC-17-full
TEST_F(Performance_1_Test, buildPrefix_bpic17_full)
{
    instantiateNeedfulAttributeMembers();
    buildPrefixTree(LOG_FILE_BPIC17_FULL, 1, "SELECT * FROM config WHERE persp = 0");
}


// построение префиксного дерева для лога BPIC-17-full
TEST_F(Performance_1_Test, buildPrefix_crereq)
{
    instantiateNeedfulAttributeMembers();
    buildPrefixTree(LOG_FILE_CREREQ, 1, "SELECT * FROM config WHERE persp = 0");
}


// построение префиксного дерева для лога BPIC-17-full
TEST_F(Performance_1_Test, buildPrefix_coselog1)
{
    instantiateNeedfulAttributeMembers();
    buildPrefixTree(LOG_FILE_COSELOG1, 1, "SELECT * FROM config WHERE persp = 0");
}

