///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing EventogTs module
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "xi/ldopa/ts/algos/ts_simple_builder.h"
#include "xi/ldopa/eventlog/sqlite/sqlitelog.h"
#include "xi/ldopa/ts/algos/freq_condenser.h"
#include "xi/ldopa/ts/algos/varws_ts_rebuilder.h"
#include "xi/ldopa/ts/algos/ts_metrics_calc.h"

#include "xi/ldopa/ts/algos/grviz/evlog_ts_red_dotwriter.h" // DOT writter
#include "xi/ldopa/ts/algos/ts_sas_converter.h"

// test settings file
#include "eventlog/csvlog_test_settings.h"

//==============================================================================
// Testbed class.
//==============================================================================

/** \brief Test class. */
class TsSimpleBuilder_1_Test : public ::testing::Test {
public:
    //-----<Types>-----
    //typedef std::shared_ptr<xi::ldopa::ITrace::IEventEnumerator> IEventEnumShPtr;
public:
    // base log files directory: need to be set precisely according to individual user settings
    // add / to the end of the dir name!
    //static const char* LOGS_BASE_DIR;

    // test log file 4
    static const char*  LOG_FILE_RTS_04;                // log04.sq3 from reduce transition systems topic
    static const char*  LOG_FILE_RTS_04_1;              // log04-1.sq3 - 04 w/o <b d c e> trace
    static const char*  LOG_FILE_RTS_05;                // log05.sq3 from reduce transition systems topic
    static const char*  LOG_FILE_RUBIN_1;               // Rubin's log w/o the case 622.
protected:
    virtual void SetUp()
    {
    } 

    virtual void TearDown()
    {
    }
}; // class TsSimpleBuilder_1_Test

// ---<do not change followings>---
const char* TsSimpleBuilder_1_Test::LOG_FILE_RTS_04 = 
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04.sq3";

const char* TsSimpleBuilder_1_Test::LOG_FILE_RTS_04_1 =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04-1.sq3";

const char* TsSimpleBuilder_1_Test::LOG_FILE_RTS_05 =
    CSVLOG1_TEST_LOGS_BASE_DIR "logs/log05.sq3";

const char* TsSimpleBuilder_1_Test::LOG_FILE_RUBIN_1 =
CSVLOG1_TEST_LOGS_BASE_DIR "logs/rubin-all-shorts.sq3";


//==============================================================================
// class BuilderProgressCB1
//==============================================================================

/** \brief Auxiliary class processing progress callbacks from algos. */
class BuilderProgressCB1 : public xi::ldopa::IProgressCallback
{
public:
    BuilderProgressCB1() : _lastProgr(0) {  }
public:
    virtual Res progress(Byte progress) 
    {
        _lastProgr = progress;
        return IProgressCallback::ResOK;
    }

protected:
    xi::ldopa::IProgressCallback::Byte _lastProgr;  ///< Last progress.
}; // class BuilderProgressCB1


//==============================================================================
// class EventLogTs
//==============================================================================

TEST_F(TsSimpleBuilder_1_Test, settings1)
{
    using namespace xi::ldopa::ts;

    TsBuilder b(nullptr, nullptr, nullptr);              // это надо под контролем держать
    b.setAutoClearStateIDsPool(false);
    EXPECT_FALSE(b.isAutoClearStateIDsPool());
    b.setAutoClearStateIDsPool(true);
    EXPECT_TRUE(b.isAutoClearStateIDsPool());
}


//------------------------------------------------------------------------------

void instantiateNeedfulAttributeMembers()
{
    // обращаемся ко всем нужным членам шаблона, чтобы можно в отладчике смотреть
    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;
    using xi::attributes::StringSharedPtr;
    using eventlog::SQLiteLog;
    using eventlog::IEventLog;

    IEventLog::Attribute a = StringSharedPtr(new std::string("Abc"));
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

//------------------------------------------------------------------------------

// собираем вместе связанные компоненты для построения
TEST_F(TsSimpleBuilder_1_Test, aSimpleMineSettings1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;
    using eventlog::SQLiteLog;

    // лог
    SQLiteLog log(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    //log.getAutoLoadConfigQry() = "SELECT * FROM DefConfig";
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // пул идентификаторов состояний
    AttrListStateIDsPool pool;
    
    // функция состояния
    PrefixStateFunc fnc(&log, &pool);
    
    // билдер
    TsBuilder bldr(&log, &fnc, &pool);
}

//------------------------------------------------------------------------------

// построение простого лога
TEST_F(TsSimpleBuilder_1_Test, buildSimpleLog1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Tsa;
    using eventlog::SQLiteLog;

    // лог
    SQLiteLog log(LOG_FILE_RTS_04);     // log04.sq3
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    //log.getAutoLoadConfigQry() = "SELECT * FROM DefConfig";
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    // отслеживатель прогресса
    BuilderProgressCB1 progrCb;
    bldr.setProgressCB(&progrCb);

    TS* ts = bldr.build(true);                  // true -- трекать активности по ходу дела
    EXPECT_EQ(11, ts->getStatesNum());
    EXPECT_EQ(10, ts->getTransitionsNum());

    // проверяем атрибуты
    EXPECT_EQ(4, ts->getMaxWS());

    TS::State st_init = ts->getInitState();
    TS::Attribute aa = Tsa::ss("a"); //TS::Attribute::ss("a");
    TS::StateRes rst_a = ts->getState(pool[{ aa }]); //().first;
    EXPECT_TRUE(rst_a.second);
    TS::TransRes rt_a = ts->getTrans(st_init, rst_a.first, aa); // ().first;
    EXPECT_TRUE(rt_a.second);

    // частота
    TS::IntRes rt_a_f = ts->getTransFreq(rt_a.first);
    EXPECT_TRUE(rt_a_f.second);
    EXPECT_EQ(2, rt_a_f.first);

    // принимающее состояние
    TS::StateRes rst_abc = ts->getState(pool[{ Tsa::ss("a"), Tsa::ss("b"), Tsa::ss("c") }]);
    EXPECT_TRUE(rst_abc.second);
    EXPECT_TRUE(ts->isStateAccepting(rst_abc.first));

    // обычное состояние
    TS::StateRes rst_ab = ts->getState(pool[{ Tsa::ss("a"), Tsa::ss("b") }]);
    EXPECT_TRUE(rst_ab.second);
    EXPECT_FALSE(ts->isStateAccepting(rst_ab.first));
}

//------------------------------------------------------------------------------

// построение простого лога и последующая редукция
TEST_F(TsSimpleBuilder_1_Test, buildAndReduceSimpleLog1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    using eventlog::SQLiteLog;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Tsa;

    // лог
    SQLiteLog log(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    //log.getAutoLoadConfigQry() = "SELECT * FROM DefConfig";
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    TS* ts = bldr.build();
    EXPECT_EQ(11, ts->getStatesNum());
    EXPECT_EQ(10, ts->getTransitionsNum());

    // в логе:
    //      <a, b, c>
    //      <a, b, d>
    //      <b, c, d>
    //      <b, d, c, e>
    // то есть частота на дугах НЕ более 2

    // редуцируем
    CondensedTsBuilder reducer(ts);
    TS* rts = reducer.build(0.4);                         // оставляем с частотностью 40 % от числа трасс


    // редуцированная и исходная — д.б. разные сп!
    EXPECT_TRUE(rts != ts);

    // после редукции 40 % (мин. сохраненная частота 2) остается 4 вершины и 3 дуг
    EXPECT_EQ(4, rts->getStatesNum());
    EXPECT_EQ(3, rts->getTransitionsNum());
}

//------------------------------------------------------------------------------

// построение простого лога, последующая редукция и восстановление
TEST_F(TsSimpleBuilder_1_Test, reduceRebuiltSimpleLog1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    using eventlog::SQLiteLog;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Tsa;

    // лог
    SQLiteLog log(LOG_FILE_RTS_05);
    log.setAutoLoadConfig(true);                        // явно указываем, что надо загружать конфиг и SQL для него
    //log.getAutoLoadConfigQry() = "SELECT * FROM DefConfig";
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    EXPECT_EQ(8, log.getTracesNum());                   // число трасс == размер лога
    EXPECT_EQ(41, log.getEventsNum());                  // общее число событий во всех трассах

    //----[Шаг 1: полная]----

    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    TS* ts = bldr.build(true);                          // мн-во состояний пусть считает
    EXPECT_EQ(16, ts->getStatesNum());
    EXPECT_EQ(15, ts->getTransitionsNum());
    EXPECT_EQ(6, ts->getMaxWS());                       // макс. размер окна
    // #appnote: здесь вообще то можно еще поднакопить множество различимых активностей


    //----[Шаг 2: жирная (редуцируем)]----
    CondensedTsBuilder reducer(ts);
    TS* rts = reducer.build(0.33);                      // оставляем с частотностью 33 % от числа трасс

    // редуцированная и исходная — д.б. разные сп!
    EXPECT_TRUE(rts != ts);

    // после редукции 33 % (мин. сохраненная частота 3) остается 6 вершин и 5 дуг
    EXPECT_EQ(6, rts->getStatesNum());
    EXPECT_EQ(5, rts->getTransitionsNum());
    EXPECT_EQ(6, rts->getMaxWS());                      // макс. размер окна


    //----[Шаг 3: восстановление]----
    VarWsTsBuilder rebuilder(&log, rts, &fnc);
    TS* fts = rebuilder.build(1, VarWsTsBuilder::zsaSpecState);

    // после восстановления с VWSC = 1 будет 12 вершин и 13 дуг
    EXPECT_EQ(12, fts->getStatesNum());
    EXPECT_EQ(13, fts->getTransitionsNum());
    EXPECT_EQ(6, fts->getMaxWS());                      // макс. размер окна


    //====={Метрики считаем}=====
    //TsMetricsCalc mCalc(&log, bldr.getLogActivities(), ts);
    TsMetricsCalc mCalc(&log, ts);
    double ts1_simpl = mCalc.calcSimplicity(ts);        // (7 + 1) / (16 + 15)
    EXPECT_EQ(258, (int)(ts1_simpl * 1000));            // ~0.258

    // точность 
    double ts1_prec = mCalc.calcPrecision(ts);          // для полной д.б. 1
    double ts3_prec = mCalc.calcPrecision(fts);         // для редуцированной...

    // генерализация пусть уже
    double ts1_genr = mCalc.calcGeneralization(ts);  
    double ts3_genr = mCalc.calcGeneralization(fts); 
}

//------------------------------------------------------------------------------

// расчет точности для TS из 5-го лога, построенного с 1-WS
TEST_F(TsSimpleBuilder_1_Test, calcPrecision1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    using eventlog::SQLiteLog;
    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Tsa;

    // лог
    SQLiteLog log(LOG_FILE_RTS_04_1);
    log.setAutoLoadConfig(true);                        // явно указываем, что надо загружать конфиг и SQL для него
    //log.getAutoLoadConfigQry() = "SELECT * FROM DefConfig";
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    EXPECT_EQ(4, log.getTracesNum());                   // число трасс == размер лога
    EXPECT_EQ(12, log.getEventsNum());                  // общее число событий во всех трассах

    //----[полная TS (префиксное дерево)]----
    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    //TS* ts = bldr.build(false);                         // мн-во состояний не нужно
    bldr.build(false);                                  // мн-во состояний НУЖНО, но посчитаем позже
    TS* ts = bldr.detach();
    EXPECT_EQ(10, ts->getStatesNum());
    EXPECT_EQ(9, ts->getTransitionsNum());
    EXPECT_EQ(3, ts->getMaxWS());                       // макс. размер окна


    //----[1-WS система переходов]----
    fnc.setWS(1);                                       // фиксированное 1-окно
    TS* ts1 = bldr.build(true);                         // множество состояний тут уже посчитаем!
    EXPECT_EQ(1, ts1->getMaxWS());                      // макс. размер окна очевиден
    EXPECT_EQ(5, ts1->getStatesNum());
    EXPECT_EQ(7, ts1->getTransitionsNum());             // а вот тут переходов побольше!
    

    //====={Метрики считаем}=====
    TsMetricsCalc mCalc(&log, ts); 
    //TsMetricsCalc mCalc(&log, bldr.getLogActivities(), ts);
    double ts1_simpl = mCalc.calcSimplicity(ts1);       // (4 + 1) / (5 + 7)
    EXPECT_EQ(416, (int)(ts1_simpl * 1000));            // ~0.416

    // точность 
    double ts_prec = mCalc.calcPrecision(ts);           // для полной д.б. 1
    double ts1_prec = mCalc.calcPrecision(ts1);         // для редуцированной...
    EXPECT_EQ(8, (int)(ts1_prec * 10));                 // 0.8 д.б.

    // генерализация пусть уже
    double ts_genr = mCalc.calcGeneralization(ts);
    double ts1_genr = mCalc.calcGeneralization(ts1);

    delete ts;                                          // мы ее отдетачили!
}

//------------------------------------------------------------------------------

// построение простого лога
TEST_F(TsSimpleBuilder_1_Test, buildSimpleTsFromLog4_Vis1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Attribute;
    using eventlog::SQLiteLog;

    // лог
    SQLiteLog log(LOG_FILE_RTS_04);     // log04.sq3
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    // отслеживатель прогресса
    BuilderProgressCB1 progrCb;
    bldr.setProgressCB(&progrCb);

    TS* ts = bldr.build(true);                  // true -- трекать активности по ходу дела
    EXPECT_EQ(11, ts->getStatesNum());
    EXPECT_EQ(10, ts->getTransitionsNum());

    // проверяем атрибуты
    EXPECT_EQ(4, ts->getMaxWS());

    TS::State st_init = ts->getInitState();
    TS::Attribute aa = Attribute::ss("a"); //TS::Attribute::ss("a");
    TS::StateRes rst_a = ts->getState(pool[{ aa }]); //().first;
    EXPECT_TRUE(rst_a.second);
    TS::TransRes rt_a = ts->getTrans(st_init, rst_a.first, aa); // ().first;
    EXPECT_TRUE(rt_a.second);

    // частота
    TS::IntRes rt_a_f = ts->getTransFreq(rt_a.first);
    EXPECT_TRUE(rt_a_f.second);
    EXPECT_EQ(2, rt_a_f.first);

    // принимающее состояние
    TS::StateRes rst_abc = ts->getState(pool[{ Attribute::ss("a"), Attribute::ss("b"), Attribute::ss("c") }]);
    EXPECT_TRUE(rst_abc.second);
    EXPECT_TRUE(ts->isStateAccepting(rst_abc.first));

    // обычное состояние
    TS::StateRes rst_ab = ts->getState(pool[{ Attribute::ss("a"), Attribute::ss("b") }]);
    EXPECT_TRUE(rst_ab.second);
    EXPECT_FALSE(ts->isStateAccepting(rst_ab.first));

    // UPD: 01/02/18: множество активностей билдером больше не подсчитываем,
    // а берем непосредственно из лога (коль уж надо)
    EXPECT_EQ(5, log.getActivitiesNum());       // |{ a, b, c, d, e }|

    // записыватель
    EvLogTSWithFreqsDotWriter dw;
    dw.write(TS_TEST_MODELS_BASE_DIR "ts/TsSimpleBuilder_1_Test-buildSimpleTsFromLog4_Vis1.gv", *ts);
}

//------------------------------------------------------------------------------

// построение простого лога
TEST_F(TsSimpleBuilder_1_Test, buildSimpleTsFromLog4_SingleAcptState1)
{
    instantiateNeedfulAttributeMembers();

    using namespace xi::ldopa;
    using namespace xi::ldopa::ts;

    typedef EvLogTSWithFreqs TS;
    typedef TS::Attribute Attribute;
    using eventlog::SQLiteLog;
    typedef TsSasConverter<TS> EvLogTsSasConverter;

    // лог
    SQLiteLog log(LOG_FILE_RTS_04);     // log04.sq3
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // пул состояний
    AttrListStateIDsPool pool;

    // функция состояния
    PrefixStateFunc fnc(&log, &pool);

    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

    // отслеживатель прогресса
    BuilderProgressCB1 progrCb;
    bldr.setProgressCB(&progrCb);

    TS* ts = bldr.build(true);                  // true -- трекать активности по ходу дела
    EXPECT_EQ(11, ts->getStatesNum());
    EXPECT_EQ(10, ts->getTransitionsNum());

    // проверяем атрибуты
    EXPECT_EQ(4, ts->getMaxWS());

    TS::State st_init = ts->getInitState();
    TS::Attribute aa = Attribute::ss("a"); //TS::Attribute::ss("a");
    TS::StateRes rst_a = ts->getState(pool[{ aa }]); //().first;
    EXPECT_TRUE(rst_a.second);
    TS::TransRes rt_a = ts->getTrans(st_init, rst_a.first, aa); // ().first;
    EXPECT_TRUE(rt_a.second);


    // частота
    TS::IntRes rt_a_f = ts->getTransFreq(rt_a.first);
    EXPECT_TRUE(rt_a_f.second);
    EXPECT_EQ(2, rt_a_f.first);

    EXPECT_EQ(5, log.getActivitiesNum());       // |{ a, b, c, d, e }|

    // переводим принимающие состояния отдельным конвертером
    Attribute wfAccepted;                       // single exit attribute
    EXPECT_TRUE(wfAccepted.getType() == Attribute::AType::tBlank);
    EvLogTsSasConverter sasConv(wfAccepted);
    EXPECT_EQ(4, sasConv.convert(ts));
    EXPECT_EQ(12, ts->getStatesNum());
    EXPECT_EQ(14, ts->getTransitionsNum());

    // записыватель
    EvLogTSWithFreqsDotWriter dw;
    dw.write(TS_TEST_MODELS_BASE_DIR "ts/TsSimpleBuilder_1_Test-buildSimpleTsFromLog4_SingleAcptState1.gv", *ts);
}
