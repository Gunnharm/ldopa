////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Complex tests for testing Petri net synthesis using regions 
///            with TS reduction.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      19.08.2018
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

// ldopa
#include "xi/ldopa/eventlog/sqlite/sqlitelog.h"

#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"
#include "xi/ldopa/ts/algos/ts_simple_builder.h"
#include "xi/ldopa/ts/algos/ts_sas_converter.h"
#include "xi/ldopa/ts/algos/freq_condenser.h"
#include "xi/ldopa/ts/algos/varws_ts_rebuilder.h"
#include "xi/ldopa/ts/algos/ts_metrics_calc.h"

#include "xi/ldopa/ts/algos/grviz/evlog_ts_red_dotwriter.h"

#include "xi/ldopa/pn/models/base_ptnet.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"                    // модель PN
#include "xi/ldopa/pn/algos/regions/pn_synthesis.h"
#include "xi/ldopa/pn/algos/grviz/evlog_ptnets_dotwriter.h"     // записыватель в DOT для PN

#include "complex_settings.h"


// Constants
// ---<do not change followings>---
const char* LOG_FILE_RTS_04 = TEST_LOGS_DIR "log04.sq3";
const char* LOG_FILE_RTS_05 = TEST_LOGS_DIR "log05.sq3";
const char* LOG_FILE_GOODRU_R2 = TEST_LOGS_DIR "good.ru/good.ru_full_reduced2.sq3";

//------------------------------------------------------------------------------


TEST(CPnSynthesis1, simplest)
{
    EXPECT_TRUE(true);
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



// Performs building of a process-tree for the given log. Synthesizes a PN on the basis
// of the mined tree. Exports both TS and PN as Graph Viz files.
void twoStepMineAndGrViz(const char* logFile, const char* tsFile, const char* pnFile, 
    const char* logConfigure = "SELECT * FROM DefConfig")
{
    //using namespace xi::ldopa;
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    using namespace xi::ldopa::eventlog;

    typedef EvLogTSWithFreqs        TS;
    typedef TS::Attribute           Attribute;
    typedef TsSasConverter<TS>      EvLogTsSasConverter;
    typedef EventLogPetriNet<>      PN;
    //typedef PN::Attribute           Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel     SplitLabel;
    typedef std::string             String;


    // лог
    SQLiteLog log(logFile);     // log04.sq3
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry(logConfigure);
    log.open();


    // пул состояний
    AttrListStateIDsPool pool;
    // функция состояния
    PrefixStateFunc fnc(&log, &pool);
    // билдер
    TsBuilder bldr(&log, &fnc, &pool);

        //// отслеживатель прогресса
        //BuilderProgressCB1 progrCb;
        //bldr.setProgressCB(&progrCb);

    TS* ts = bldr.build(true);                  // true -- трекать активности по ходу дела

    // переводим принимающие состояния отдельным конвертером
    Attribute wfAccepted;                       // single exit attribute
    EvLogTsSasConverter sasConv(wfAccepted);
    sasConv.convert(ts);


    // записыватель TS
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(tsFile, *ts);


    //=====[ синтез PN ]=====    
    PNSynth synth(ts, PNSynth::Options::F_MAKE_WFNET);
    synth.setWFTransLabel(wfAccepted);
    synth.synthesize();

    PN* pn = synth.getPn();

    // записыватель PN
    EventLogPetriNetDotWriter pndw;
    pndw.writePn(pnFile,
        *pn, synth.getInitMarking());
}


//------------------------------------------------------------------------------


enum SelfLoopPolicy
{
    slNoSLoops,             // без петель (игнорировать)
    slReestablish,          // восстанавливать после синтеза
    // slTSConvert          // преобразовать исходную TS, чтобы не было петель
    slProcess               // обрабатывать петли модифицированный алгоритмом
};

//------------------------------------------------------------------------------


// строит префиксное дерево, конденсирует и восстанавливает
// doMetrics3 - если true, считает метрики для 3TS
// синтезирует PN для редуцированной
void buildReduceSynth(const char* fn, double threshold, double vwsc, bool doMetrics3,
    bool doSynth,
    SelfLoopPolicy slPolic,
    const char* tsFile3, const char* pnFile3,
    const char* logConfigure = "SELECT * FROM DefConfig")
{
    using namespace xi::ldopa::ts;
    using namespace xi::ldopa::pn;
    using namespace xi::ldopa::eventlog;


    typedef EvLogTSWithFreqs        TS;
    typedef TS::Attribute           Attribute;
    typedef TsSasConverter<TS>      EvLogTsSasConverter;
    typedef EventLogPetriNet<>      PN;
    //typedef PN::Attribute           Attribute;
    typedef PnRegSynthesizer<TS, PN> PNSynth;
    typedef PNSynth::SplitLabel     SplitLabel;
    typedef std::string             String;



    // лог
    SQLiteLog log(fn);
    log.setAutoLoadConfig(true);                        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry(logConfigure);
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

    //EXPECT_EQ(16, ts->getStatesNum());
    //EXPECT_EQ(15, ts->getTransitionsNum());
    //EXPECT_EQ(6, ts->getMaxWS());                       // макс. размер окна


    //----[Шаг 2: жирная (редуцируем)]----
    CondensedTsBuilder reducer(ts);

    start = clock();
    TS* rts = reducer.build(threshold);                 // оставляем с частотностью threshold % от числа трасс
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    EXPECT_TRUE(false) << "2TS Building complete. Time (ms): " << elapsed;

    // редуцированная и исходная — д.б. разные сп!
    EXPECT_TRUE(rts != ts);

    //// после редукции 33 % (мин. сохраненная частота 3) остается 6 вершин и 5 дуг
    //EXPECT_EQ(6, rts->getStatesNum());
    //EXPECT_EQ(5, rts->getTransitionsNum());
    //EXPECT_EQ(6, rts->getMaxWS());                      // макс. размер окна


    //----[Шаг 3: восстановление]----
    VarWsTsBuilder rebuilder(&log, rts, &fnc);
    start = clock();
    TS* fts = rebuilder.build(vwsc, VarWsTsBuilder::zsaSpecState);
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    EXPECT_TRUE(false) << "3TS Building complete. Time (ms): " << elapsed;


    //// после восстановления с VWSC = 1 будет 12 вершин и 13 дуг
    //EXPECT_EQ(12, fts->getStatesNum());
    //EXPECT_EQ(13, fts->getTransitionsNum());
    //EXPECT_EQ(6, fts->getMaxWS());                      // макс. размер окна



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



    // переводим принимающие состояния отдельным конвертером
    Attribute wfAccepted;                       // single exit attribute
    EvLogTsSasConverter sasConv(wfAccepted);
    sasConv.convert(fts);


    // записыватель TS3
    EvLogTSWithFreqsDotWriter tsdw;
    tsdw.write(tsFile3, *fts);

    //=====[ синтез PN ]=====    
    if (doSynth)
    {
        start = clock();
        PNSynth synth(fts, PNSynth::Options::F_MAKE_WFNET);
        synth.setWFTransLabel(wfAccepted);

        if (slPolic == slReestablish)           // если петли надо синтезатором восстанавливать
            synth.options().setSelfLoopPolicy(PNSynth::Options::slReestablish);
        else if (slPolic == slProcess)
            synth.options().setSelfLoopPolicy(PNSynth::Options::slProcess);
        else
            synth.options().setSelfLoopPolicy(PNSynth::Options::slIgnore);


        synth.synthesize();

        PN* pn = synth.getPn();

        stop = clock();
        elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        EXPECT_TRUE(false) << "PN synthesized. Time (ms): " << elapsed;


        // записыватель PN
        EventLogPetriNetDotWriter pndw;
        pndw.writePn(pnFile3,
            *pn, synth.getInitMarking());
    }
}



TEST(CPnSynthesis1, Goodru2_redSynthNoSLoops1)
{
    instantiateNeedfulAttributeMembers();


    buildReduceSynth(LOG_FILE_GOODRU_R2, 0.33,

        //>>> VWSC
        //0.1,               // 18736 - успешно
        //0.05,               // 1375 ms - успешно
        //0.15,
        //0.25,             // обн. — 46 сек успешно   ---ок. 20 мин — прервано
        0.5,                // ок — 560 сек (~10 мин)
        //<<< VWSC

        false,      // метрики не считаем
        true,       // синтезируем
        slNoSLoops, // без петель
        GRMODELS_OUTP_DIR "CPnSynthesis1/goodru2_red1-ts.gv",   // выход TS
        GRMODELS_OUTP_DIR "CPnSynthesis1/goodru2_red1-pn.gv");  // выход PN

}


//------------------------------------------------------------------------------


//  восстанавливаем петли синтезатором
TEST(CPnSynthesis1, Goodru2_redSynthSlReest1)
{
    instantiateNeedfulAttributeMembers();


    buildReduceSynth(LOG_FILE_GOODRU_R2, 0.33,

        //>>> VWSC
        //0.1,               // 
        //0.05,               // 
        //0.15,
        0.25,             // 52 сек
        //0.5,                // ок — 560 сек (~10 мин)
        //<<< VWSC

        false,              // метрики не считаем
        true,               // синтезируем
        slReestablish,      // восстанавливаем петли синтезатором
        GRMODELS_OUTP_DIR "CPnSynthesis1/goodru2_red1_Slr-ts.gv",   // выход TS
        GRMODELS_OUTP_DIR "CPnSynthesis1/goodru2_red1_Slr-pn.gv");  // выход PN

}



//------------------------------------------------------------------------------


//  восстанавливаем петли синтезатором
TEST(CPnSynthesis1, Goodru2_redSynthSlReest1_ProcessSLoops)
{
    instantiateNeedfulAttributeMembers();


    buildReduceSynth(LOG_FILE_GOODRU_R2, 0.33,

        //>>> VWSC
        //0.1,               // 
        //0.05,               // 
        //0.15,
        0.25,             // 52 сек
        //0.5,                // ок — 560 сек (~10 мин)
        //<<< VWSC

        false,              // метрики не считаем
        true,               // синтезируем
        slProcess,      // восстанавливаем петли синтезатором
        GRMODELS_OUTP_DIR "CPnSynthesis1/goodru2_red1_Slr-ts.gv",   // выход TS
        GRMODELS_OUTP_DIR "CPnSynthesis1/goodru2_red1_Slr-pn.gv");  // выход PN

}
