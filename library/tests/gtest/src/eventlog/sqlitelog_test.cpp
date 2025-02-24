////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Tests for SQLite Log component.
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      23.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// HOW TO SET A TEST ENVIRONMENT:
/// one need to define CSVLOG1_TEST_LOGS_BASE_DIR directive to set it to a
/// machine/user specific value
///
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/eventlog/sqlite/sqlitelog.h"

// test settings file
#include "eventlog/csvlog_test_settings.h"

// std
#include <map>
#include <memory>

// xilib
#include "xi/types/aliases.h"


using namespace xi::ldopa;
using namespace xi::ldopa::eventlog;

// Tests SQLiteLog class. Part 1
class SQLiteLog_1_Test : public ::testing::Test {
public:
    // base log files directory: need to be set precisely according to individual user settings
    // add / to the end of the dir name!
    // static const char* LOGS_BASE_DIR;
    
    // test log file 1
    static const char*  LOG_FILE_RTS_04;                // log04.sq3 from reduce transition systems topic
public:
    // helpers
    static bool isStrNullOrEmpty(const std::string* str)
    {
        return /*xi::ldopa::*/SQLiteLog::isStrNullOrEmpty(str);
    }

protected:
    virtual void SetUp()
    {
        //SetUpGraph();
    } // virtual void SetUp() 

    virtual void TearDown()
    {
        //delete _gr1;
    }

}; // class SQLiteLog_1_Test

// ---<do not change followings>
const char* SQLiteLog_1_Test::LOG_FILE_RTS_04 = CSVLOG1_TEST_LOGS_BASE_DIR "logs/log04.sq3";

//===================================================================================

void instantiateNeedfulAttributeMembers()
{
    // обращаемся ко всем нужным членам шаблона, чтобы можно в отладчике смотреть
    using namespace xi::ldopa;
    //using namespace xi::ldopa::ts;
    using xi::attributes::StringSharedPtr;
    //using eventlog::SQLiteLog;
    using eventlog::IEventLog;

    IEventLog::Attribute a = StringSharedPtr(new std::string("Abc"));
    a.getType();
    a.getRef();
    a.getPtr();
    a.toString();
    a.asStr();
    a.asStrP();
}

//------------- Tests  --------------

TEST_F(SQLiteLog_1_Test, simplest)
{
    EXPECT_EQ(1, 1);
}


TEST_F(SQLiteLog_1_Test, simplyOpenFile)
{
    // скобки добавляем, чтобы увидеть, что деструктор вызывается
    {
        SQLiteLog log;
        log.setFileName(LOG_FILE_RTS_04);
        log.open();
        EXPECT_EQ(true, log.isOpen());          // д.б. открыт

        // добавочка от 16.11.17: инфо-строка
        std::string info = log.getInfoStr();

    }
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, bitsetSettings1)
{
    SQLiteLog log;

    // SET_AUTO_LOAD_SETS

    EXPECT_EQ(true, log.isAutoLoadConfig());
    
    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());

    log.setAutoLoadConfig(false);
    EXPECT_EQ(false, log.isAutoLoadConfig());

    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());

    // event log "vertical" attribute extracting for events
    EXPECT_FALSE(log.isVerticalEventAttrExtracting());      // по умолчанию — false (horizontal)
    log.setVerticalEventAttrExtracting(true);
    EXPECT_TRUE(log.isVerticalEventAttrExtracting());
    log.setVerticalEventAttrExtracting(false);
    EXPECT_FALSE(log.isVerticalEventAttrExtracting());

    // event log "vertical" attribute extracting for traces
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());      // по умолчанию — false (horizontal)
    log.setVerticalTraceAttrExtracting(true);
    EXPECT_TRUE(log.isVerticalTraceAttrExtracting());
    log.setVerticalTraceAttrExtracting(false);
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, autoLoadConfig1)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);

    // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());

    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    
    log.open();
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, autoLoadConfig2BadQry)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);

    // явно указываем, что надо загружать конфиг и SQL для него

    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());

    log.setAutoLoadConfigQry("BADDDSELECT * FROM DefConfig");

    // должно полететь исключение, т.к. синтаксис неверный
    ASSERT_THROW(log.open(), LdopaException);
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, autoLoadConfigLogQueries)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log(LOG_FILE_RTS_04);

    // проверяем лог-запросы до
    EXPECT_TRUE(log.getQryEventsNumLog().empty());      //EXPECT_TRUE(log.getEventsNumLogQry().empty());
    EXPECT_TRUE(log.getQryTracesLog().empty());         //EXPECT_TRUE(log.getTracesLogQry().empty());
    EXPECT_TRUE(log.getQryTracesNumLog().empty());      //EXPECT_TRUE(log.getTracesNumLogQry().empty());
    EXPECT_TRUE(log.getQryGetLogAttr().empty());        //EXPECT_TRUE(log.getGetLogAttrQry().empty());
    EXPECT_TRUE(log.getQryGetAllLogAttrs().empty());    //EXPECT_TRUE(log.getGetAllLogAttrsQry().empty());
    EXPECT_TRUE(log.getQryGetEventAttr().empty());      //EXPECT_TRUE(log.getGetEventAttr().empty());
    EXPECT_TRUE(log.getQryGetEventAttrByID().empty());  //EXPECT_TRUE(log.getGetEventAttrByID().empty());
    EXPECT_TRUE(log.getQryGetAllEventAttrs().empty());  //EXPECT_TRUE(log.getGetAllEventAttrs().empty());    
    EXPECT_TRUE(log.getQryGetAllEventAttrsByID().empty());
    EXPECT_TRUE(log.getQryGetTraceAttr().empty());
    EXPECT_TRUE(log.getQryGetAllTraceAttrs().empty());
    EXPECT_TRUE(log.getQryGetTraceEventsNum().empty());

    EXPECT_TRUE(log.getQryActivitiesNumLog().empty());
    
    // ИД атрибутов
    EXPECT_TRUE(log.getEvActAttrId().empty());
    EXPECT_TRUE(log.getEvTimestAttrId().empty());
    EXPECT_TRUE(log.getEvCaseAttrId().empty());

    // другие параметры
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());       // НЕ вертикальная загрузка атр. трасс

    // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());

    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();
    
    // проверяем лог-запросы после    
    EXPECT_FALSE(log.getQryEventsNumLog().empty());     //EXPECT_FALSE(log.getEventsNumLogQry().empty());
    EXPECT_FALSE(log.getQryTracesLog().empty());        // EXPECT_FALSE(log.getTracesLogQry().empty());
    EXPECT_FALSE(log.getQryTracesNumLog().empty());     //EXPECT_FALSE(log.getTracesNumLogQry().empty());
    EXPECT_FALSE(log.getQryGetLogAttr().empty());       //EXPECT_FALSE(log.getGetLogAttrQry().empty());
    EXPECT_FALSE(log.getQryGetAllLogAttrs().empty());   //EXPECT_FALSE(log.getGetAllLogAttrsQry().empty());
    EXPECT_FALSE(log.getQryGetEventAttr().empty());     //EXPECT_FALSE(log.getGetEventAttr().empty());
    EXPECT_FALSE(log.getQryGetEventAttrByID().empty()); //EXPECT_FALSE(log.getGetEventAttrByID().empty());
    EXPECT_FALSE(log.getQryGetAllEventAttrs().empty()); //EXPECT_FALSE(log.getGetAllEventAttrs().empty());
    EXPECT_FALSE(log.getQryGetAllEventAttrsByID().empty());
    EXPECT_FALSE(log.getQryGetTraceAttr().empty());
    EXPECT_FALSE(log.getQryGetAllTraceAttrs().empty());
    EXPECT_FALSE(log.getQryGetTraceEventsNum().empty());

    EXPECT_FALSE(log.getQryActivitiesNumLog().empty());

    // ИД атрибутов
    EXPECT_FALSE(log.getEvActAttrId().empty());
    EXPECT_FALSE(log.getEvTimestAttrId().empty());
    EXPECT_FALSE(log.getEvCaseAttrId().empty());


    // другие параметры
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());       // НЕ вертикальная загрузга атр. трасс
}


//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, overloadConfig1)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log(LOG_FILE_RTS_04);

    // запросы
    EXPECT_TRUE(log.getEvCaseAttrId().empty());

    // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());

    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // проверяем лог-запросы после
    EXPECT_FALSE(log.getEvCaseAttrId().empty());

    // сейчас изменится параметр, сохраним для проверки
    std::string caseIDold = log.getEvCaseAttrId();
    log.overloadConfigFromLog("SELECT * FROM Config WHERE persp = 1");
    EXPECT_FALSE(log.getEvCaseAttrId().empty());
    EXPECT_TRUE(log.getEvCaseAttrId() != caseIDold);        // новый параметр д.б.
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, autoLoadConfigParams2)
{
    // подгружаем конфиг и проверяем, что он работает в части других параметров
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);

    // другие параметры ДО
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());       // НЕ вертикальная загрузка атр. трасс
    EXPECT_FALSE(log.isVerticalEventAttrExtracting());       // НЕ вертикальная загрузка атр. событий

    // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfig(true);
    EXPECT_EQ(true, log.isAutoLoadConfig());
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // другие параметры ПОСЛЕ
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());       // НЕ вертикальная загрузга атр. трасс
    EXPECT_FALSE(log.isVerticalEventAttrExtracting());       // НЕ вертикальная загрузка атр. событий
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getEventsNum)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);    
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // проверяем лог-запросы после
    EXPECT_FALSE(log.getQryEventsNumLog().empty());
    EXPECT_EQ(13, log.getEventsNum());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getTracesNum)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // проверяем лог-запросы после
    EXPECT_FALSE(log.getQryTracesNumLog().empty());    //EXPECT_FALSE(log.getTracesNumLogQry().empty());
    EXPECT_EQ(4, log.getTracesNum());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getActivitiesNum)
{
    // подгружаем конфиг и проверяем, что он работает
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // проверяем лог-запросы после
    EXPECT_FALSE(log.getQryActivitiesNumLog().empty());
    EXPECT_EQ(5, log.getActivitiesNum());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getLogAttr1)
{

    using xi::attributes::IDestructableObject;
    using xi::attributes::DestrByteArray;

    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // 1
    SQLiteLog::Attribute a1;
    bool a1r = log.getLogAttr("myatr1", a1);
    EXPECT_TRUE(a1r);
    EXPECT_EQ(SQLiteLog::Attribute::tStringSharedPtr, a1.getType());
    EXPECT_EQ("My Attribute1", a1.asStr());

    // 2
    SQLiteLog::Attribute a2;
    bool a2r = log.getLogAttr("myatr2", a2);
    EXPECT_TRUE(a2r);
    EXPECT_EQ(SQLiteLog::Attribute::tInt64, a2.getType());      // был инт, но его повысили...
    EXPECT_EQ(42, a2.asInt());

    {                                               // проверить деструктор
        // 3
        SQLiteLog::Attribute a3;
        bool a3r = log.getLogAttr("blobatr1", a3);
        EXPECT_TRUE(a3r);
        EXPECT_EQ(SQLiteLog::Attribute::tDestrObjSharedPtr, a3.getType());

        IDestructableObject* dob = a3.asDestrObj();

    }
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllLogAttrs1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    std::shared_ptr<IAttributesEnumerator> aen(log.getLogAttrs());
    
    for (int i = 0; i < 2; ++i)         // для проверки reset-а делаем 2 раза
    {
        int counter = 0;
        while (aen->hasNext())
        {
            IEventLog::NamedAttribute na = aen->getNext();
            ++counter;
        }
        EXPECT_EQ(3, counter);
        EXPECT_TRUE(log.getLogAttrsNum() == counter);

        aen->reset();
    }
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllLogAttrs2)
{
    // используем attributemap

    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    SQLiteLog::AttributesMap am = log.getLogAttrsAsMap();
    EXPECT_EQ(3, am.size());
    EXPECT_TRUE(log.getLogAttrsNum() == am.size());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getEventAttr1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // 1
    SQLiteLog::Attribute a1;
    bool a1r = log.getEventAttr(1, 2, "activity", a1);      // трасса смещ 1 (2) событие смещ 2 (3)
    EXPECT_TRUE(a1r);
    EXPECT_EQ(SQLiteLog::Attribute::tStringSharedPtr, a1.getType());
    EXPECT_EQ("d", a1.asStr());

    // 2
    SQLiteLog::Attribute a2;
    bool a2r = log.getEventAttr(1, 2, "timest", a2);      // трасса смещ 1 (2) событие смещ 2 (3)
    EXPECT_TRUE(a2r);
    EXPECT_EQ(SQLiteLog::Attribute::tInt64, a2.getType());
    EXPECT_EQ(1497980376, a2.asInt64());
}


//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getEventAttrByIDHor1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // 1
    SQLiteLog::Attribute a1;
    bool a1r = log.getEventAttr(6, "activity", a1);      // 6 соотв. трасса смещ 1 (2) событие смещ 2 (3)
    EXPECT_TRUE(a1r);
    EXPECT_EQ(SQLiteLog::Attribute::tStringSharedPtr, a1.getType());
    EXPECT_EQ("d", a1.asStr());

    // 2
    SQLiteLog::Attribute a2;
    bool a2r = log.getEventAttr(6, "timest", a2);      // трасса смещ 1 (2) событие смещ 2 (3)
    EXPECT_TRUE(a2r);
    EXPECT_EQ(SQLiteLog::Attribute::tInt64, a2.getType());
    EXPECT_EQ(1497980376, a2.asInt64());
}


//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getEventAttrByIDVert1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // так как в конфигурации установлено по умолчанию горизонтальное извлечение ивентовых аттрибутов
    // и соответствующий запрос, после чтения конфигурации надо руками задать верт. и новый запрос

    EXPECT_FALSE(log.isVerticalEventAttrExtracting());      // горизонтальная по умолчанию
    log.setVerticalEventAttrExtracting(true);
    log.getQryGetEventAttrByID() = "SELECT * FROM EventsAttrVert WHERE event_id = ?1 AND id = ?2";
    //  эквивалент log.setConfigParam(PAR_LQRY_GETEVENTATTR_BYID, )
    EXPECT_TRUE(log.isVerticalEventAttrExtracting());       // явно задали вертикальную

    // 1
    SQLiteLog::Attribute a1;
    bool a1r = log.getEventAttr(1, "activity", a1);      // 
    EXPECT_TRUE(a1r);
    EXPECT_EQ(SQLiteLog::Attribute::tStringSharedPtr, a1.getType());
    EXPECT_EQ("a", a1.asStr());

    // 2
    SQLiteLog::Attribute a2;
    bool a2r = log.getEventAttr(4, "trace", a2);      // 
    EXPECT_TRUE(a2r);
    EXPECT_EQ(SQLiteLog::Attribute::tInt64, a2.getType());
    EXPECT_EQ(1, a2.asInt64());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllEventAttrs1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // трасса смещ 1 (2) событие смещ 2 (3)
    std::shared_ptr<IAttributesEnumerator> aen(log.getEventAttrs(1, 2));
    
    for (int i = 0; i < 2; ++i)         // для проверки reset-а делаем 2 раза
    {
        int counter = 0;
        while (aen->hasNext())
        {
            IEventLog::NamedAttribute na = aen->getNext();
            ++counter;
        }
        EXPECT_EQ(5, counter);

        aen->reset();
    }
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllEventAttrs2)
{
    // используем attributemap
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // трасса смещ 1 (2) событие смещ 2 (3)
    SQLiteLog::AttributesMap am = log.getEventAttrsAsMap(1, 2);
    EXPECT_EQ(5, am.size());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllEventAttrsByID1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // ивент 6 экв. трасса смещ 1 (2) событие смещ 2 (3)
    std::shared_ptr<IAttributesEnumerator> aen(log.getEventAttrs(6));

    for (int i = 0; i < 2; ++i)         // для проверки reset-а делаем 2 раза
    {
        int counter = 0;
        while (aen->hasNext())
        {
            IEventLog::NamedAttribute na = aen->getNext();
            ++counter;
        }
        EXPECT_EQ(5, counter);

        aen->reset();
    }
}



//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllEventAttrsByID2)
{
    // используем attributemap
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // ид 6 экв. трасса смещ 1 (2) событие смещ 2 (3)
    SQLiteLog::AttributesMap am = log.getEventAttrsAsMap(6);
    EXPECT_EQ(5, am.size());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getTraceAttrHor1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // 1
    SQLiteLog::Attribute a1;
    bool a1r = log.getTraceAttr(1, "case", a1);     // трасса (2) id 1
    EXPECT_TRUE(a1r);
    EXPECT_EQ(SQLiteLog::Attribute::tStringSharedPtr, a1.getType());
    EXPECT_EQ("case2", a1.asStr());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getTraceAttrVert1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // так как в конфигурации установлено по умолчанию горизонтальное извлечение трассовых аттрибутов
    // и соответствующий запрос, после чтения конфигурации надо руками задать верт. и новый запрос
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());      // горизонтальная по умолчанию
    log.setVerticalTraceAttrExtracting(true);
    log.getQryGetTraceAttr() = "SELECT * FROM TracesAttrVert WHERE trace_id = ?1 AND id = ?2";
    EXPECT_TRUE(log.isVerticalTraceAttrExtracting());       // явно задали вертикальную

    // 1
    SQLiteLog::Attribute a1;
    bool a1r = log.getTraceAttr(1, "case", a1);     // трасса (2) id 1
    EXPECT_TRUE(a1r);
    EXPECT_EQ(SQLiteLog::Attribute::tStringSharedPtr, a1.getType());
    EXPECT_EQ("case2", a1.asStr());

    // 2
    SQLiteLog::Attribute a2;
    bool a2r = log.getTraceAttr(1, "xiattr", a2);     // трасса (2) id 1
    EXPECT_TRUE(a2r);
    EXPECT_EQ(SQLiteLog::Attribute::tInt64, a2.getType());
    EXPECT_EQ(42, a2.asInt());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllTraceAttrsHor1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // по умолчанию, там горизонтальный режим извлечения атрибутов
    // трасса смещ 1 
    std::shared_ptr<IAttributesEnumerator> aen(log.getTraceAttrs(1));

    for (int i = 0; i < 2; ++i)         // для проверки reset-а делаем 2 раза
    {
        int counter = 0;
        while (aen->hasNext())
        {
            IEventLog::NamedAttribute na = aen->getNext();
            ++counter;
        }
        EXPECT_EQ(2, counter);          // 2 атрибута в горизонтальном режиме...

        aen->reset();
    }
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllTraceAttrsHor2)
{
    // используем attributemap
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // трасса смещ 1 
    SQLiteLog::AttributesMap am = log.getTraceAttrsAsMap(1);
    EXPECT_EQ(2, am.size());
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllTraceAttrsVert1)
{
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // так как в конфигурации установлено по умолчанию горизонтальное извлечение трассовых аттрибутов
    // и соответствующий запрос, после чтения конфигурации надо руками задать верт. и новый запрос

    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());      // горизонтальная по умолчанию
    log.setVerticalTraceAttrExtracting(true);
    log.getQryGetAllTraceAttrs() = "SELECT * FROM TracesAttrVert WHERE trace_id = ?1";
    EXPECT_TRUE(log.isVerticalTraceAttrExtracting());       // явно задали вертикальную

    // трасса смещ 1 
    std::shared_ptr<IAttributesEnumerator> aen(log.getTraceAttrs(1));

    for (int i = 0; i < 2; ++i)         // для проверки reset-а делаем 2 раза
    {
        int counter = 0;
        while (aen->hasNext())
        {
            IEventLog::NamedAttribute na = aen->getNext();
            ++counter;
        }
        EXPECT_EQ(2, counter);          // 2 атрибута в вертикальном режиме для трассы 1

        aen->reset();
    }
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, getAllTraceAttrsVert2)
{
    // используем attributemap
    SQLiteLog log;
    log.setFileName(LOG_FILE_RTS_04);
    log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
    log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
    log.open();

    // так как в конфигурации установлено по умолчанию горизонтальное извлечение трассовых аттрибутов
    // и соответствующий запрос, после чтения конфигурации надо руками задать верт. и новый запрос
    EXPECT_FALSE(log.isVerticalTraceAttrExtracting());      // горизонтальная по умолчанию
    log.setVerticalTraceAttrExtracting(true);
    log.getQryGetAllTraceAttrs() = "SELECT * FROM TracesAttrVert WHERE trace_id = ?1";
    EXPECT_TRUE(log.isVerticalTraceAttrExtracting());       // явно задали вертикальную

    // трасса смещ 1 
    SQLiteLog::AttributesMap am = log.getTraceAttrsAsMap(1);
    EXPECT_EQ(2, am.size());
}


// new object-oriented approach (as from 16/02/2018)

// берет пару трасс; контроллируем, что деструктор для них вызывается
TEST_F(SQLiteLog_1_Test, getTraces1)
{
    {
        SQLiteLog log;
        log.setFileName(LOG_FILE_RTS_04);
        log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
        //log.getAutoLoadConfigQry() = "SELECT * FROM DefConfig";
        log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
        log.open();

        // здесь будет запрос и память под них же распределится
        EXPECT_EQ(4, log.getTracesNum());
        IEventTrace* tr0 = log.getTrace(0);
        EXPECT_TRUE(tr0 != nullptr);
        EXPECT_TRUE(tr0->getLog() == &log);

        IEventTrace* tr3 = log.getTrace(3);
        EXPECT_TRUE(tr3 != nullptr);

        // несуществующая трасса 4 (всего их 4, последняя №3)
        IEventTrace* tr4 = log.getTrace(4);
        EXPECT_TRUE(tr4 == nullptr);
    }                                                   // проверять деструктор здесь, что трассы удаляет!
}

// берет пару трасс, смотрит размеры
TEST_F(SQLiteLog_1_Test, getTraces2)
{
    {
        SQLiteLog log;
        log.setFileName(LOG_FILE_RTS_04);
        log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
        log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
        log.open();

        // здесь будет запрос и память под них же распределится
        EXPECT_EQ(4, log.getTracesNum());
        IEventTrace* tr0 = log.getTrace(0);
        EXPECT_TRUE(tr0 != nullptr);
        EXPECT_TRUE(tr0->getLog() == &log);
        EXPECT_EQ(3, tr0->getSize());

        IEventTrace* tr3 = log.getTrace(3);
        EXPECT_TRUE(tr3 != nullptr);
        EXPECT_EQ(4, tr3->getSize());
    }                                                   // проверять деструктор здесь, что трассы удаляет!
}


// берет пару событий из трассы
TEST_F(SQLiteLog_1_Test, getEvents1)
{
    {
        SQLiteLog log;
        log.setFileName(LOG_FILE_RTS_04);
        log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
        log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
        log.open();

        // здесь будет запрос и память под них же распределится
        EXPECT_EQ(4, log.getTracesNum());

        IEventTrace* tr3 = log.getTrace(3);
        EXPECT_TRUE(tr3 != nullptr);
        EXPECT_EQ(4, tr3->getSize());

        // берем 0-й ивент
        IEvent* ev0 = tr3->getEvent(0);
        EXPECT_TRUE(ev0 != nullptr);
        IEvent* ev1 = tr3->getEvent(1);
        EXPECT_TRUE(ev1 != nullptr);
        IEvent* ev3 = tr3->getEvent(3);        // тут сразу два вычитать д.б.
        EXPECT_TRUE(ev3 != nullptr);

        IEvent* eve = tr3->getEvent(4);        // такого нет
        EXPECT_TRUE(eve == nullptr);
    }                                                   // проверять деструктор здесь, что ивенты удаляет!
}


// проверяем атрибуты событий из трассы
TEST_F(SQLiteLog_1_Test, getEvents2)
{
    {
        SQLiteLog log;
        log.setFileName(LOG_FILE_RTS_04);
        log.setAutoLoadConfig(true);        // явно указываем, что надо загружать конфиг и SQL для него
        log.setAutoLoadConfigQry("SELECT * FROM DefConfig");
        log.open();

        // здесь будет запрос и память под них же распределится
        EXPECT_EQ(4, log.getTracesNum());

        IEventTrace* tr3 = log.getTrace(3);
        EXPECT_TRUE(tr3 != nullptr);
        EXPECT_EQ(4, tr3->getSize());

        // берем 0-й ивент
        IEvent* ev0 = tr3->getEvent(0);
        EXPECT_TRUE(ev0 != nullptr);

        // инд. доступ
        IEvent::Attribute a;
        ev0->getAttr("activity", a);
        EXPECT_EQ("b", a.asStr());

        // коллекция атрибутов
        int attrsNum = ev0->getAttrsNum();
        int c = 0;
        IAttributesEnumerator* en = ev0->getAttrs();
        while (en->hasNext())
        {
            en->getNext();
            c++;
        }
            
        delete en;

        // проверяем, что наенумерованное число атрибутов совпадает с заявленным
        EXPECT_TRUE(c == attrsNum);
    }                                                   // проверять деструктор здесь, что ивенты удаляет!
}

//------------------------------------------------------------------------------

// берет пару трасс с использованием лукапа для перехода от абс. номеров к идентификаторам кейсов
TEST_F(SQLiteLog_1_Test, getTracesWithLookup1)
{
    instantiateNeedfulAttributeMembers();
    {
        SQLiteLog log;
        log.setFileName(LOG_FILE_RTS_04);
        log.setAutoLoadConfig(true);        
        // в конфигурации 4 параметр lookup_trace_ids = 1, что означает предзагрузку ИДров трасс
        log.setAutoLoadConfigQry("SELECT * FROM Config WHERE persp = 4");
        log.open();

        EXPECT_TRUE(log.isLookupTraceIDs());

        // число трасс и число выделенных идентификаторов трасс
        EXPECT_EQ(4, log.getTracesNum());
        EXPECT_EQ(4, log.getTraceIDs().size());

        // по аналогии с getEvents1()
        IEventTrace* tr3 = log.getTrace(3);
        EXPECT_TRUE(tr3 != nullptr);
        EXPECT_EQ(3, tr3->getSize());

        // берем 0-й ивент 4-й трассы
        IEvent* ev0 = tr3->getEvent(0);
        EXPECT_TRUE(ev0 != nullptr);


        IEventTrace* tr0 = log.getTrace(0);
        EXPECT_TRUE(tr0 != nullptr);
        EXPECT_EQ(4, tr0->getSize());       // "4"
    }
}

//------------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, sqlizeString1)
{
    std::string s1 = "abc";
    std::string s1_ = SQLiteLog::sqlizeString(s1);
    EXPECT_EQ("'abc'", s1_);

    std::string s2 = "a'bc";
    std::string s2_ = SQLiteLog::sqlizeString(s2);
    EXPECT_EQ("'a''bc'", s2_);

    std::string s3 = "'a'b'c'";
    std::string s3_ = SQLiteLog::sqlizeString(s3);
    EXPECT_EQ("'''a''b''c'''", s3_);
}


//------------------------------------------------------------------------------


#pragma region SQLite Helpers

TEST_F(SQLiteLog_1_Test, gtestTrouble1)
{
    int a = 0;

    a = a + 1;
    double b = a + 2;

    EXPECT_EQ(1, a);

    b = 10;

    EXPECT_EQ(10, b);
}

TEST_F(SQLiteLog_1_Test, sqlHelpersStmtGetColsNum1)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT COUNT(*) FROM Events"));
    
    EXPECT_EQ(1, stmt->getColsNum());      // теперь определено

    stmt->fetch();
    
    EXPECT_EQ(1, stmt->getColsNum());
}




TEST_F(SQLiteLog_1_Test, sqlHelpersStmtGetColumn1)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT * FROM Events"));

    EXPECT_EQ(5, stmt->getColsNum());      // теперь определено

    stmt->fetch();

    EXPECT_EQ(5, stmt->getColsNum());

    // BLOB
    int bsize;
    const void* rVoid = stmt->getBlob(0, bsize);
    rVoid = stmt->getBlob(1, bsize);
    rVoid = stmt->getBlob(2, bsize);
    rVoid = stmt->getBlob(3, bsize);
    ASSERT_THROW(stmt->getBlob(5, bsize), LdopaException);   // нет такой строки

    // double
    double rReal = stmt->getDouble(0);
    rReal = stmt->getDouble(2);

    // int
    double rInt = stmt->getInt(0);
    EXPECT_EQ(1, rInt);

    // int64
    xi::types::TInt64 rInt64 = stmt->getInt(0);
    EXPECT_EQ(1, rInt64);
}


TEST_F(SQLiteLog_1_Test, sqlHelpersStmtGetTextColumn2)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT param, value FROM Config WHERE param = 'test_sqlh_text1'"));
    stmt->fetch();

    EXPECT_EQ(2, stmt->getColsNum());

    // c-string
    int size;
    const unsigned char* cStr = stmt->getCStr(1, size);     // "Text1"
    EXPECT_EQ(5, size);

    // русский UTF-8
    std::shared_ptr<SQLiteStmt> stmt2(db.execSql("SELECT param, value FROM Config WHERE param = 'test_sqlh_text2'"));
    stmt2->fetch();

    EXPECT_EQ(2, stmt2->getColsNum());

    // c-string
    cStr = stmt2->getCStr(1, size);     // "Текст"
    EXPECT_EQ(10, size);                // тут будет по 2 байта на символ, т.е. 10 байт

    // смешанный UTF-8
    std::shared_ptr<SQLiteStmt> stmt3(db.execSql("SELECT param, value FROM Config WHERE param = 'test_sqlh_text3'"));

    stmt3->fetch();

    EXPECT_EQ(2, stmt3->getColsNum());

    // c-string
    cStr = stmt3->getCStr(1, size);     // "Zы"
    EXPECT_EQ(3, size);                 // Z - 1 байт, ы - 2 байта
}


TEST_F(SQLiteLog_1_Test, sqlHelpersStmtGetTextColumn3)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text1'"));

    stmt->fetch();

    EXPECT_TRUE(stmt->getColsNum() >= 2);
    
    std::string str = stmt->getStr(1);     // "Text1"
    EXPECT_EQ(5, str.length());
    //EXPECT_EQ("Text1", str);
    EXPECT_TRUE(std::string("Text1") == str);


    //https://stackoverflow.com/questions/688760/how-to-create-a-utf-8-string-literal-in-visual-c-2008
    // в Visual Studio невозможно надеяться, что строковые литералы будут UTF-8, даже если 
    // исходник в UTF-8, поэтому используем эскейпы

    // русский UTF-8
    std::shared_ptr<SQLiteStmt> stmt2(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text2'"));

    stmt2->fetch();

    EXPECT_TRUE(stmt2->getColsNum() >= 2);

    str = stmt2->getStr(1);                 // "Текст"
    EXPECT_EQ(10, str.length());            // тут будет по 2 байта на символ, т.е. 10 байт
    EXPECT_EQ("\xD0\xA2\xD0\xB5\xD0\xBA\xD1\x81\xD1\x82", str);

    // смешанный UTF-8
    //std::shared_ptr<SQLiteStmt> stmt2(db.execSql("SELECT * FROM Config WHERE param = 'test_sqlh_text2'"));
    std::shared_ptr<SQLiteStmt> stmt3(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text3'"));

    stmt3->fetch();

    EXPECT_TRUE(stmt3->getColsNum() >= 2);

    str = stmt3->getStr(1);                 // "Zы"
    EXPECT_EQ(3, str.length());             // Z - 1 байт, ы - 2 байта
    EXPECT_EQ("\x5A\xD1\x8B", str);
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, sqlHelpersStmtGetTextColumn4)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text1'"));

    stmt->fetch();

    EXPECT_TRUE(stmt->getColsNum() >= 2);

    // wide c-string
    int size;
    const wchar_t* cStr = stmt->getWCStr(1, size);      // "Text1"
    EXPECT_EQ(10, size);                                // по 2 байта на символ

    // русский UTF-16
    std::shared_ptr<SQLiteStmt> stmt2(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text2'"));

    stmt2->fetch();
    EXPECT_TRUE(stmt2->getColsNum() >= 2);

    cStr = stmt2->getWCStr(1, size);        // "Текст"
    EXPECT_EQ(10, size);                    // по 2 байта на символ

    // смешанный UTF-16
    std::shared_ptr<SQLiteStmt> stmt3(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text3'"));

    stmt3->fetch();

    EXPECT_TRUE(stmt3->getColsNum() >= 2);

    cStr = stmt3->getWCStr(1, size);        // "Zы"
    EXPECT_EQ(4, size);                     // по 2 байта на символ
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, sqlHelpersStmtGetTextColumn5)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text1'"));

    stmt->fetch();

    EXPECT_TRUE(stmt->getColsNum() >= 2);

    std::wstring str = stmt->getWStr(1);        // "Text1"
    EXPECT_EQ(5, str.length());                 // 10 байт НО 5 символов
    EXPECT_TRUE(std::wstring(L"Text1") == str);
    

    // русский UTF-16
    std::shared_ptr<SQLiteStmt> stmt2(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text2'"));
    stmt2->fetch();

    EXPECT_TRUE(stmt2->getColsNum() >= 2);

    str = stmt2->getWStr(1);                // "Текст"
    EXPECT_EQ(5, str.length());             // тут будет по 2 байта на символ, т.е. 10 байт, НО символов 5!
    EXPECT_EQ(std::wstring(L"Текст"), str);

    // смешанный UTF-8
    std::shared_ptr<SQLiteStmt> stmt3(db.execSql("SELECT * FROM DefConfig WHERE param = 'test_sqlh_text3'"));
    stmt3->fetch();

    EXPECT_TRUE(stmt3->getColsNum() >= 2);

    str = stmt3->getWStr(1);                // "Zы"
    EXPECT_EQ(2, str.length());             // Z - 1 байт, ы - 2 байта, НО 2 символа
    EXPECT_EQ(std::wstring(L"Zы"), str);
}

//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, sqlHelpersStmtBindText1)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT * FROM LogAttributes WHERE id = ?"));
    const char* p = "myatr1";
    stmt->bindText(1, p);                   // 1 — самый левый ? в запросе
    stmt->fetch();

    EXPECT_EQ(3, stmt->getColsNum());

    std::string str = stmt->getStr(0);      // "myatr1"
    EXPECT_EQ("myatr1", str);
    str = stmt->getStr(1);                  // "My Attribute1"
    EXPECT_EQ("My Attribute1", str);

    // перебиндим
    stmt->reset();                          // перед перебиндингом обязательно ресет! 
    const char* p1 = "myatr2";              // иначе 21 ошибка out of sequence
    stmt->bindText(1, p1);    
    stmt->fetch();

    EXPECT_EQ(3, stmt->getColsNum());
    str = stmt->getStr(0);                  // "myatr2"
    EXPECT_EQ("myatr2", str);
    int r42 = stmt->getInt(1);                  // 42
    EXPECT_EQ(42, r42);

    stmt->clearBinding();
    stmt->reset();
    bool lastFetchRes = stmt->fetch();

    EXPECT_EQ(3, stmt->getColsNum());
    EXPECT_FALSE(lastFetchRes);
}


//-----------------------------------------------------------------------------

TEST_F(SQLiteLog_1_Test, sqlHelpersStmtBindText2)
{
    SQLiteDB db(LOG_FILE_RTS_04);
    db.open();

    std::shared_ptr<SQLiteStmt> stmt(db.execSql("SELECT COUNT(*) FROM Events WHERE case_id = ?1"));
    const char* p = "42";
    stmt->bindText(1, p);                   // 1 — самый левый ? в запросе
    stmt->fetch();

    EXPECT_EQ(1, stmt->getColsNum());

    int res = stmt->getInt(0);
    EXPECT_EQ(3, res);
}



#pragma endregion // SQLite Helpers
