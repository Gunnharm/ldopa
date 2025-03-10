////////////////////////////////////////////////////////////////////////////////
// Module Name:  sqlitelog.h/cpp
// Authors:      Sergey Shershakov
// Version:      0.1.0
// Date:         23.08.2017
// Copyright (c) xidv.ru 2014–2017.
//
// This source is for internal use only — Restricted Distribution.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////


// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

// ldopa
#include "xi/ldopa/eventlog/sqlite/sqlitelog.h"
#include "xi/ldopa/utils.h"


// xilib
#include "xi/collections/enumerators.hpp"

// std
#include <algorithm>    // std::sort
#include <memory>       // shared_ptr

namespace xi { namespace ldopa { namespace eventlog {;   //

//==============================================================================
// class SQLiteTrace
//==============================================================================

//-- string consts
const char* SQLiteTrace::PAR_LQRY_GETTRACE_EVENTS = "qryl_get_trace_events";


SQLiteTrace::SQLiteTrace(SQLiteLog* owner, UInt traceNum)
    : _owner(owner)
    , _traceNum(traceNum)
    , _size(-1)                     // пока-то число ивентов неизвестно
    , _evExtractStmt(nullptr)       // экстратора ивентов тоже нет
    , _attrsExtracted(false)
{
}

//------------------------------------------------------------------------------

SQLiteTrace::~SQLiteTrace()
{
    freeEvents(false);       // грохнем ивенты напоследок без перераспр. массива, чтобы сэкон.
    if (_evExtractStmt)
        delete _evExtractStmt;
}

//------------------------------------------------------------------------------

int SQLiteTrace::getAttrsNum()
{
    // перед тем, как запрашивать атрибуты, нужно уточнить, были ли они извлечены
    if (!_attrsExtracted)
        extractAttributes();

    return _attributes.size();
}

//------------------------------------------------------------------------------

bool SQLiteTrace::getAttr(const char* id, Attribute& a)
{
    // перед тем, как запрашивать атрибуты, нужно уточнить, были ли они извлечены
    if (!_attrsExtracted)
        extractAttributes();

    // возможно, извлечение атрибута по имени надо вынести в отдельный метод,
    // зависящей от реализации - структуры хранения
    for (size_t i = 0; i < _attributes.size(); ++i)
    {
        NamedAttribute& el = _attributes[i];
        if (el.first == id)
        {
            a = el.second;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteTrace::getAttrs()
{    
    // перед тем, как запрашивать атрибуты, нужно уточнить, были ли они извлечены
    if (!_attrsExtracted)
        extractAttributes();

    // теперь конструируем перечислятор по вектору, где они закешированы
    AttrsVecCEnum* en = new AttrsVecCEnum(_attributes.cbegin(), _attributes.cend());
    return en;
}

//------------------------------------------------------------------------------

void SQLiteTrace::extractAttributes()
{
    // здесь, в принципе, можно было бы запросить число атрибутов SQL-запросом, 
    // но, кажется, это будет дороже, чем использовать расширяющийся контейнер и
    // заполнить по факту атрибутами, которые запрос вернет

    
    std::shared_ptr<IAttributesEnumerator> aen(_owner->requestTraceAttrs(_traceNum));
    SQLiteLog::copyAttributes2Vec(aen.get(), _attributes);
    _attrsExtracted = true;
}



//------------------------------------------------------------------------------

IEvent* SQLiteTrace::getEvent(UInt eventNum)
{
    // если номер больше числа ивентов, возвращаем по договоренности бублик
    if (eventNum >= (UInt)getSize())
        return nullptr;

    // если уже извлекался ранее, то возвращаем его
    if (eventNum < _events.size())
        return _events[eventNum];

    // иначе надо подгрузить ивенты вплоть до evetnNum-ого включительно
    fetchUpTo(eventNum);

    if (eventNum < _events.size())
        return _events[eventNum];

    // если же и после этого такого ивента нет, значит противоречие
    // между qry, возвращающим для трассы число ивентов, и qry, 
    // возвращающим собственно ивенты, поэтому исключение
    //if (eventNum >= _events.size())
    throw LdopaException(
        "A query for getting event doesn't provide as many events as the trace-size-query promised.");

    //return _events[eventNum];
}

//------------------------------------------------------------------------------

int SQLiteTrace::getSize()
{
    // закешированное значение, исходя из соображений, что размер трассы не поменяется
    if (_size == -1)
        obtainEventsNum();      // requestSize();

    return _size;               // return 0;
}

//------------------------------------------------------------------------------

IEventLog* SQLiteTrace::getLog()
{
    return _owner;
}


//------------------------------------------------------------------------------

void SQLiteTrace::requestSize()
{
    _size = _owner->getTraceSize(_traceNum);
}

//------------------------------------------------------------------------------

void SQLiteTrace::obtainEventsNum()
{
    requestSize();
    allocEventsStorage();
}

//------------------------------------------------------------------------------

void SQLiteTrace::allocEventsStorage()
{
    assert(_events.size() == 0);
    assert(_size != -1);            // это тоже не норм

    // ориентируемся на число трасс, которое д.б. определено к этому моменту
    // т.к. ивенты в силу конструкции мы можем извекать только подряд, то здесь,
    // в отличие от лога, мы не распределяем память с нулевыми указателями, а
    // распределим емкость под макс. число элементов, а размер будем увеличивать по
    // ходу добавления ивентов
    _events.reserve(_size);
}

//------------------------------------------------------------------------------

void SQLiteTrace::freeEvents(bool needResize)
{
    // т.к. к моменту вызова этого метода переменная числа ивентов может быть уже
    // сброшена, то ориентируемся только на структуру
    for (VectorOfEvents::iterator cur = _events.begin(); cur != _events.end(); ++cur)
    {
        IEvent* ev = *cur;
        if (ev)
            delete ev;
    }

    if (needResize)                     // это имеет смысл делать только в том случае,
        _events.resize(0);              // если повторно переиспользоваться будет,
                                        // иначе дешевле это не делать — грожнет сам объект
}

//------------------------------------------------------------------------------

void SQLiteTrace::fetchUpTo(int eventNum)
{
    if (!_evExtractStmt)
        createExtractor();
    
    // число вычитываемых записей надо вынести за цикл, иначе при добавлении в массив
    // будем на 1 сокращать на каждой итерации
    int rowsCount = int(eventNum - _events.size() + 1);
    
    // число фетчей — номер события минус число уже запрошенных + 1
    for (int i = 0; i < rowsCount; ++i)
    {
        // должны иметь возможность фетчить ровно столько, насколько зарядили цикл
        // если не получается очередной фетч, значит что-то неправильно с запросами        
        if (!_evExtractStmt->fetch())
            throw LdopaException("Can't get another event, buth should have.");

        // итак, сфетчилось, создаем очередной ивент с этого фетча
        SQLiteEvent* ev = new SQLiteEvent(this, _evExtractStmt);
        _events.push_back(ev);
    }
}


//------------------------------------------------------------------------------


void SQLiteTrace::createExtractor()
{
    const std::string& qry = _owner->getConfigParamOrDef(PAR_LQRY_GETTRACE_EVENTS); // getQryGetTraceEvents();
    if (qry.empty())
        throw LdopaException(
        "Can't extract trace's events because there has not been set a corresponding query.");

    _evExtractStmt = _owner->_db.execSql(qry);
    
    // теперь не напрямую биндим, а косвенно — либо по абс. номеру, либо через лукап
    _owner->bindTraceID(_evExtractStmt, _traceNum, 1);
}



//==============================================================================
// class SQLiteEvent
//==============================================================================


SQLiteEvent::SQLiteEvent(SQLiteTrace* owner, SQLiteStmt* initStmt)
    : _owner(owner)
{
    // выдираем атрибуты
    extractAttributes(initStmt);
}

//------------------------------------------------------------------------------

bool SQLiteEvent::getAttr(const char* id, Attribute& a)
{
        for (size_t i = 0; i < _attributes.size(); ++i)
        {
            //std::pair<std::string, Attribute>
            NamedAttribute& el = _attributes[i];
            if (el.first == id)
            {
                a = el.second;
                return true;
            }
        }
        return false;
}

//------------------------------------------------------------------------------

int SQLiteEvent::getAttrsNum()
{
    return _attributes.size();
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteEvent::getAttrs()
{
    AttrsVecCEnum* en = new AttrsVecCEnum(_attributes.cbegin(), _attributes.cend());
    return en;
}

//------------------------------------------------------------------------------

IEventTrace* SQLiteEvent::getTrace()
{
    return _owner;
}

//------------------------------------------------------------------------------

void SQLiteEvent::extractAttributes(SQLiteStmt* stmt)
{
    Attribute attr;    
    std::string aName;          //const char* aName = stmt->getColumnName(i); - может так дешевле?

    int colsNum = stmt->getColsNum();
    _attributes.reserve(colsNum);

    // перебираем все колоночки текущей записи
    for (int i = 0; i < colsNum; ++i)
    {
        if (SQLiteLog::extractAttributeHor(stmt, i, attr, &aName))
        {
            _attributes.push_back(make_pair(aName, attr));
        }
    }
}

//------------------------------------------------------------------------------

//==============================================================================
// class SQLiteLog
//==============================================================================


//-- string consts

#pragma region SQLiteLog:: Strings
const char* SQLiteLog::PAR_LQRY_EVENTS_NUM              = "qryl_events_num";
const char* SQLiteLog::PAR_LQRY_TRACES_NUM              = "qryl_traces_num";
const char* SQLiteLog::PAR_LQRY_ACTIVITIES_NUM          = "qryl_get_activities_num";
const char* SQLiteLog::PAR_LQRY_TRACES                  = "qryl_traces";
const char* SQLiteLog::PAR_LQRY_GETLOGATTR              = "qryl_get_logattr";
const char* SQLiteLog::PAR_LQRY_GETALLLOGATTRS          = "qryl_getall_logattrs";
const char* SQLiteLog::PAR_LQRY_GETEVENTATTR            = "qryl_get_eventattr";
const char* SQLiteLog::PAR_LQRY_GETEVENTATTR_BYID       = "qryl_get_eventattr_byid";
const char* SQLiteLog::PAR_LQRY_GETALLEVENTATTRS        = "qryl_getall_eventattrs";
const char* SQLiteLog::PAR_LQRY_GETALLEVENTATTRS_BYID   = "qryl_getall_eventattrs_byid";
const char* SQLiteLog::PAR_LQRY_GETTRACEATTR            = "qryl_get_traceattr";
const char* SQLiteLog::PAR_LQRY_GETALLTRACEATTRS        = "qryl_getall_traceattrs";
const char* SQLiteLog::PAR_LQRY_GETTRACE_EVENTSNUM      = "qryl_get_trace_eventsnum";

// attributes ID params
const char* SQLiteLog::PAR_EV_ACT_ATTRID                = "ev_act_attr_id";
const char* SQLiteLog::PAR_EV_TIMEST_ATTRID             = "ev_timest_attr_id";
const char* SQLiteLog::PAR_EV_CASE_ATTRID               = "ev_case_attr_id";


const char* SQLiteLog::PAR_IS_REQTRACEATTRVERT          = "req_trace_attr_vert";
const char* SQLiteLog::PAR_IS_REQEVENTATTRVERT          = "req_event_attr_vert";

const char* SQLiteLog::PAR_LOOKUP_TRACE_IDS             = "lookup_trace_ids";


const char* SQLiteLog::COMP_LABEL                       = "SQLiteLog";
#pragma endregion //Strings

SQLiteLog::SQLiteLog()
    //: _dbHandle(NULL)
    : _settings(DEF_SETTINGS)
{
    reset();
}

//------------------------------------------------------------------------------

SQLiteLog::SQLiteLog(const std::string& fn)
    : SQLiteLog()
{
    setFileName(fn);
}

//------------------------------------------------------------------------------

SQLiteLog::~SQLiteLog()
{
    close();            // аккуратно закрываем
}


//------------------------------------------------------------------------------

void SQLiteLog::open()
{
    // если уже открыт, ничего более не открываем
    if (isOpenIntern())
        return;

    // открываем БД
    //openSqliteDb();
    _db.open();

    // если все хорошо открылось, подгрузим конфиг из БД
    if (isAutoLoadConfig())
        loadConfigFromLog();
}

//------------------------------------------------------------------------------

void SQLiteLog::close()
{
    // неоткрытый закрыть нельзя
    if (!isOpenIntern())
        return;

    //closeSqliteDb();
    freeTraces();           // удаляем извлеченные и сохраненные трассы перед закрытием БД
    _db.close();
    reset();
}

//------------------------------------------------------------------------------

bool SQLiteLog::isOpen()
{
    return isOpenIntern();
}

//------------------------------------------------------------------------------

void SQLiteLog::copyAttributes2Map(IAttributesEnumerator* aen, AttributesMap& amap)
{
    // перебираем все атрибуты в перечислении и заносим их в мапу
    while (aen->hasNext())
    {
        IEventLog::NamedAttribute na = aen->getNext();
        amap.insert(na);
    }
}

//------------------------------------------------------------------------------

void SQLiteLog::copyAttributes2Vec(IAttributesEnumerator* aen, NmAttributesVectors& avec)
{
    // перебираем все атрибуты в перечислении и заносим их в мапу
    while (aen->hasNext())
    {
        IEventLog::NamedAttribute na = aen->getNext();
        avec.push_back(na);
    }
}

//------------------------------------------------------------------------------

int SQLiteLog::getEventsNum()
{
    if (!isOpenIntern())
        return 0;               // по определению

    // если лог открыт, смотрим, было ли уже запрошено число событий
    if (_logEventsNum == -1)
        requestLogEventsNum();

    return _logEventsNum;
}

//------------------------------------------------------------------------------

int SQLiteLog::getTracesNum()
{
    if (!isOpenIntern())
        return 0;                   // по соглашению

    // если лог открыт, смотрим, было ли уже запрошено число событий
    if (_logTracesNum == -1)
        obtainLogTracesNum();
    return _logTracesNum;
}
//------------------------------------------------------------------------------

int SQLiteLog::getActivitiesNum()
{
    if (!isOpenIntern())
        return 0;                   // по соглашению

    // если лог открыт, смотрим, было ли уже запрошено число событий
    if (_logActivitiesNum == -1)
        requestLogActivitiesNum();

    return _logActivitiesNum;
}


//------------------------------------------------------------------------------

int SQLiteLog::getLogAttrsNum()
{
    if (!isOpenIntern())
        return 0;                   // по соглашению

    // если лог открыт, то перед тем, как запрашивать атрибуты, нужно уточнить, 
    // были ли они извлечены
    if (!_logAttrsExtracted)
        extractLogAttributes();

    return _logAttributes.size();
}

//------------------------------------------------------------------------------

bool SQLiteLog::getLogAttr(const char* id, Attribute& a)
{
    if (!isOpenIntern())
        return false;               // по соглашению

    // если лог открыт, то перед тем, как запрашивать атрибуты, нужно уточнить, знаем
    // ли мы их число и, соответственно, распределено ли достаточно памяти для их хранения
    if (!_logAttrsExtracted)
        extractLogAttributes();

    // возможно, извлечение атрибута по имени надо вынести в отдельный метод,
    // зависящей от реализации - структуры хранения
    for (size_t i = 0; i < _logAttributes.size(); ++i)
    {
        NamedAttribute& el = _logAttributes[i];
        if (el.first == id)
        {
            a = el.second;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::getLogAttrs()
{
    if (!isOpenIntern())
        return nullptr;               // по соглашению

    // если лог открыт, то перед тем, как запрашивать атрибуты, нужно уточнить, знаем
    // ли мы их число и, соответственно, распределено ли достаточно памяти для их хранения
    if (!_logAttrsExtracted)
        extractLogAttributes();

    // теперь конструируем перечислятор по вектору, где они закешированы
    NmAttrsVecCEnum* en = new NmAttrsVecCEnum(_logAttributes.cbegin(), _logAttributes.cend());
    return en;
}

//------------------------------------------------------------------------------

void SQLiteLog::extractLogAttributes()
{
    std::shared_ptr<IAttributesEnumerator> aen(requestLogAttrs());
    copyAttributes2Vec(aen.get(), _logAttributes);
    _logAttrsExtracted = true; //_logAttrsNum = _logAttributes.size();
}


//------------------------------------------------------------------------------

bool SQLiteLog::getEventAttr(int traceNum, int eventNum, const char* id, Attribute& a)
{
    if (!isOpenIntern())
        return false;               // по соглашению

    return requestEventAttr(traceNum, eventNum, id, a);
}

//------------------------------------------------------------------------------

bool SQLiteLog::getEventAttr(int eventId, const char* id, Attribute& a)
{
    if (!isOpenIntern())
        return false;               // по соглашению

    return requestEventAttrByID(eventId, id, a);
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::getEventAttrs(int traceNum, int eventNum)
{
    if (!isOpenIntern())
        return nullptr;               // по соглашению

    return requestEventAttrs(traceNum, eventNum);
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::getEventAttrs(int eventId)
{
    if (!isOpenIntern())
        return nullptr;               // по соглашению

    return requestEventAttrsByID(eventId);
}

//------------------------------------------------------------------------------

bool SQLiteLog::getTraceAttr(int traceNum, const char* id, Attribute& a)
{
    if (!isOpenIntern())
        return false;               // по соглашению

    return requestTraceAttr(traceNum, id, a);

}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::getTraceAttrs(int traceNum)
{
    if (!isOpenIntern())
        return nullptr;               // по соглашению

    return requestTraceAttrs(traceNum);
}

//------------------------------------------------------------------------------

int SQLiteLog::getTraceSize(int traceNum)
{
    if (!isOpenIntern())
        return 0;               // по соглашению

    return requestTraceSize(traceNum);
}



//------------------------------------------------------------------------------

std::string SQLiteLog::getEvActAttrId() const
{
    return getConfigParamOrEmpty(PAR_EV_ACT_ATTRID);
}

//------------------------------------------------------------------------------

std::string SQLiteLog::getEvTimestAttrId() const
{
    return getConfigParamOrEmpty(PAR_EV_TIMEST_ATTRID);
}

//------------------------------------------------------------------------------

std::string SQLiteLog::getEvCaseAttrId() const
{
    return getConfigParamOrEmpty(PAR_EV_CASE_ATTRID);
}

//------------------------------------------------------------------------------

std::string SQLiteLog::getInfoStr() const
{
    // инф. строка: название логера и имя текущего файла лога

    std::string res(COMP_LABEL);
    res.append(": ").append(getFileName());

    return res;
}

//------------------------------------------------------------------------------

SQLiteLog::AttributesMap SQLiteLog::getLogAttrsAsMap()
{
    AttributesMap aMap;
    if (!isOpenIntern())
        return aMap;

    std::shared_ptr<IAttributesEnumerator> aen(requestLogAttrs());
    copyAttributes2Map(aen.get(), aMap);
    
    return aMap;
}

//------------------------------------------------------------------------------

SQLiteLog::AttributesMap SQLiteLog::getEventAttrsAsMap(int traceNum, int eventNum)
{
    AttributesMap aMap;
    if (!isOpenIntern())
        return aMap;

    std::shared_ptr<IAttributesEnumerator> aen(getEventAttrs(traceNum, eventNum));
    copyAttributes2Map(aen.get(), aMap);

    return aMap;

}

//------------------------------------------------------------------------------

SQLiteLog::AttributesMap SQLiteLog::getEventAttrsAsMap(int eventId)
{
    AttributesMap aMap;
    if (!isOpenIntern())
        return aMap;

    std::shared_ptr<IAttributesEnumerator> aen(getEventAttrs(eventId));
    copyAttributes2Map(aen.get(), aMap);

    return aMap;
}

//------------------------------------------------------------------------------

SQLiteLog::AttributesMap SQLiteLog::getTraceAttrsAsMap(int traceNum)
{
    AttributesMap aMap;
    if (!isOpenIntern())
        return aMap;

    std::shared_ptr<IAttributesEnumerator> aen(getTraceAttrs(traceNum));
    copyAttributes2Map(aen.get(), aMap);

    return aMap;
}

//------------------------------------------------------------------------------

void SQLiteLog::overloadConfigFromLog(const std::string& confq)
{
    if (!isOpenIntern())
        throw LdopaException("Can't overload config cause the log is not open");

    loadConfigFromLog(confq);
}



//------------------------------------------------------------------------------

void SQLiteLog::loadConfigFromLog()
{
    if (_qryLoadConfig.empty())
        return;                         // молча ничего не загружаем
    loadConfigFromLog(_qryLoadConfig);
    processConfigParams();
}


//------------------------------------------------------------------------------

void SQLiteLog::reset()
{
    _logEventsNum = -1;
    _logTracesNum = -1;
    _logActivitiesNum = -1; 
    _logAttrsExtracted = false; // вместо _logAttrsNum = -1;    

    // очищаем структуры        
    _workQueries.clear();
    _logAttributes.clear();
}

//------------------------------------------------------------------------------

void SQLiteLog::requestLogEventsNum()
{
    const std::string& qry = getQryEventsNumLog();      // getConfigParam(PAR_LQRY_EVENTS_NUM);
    if (qry.empty())            //isStrNullOrEmpty(qry))                          //if (_qrylEventsNum.empty())
        throw LdopaException(
            "Can't request a number of events because there has not been set a corresponding query.");

    // работаем через индивидуальный statement    
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry)); 

    // вытаскиваем одну (и только одну) запись: если не вытаскивается, это сурьезно...
    if (!stmt->fetch())
        throw LdopaException("Can't request a number of events: query does not return result.");

    _logEventsNum = stmt->getInt(0);        // 0 — надо бы зафиксировать константой
}

//------------------------------------------------------------------------------

void SQLiteLog::requestLogTracesNum()
{
    const std::string& qry = getQryTracesNumLog();
    if (qry.empty())
        throw LdopaException(
            "Can't request a number of traces because there has not been set a corresponding query.");

    // работаем через индивидуальный statement
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry)); // _qrylTracesNum));

    // вытаскиваем одну (и только одну) запись: если не вытаскивается, это сурьезно...
    if (!stmt->fetch())
        throw LdopaException("Can't request a number of traces: query does not return result.");

    _logTracesNum = stmt->getInt(0);        // 0 — надо бы зафиксировать константой
}

//------------------------------------------------------------------------------

void SQLiteLog::obtainLogTracesNum()
{    
    requestLogTracesNum();    
    allocTracesStorage();
}



//------------------------------------------------------------------------------

void SQLiteLog::requestLogActivitiesNum()
{
    const std::string& qry = getQryActivitiesNumLog();
    if (qry.empty())                  
        throw LdopaException(
        "Can't request a number of activities because there has not been set a corresponding query.");

    // работаем через индивидуальный statement
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry));

    // вытаскиваем одну (и только одну) запись: если не вытаскивается, это сурьезно...
    if (!stmt->fetch())
        throw LdopaException("Can't request a number of activities: query does not return result.");

    _logActivitiesNum = stmt->getInt(0);        // 0 — надо бы зафиксировать константой
}
//------------------------------------------------------------------------------


bool SQLiteLog::requestLogAttr(const char* id, Attribute& a)
{
    // DONE: переделать на использование extractAttributeVertAndCheck

    const std::string& qry = getQryGetLogAttr();
    if (qry.empty())                          //if (_qrylGetLogAttr.empty())
        throw LdopaException(
        "Can't extract a log attribute because there has not been set a corresponding query.");

    // работаем через индивидуальный statement
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry)); // _qrylGetLogAttr));
    stmt->bindText(1, id);
    
    // декодируем результат, исходя из такого раскалада (для однозначности) колонок:
    // 0 - id атрибута
    // 1 - значение атрибута
    // 2 - тип атрибута, представленный кодом
    return fetchAttributeVert(stmt.get(), id, a, 0, 1, 2);
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::requestLogAttrs()
{
    const std::string& qry = getQryGetAllLogAttrs();
    if (qry.empty())                          //if (_qrylGetAllLogAttrs.empty())
        throw LdopaException(
        "Can't extract a log attribute because there has not been set a corresponding query.");

    // здесь передадим управление памятью вспомогательному объекту перечисления
    SQLiteStmt* stmt = _db.execSql(qry);                // _qrylGetAllLogAttrs);

    return new SQLiteStmtAttrsVertEnumerator(stmt, true);   // передаем управление памятью stmt
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::requestEventAttrs(int traceNum, int eventNum)
{
    // и на гориз, и на верт — один запрос с одним параметром — номером трассы
    const std::string& qry = getQryGetAllEventAttrs();
    if (qry.empty())                        //if (_qrylGetAllEventAttrs.empty())
        throw LdopaException(
        "Can't extract event attributes because there has not been set a corresponding query.");

    // здесь передадим управление памятью вспомогательному объекту перечисления
    SQLiteStmt* stmt = _db.execSql(qry);                // _qrylGetAllEventAttrs);
    //stmt->bindInt(1, traceNum);           // первый параметр — номер трассы
    bindTraceID(stmt, traceNum, 1);         // новый способ бинда
    stmt->bindInt(2, eventNum);             // второй параметр — номер ивента в трассе (смещение)

    // вертикальный режим
    if (isVerticalTraceAttrExtracting())
        return new SQLiteStmtAttrsVertEnumerator(stmt, true, 1, 2, 3);  // см. маппинг атрибутов!

    // горизонтальный режим
    return new SQLiteStmtAttrsHorEnumerator(stmt, true);
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::requestEventAttrsByID(int eventId)
{
    // и на гориз, и на верт — один запрос с одним параметром — номером трассы
    const std::string& qry = getQryGetAllEventAttrsByID();
    if (qry.empty())                            //if (_qrylGetAllEventAttrsByID.empty())
        throw LdopaException(
        "Can't extract event attributes because there has not been set a corresponding query.");


    // здесь передадим управление памятью вспомогательному объекту перечисления
    SQLiteStmt* stmt = _db.execSql(qry);        // _qrylGetAllEventAttrsByID);
    stmt->bindInt(1, eventId);                  // первый параметр — ид события

    // вертикальный режим
    if (isVerticalTraceAttrExtracting())
        return new SQLiteStmtAttrsVertEnumerator(stmt, true, 1, 2, 3);  // см. маппинг атрибутов!

    // горизонтальный режим
    return new SQLiteStmtAttrsHorEnumerator(stmt, true);
}




//------------------------------------------------------------------------------

bool SQLiteLog::requestEventAttr(int traceNum, int eventNum, 
    const char* id, Attribute& a)
{
    const std::string& qry = getQryGetEventAttr();
    if (qry.empty())                          //if (_qrylGetEventAttr.empty())
        throw LdopaException(
        "Can't extract an event attribute because there has not been set a corresponding query.");


    // запрос 
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry)); // _qrylGetEventAttr));

    // 1-й параметр извлечения атрибута события — смещение трассы,
    // 2-й параметр — смещение атрибута с учетом сортировки датасета,
    // будет для обоих режимов
    //stmt->bindInt(1, traceNum);         // первый параметр — номер трассы
    bindTraceID(stmt.get(), traceNum, 1);         // новый способ бинда трассы
    stmt->bindInt(2, eventNum);         // второй параметр — номер ивента в трассе (смещение)


    // вертикальный режим   // TODO: надо тестировать, но там муторные запросы, т.ч. до лучших времен
    if (isVerticalEventAttrExtracting())
    {
        // 3-й параметр запроса — идентификатор атрибута — только для вертикального
        stmt->bindText(3, id);

        return fetchAttributeVert(stmt.get(), id, a, 1, 2, 3);
    }

    // горизонтальный режим
    return fetchAttributeHor(stmt.get(), id, a);
}

//------------------------------------------------------------------------------

bool SQLiteLog::requestEventAttrByID(int eventId, const char* id, Attribute& a)
{
    const std::string& qry = getQryGetEventAttrByID();
    if (qry.empty())                          //if (_qrylGetEventAttrByID.empty())
        throw LdopaException(
        "Can't extract an event attribute because there has not been set a corresponding query.");

    // запрос 
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry));  // _qrylGetEventAttrByID));

    // 1-й параметр извлечения атрибута события — уникальный номер события,
    // будет для обоих режимов
    stmt->bindInt(1, eventId);

    // вертикальный режим
    if (isVerticalEventAttrExtracting())
    {
        // 2-й параметр запроса — идентификатор атрибута — только для вертикального
        stmt->bindText(2, id);

        return fetchAttributeVert(stmt.get(), id, a, 1, 2, 3);
    }

    // горизонтальный режим
    return fetchAttributeHor(stmt.get(), id, a);
}


//------------------------------------------------------------------------------

bool SQLiteLog::requestTraceAttr(int traceNum, const char* id, Attribute& a)
{
    // при горизонтальном запросе используем тот же самый запрос, что и для извлечения одного атрибута!
    const std::string& qry = getQryGetTraceAttr();
    if (qry.empty())                                        //if (_qrylGetTraceAttr.empty())
        throw LdopaException(
        "Can't extract trace attributes because there has not been set a corresponding query.");

    // запрос
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry));     //_qrylGetTraceAttr));

    // 1-й параметр извлечения атрибута трассы — уникальный номер трассы (с 0),
    // будет для обоих режимов
    bindTraceID(stmt.get(), traceNum, 1);         // новый способ бинда трассы

    // вертикальный режим
    if (isVerticalTraceAttrExtracting())
    {
        // 2-й параметр запроса — идентификатор атрибута — только для вертикального
        stmt->bindText(2, id);

        return fetchAttributeVert(stmt.get(), id, a, 1, 2, 3);
    }

    // горизонтальный режим
    return fetchAttributeHor(stmt.get(), id, a);
}

//------------------------------------------------------------------------------

IAttributesEnumerator* SQLiteLog::requestTraceAttrs(int traceNum)
{
    // и на гориз, и на верт — один запрос с одним параметром — номером трассы
    // при горизонтальном запросе используем тот же самый запрос, что и для извлечения одного атрибута!
    const std::string& qry = getQryGetAllTraceAttrs();
    if (qry.empty())                                        //if (_qrylGetAllTraceAttrs.empty())
        throw LdopaException(
        "Can't extract trace attributes because there has not been set a corresponding query.");

    // !!!здесь передадим управление памятью stmt вспомогательному объекту перечисления!!! не надо умных указателей!
    SQLiteStmt* stmt = _db.execSql(qry);                    // _qrylGetAllTraceAttrs);
    bindTraceID(stmt, traceNum, 1);         // новый способ бинда трассы

    // вертикальный режим
    if (isVerticalTraceAttrExtracting())
        return new SQLiteStmtAttrsVertEnumerator(stmt, true, 1, 2, 3);  // см. маппинг атрибутов!

    // горизонтальный режим
    return new SQLiteStmtAttrsHorEnumerator(stmt, true);
}

//------------------------------------------------------------------------------

int SQLiteLog::requestTraceSize(int traceNum)
{
    const std::string& qry = getQryGetTraceEventsNum();
    if (qry.empty())                                        //if (_qrylGetTraceEventsNum.empty())
        throw LdopaException(
        "Can't request a trace size because there has not been set a corresponding query.");

    // здесь передадим управление памятью вспомогательному объекту перечисления
    SQLiteStmt* stmt = _db.execSql(qry);                    // _qrylGetTraceEventsNum);

    // теперь не напрямую биндим, а косвенно — либо по абс. номеру, либо через лукап
    bindTraceID(stmt, traceNum, 1);
    // вытаскиваем одну (и только одну) запись: если не вытаскивается, это сурьезно...
    if (!stmt->fetch())
    {
        delete stmt;
        throw LdopaException("Can't request a trace size: query does not return result.");
    }
        
    return stmt->getInt(0);        // 0 — надо бы зафиксировать константой (тж. см. запрос числа событий)
}

//------------------------------------------------------------------------------

void SQLiteLog::allocTracesStorage()
{
    assert(_traces.size() == 0);
    assert(_logTracesNum != -1);            // это тоже не норм

    // ориентируемся на число трасс, которое д.б. определено к этому моменту
    _traces.resize(_logTracesNum);
    for (VectorOfTraces::iterator cur = _traces.begin(); cur != _traces.end(); ++cur)
        *cur = nullptr;                     // TODO: возможно, это лишнее, т.к. нулями будет, проверить!
}

//------------------------------------------------------------------------------

void SQLiteLog::freeTraces()
{
    typedef VectorOfTraces::iterator IT;
    // т.к. к моменту вызова этого метода переменная числа трасс может быть уже
    // сброшена, то ориентируемся только на структуру
    
    for (IT cur = _traces.begin(); cur != _traces.end(); ++cur)
    {
        IEventTrace* tr = *cur;
        if (tr)
            delete tr;
    }

    _traces.resize(0);
}

//------------------------------------------------------------------------------

void SQLiteLog::bindTraceID(SQLiteStmt* stmt, int traceNum, int paramNum /*= 1*/)
{
    // если простой не-lookup режим
    if (!isLookupTraceIDs())
    {
        stmt->bindInt(paramNum, traceNum);       // первый параметр — номер трассы
        return;
    }

    //==lookup режим

    // если для номера трассы нет подходящего идентификатора, это что-то нехорошее
    if ((size_t)traceNum >= _traceIDs.size())
        throw LdopaException("Can't lookup for a value for a given trace num.");

    
    Attribute a = _traceIDs[traceNum];          // собственно — лукап-атрибут
    bindAttribute(stmt, a, paramNum);           // биндим
}

//------------------------------------------------------------------------------

IEventTrace* SQLiteLog::getTrace(int traceNum)
{
    if (!isOpenIntern())
        return nullptr;                 // по соглашению

    // если лог открыт, то перед тем, как запрашивать трассы, нужно уточнить, знаем
    // ли мы их число и, соответственно, распределено ли достаточно памяти для их хранения
    if (_logTracesNum == -1)
        obtainLogTracesNum();           // здесь и запросят, и распределят

    // если запрошенная трасса за пределами, представляемыми логом, покеда
    if (traceNum >= _logTracesNum)
        return nullptr;

    // сперва смотрим, была ли такая трасса уже ранее запрошена (и добавлена в массивчек)
    // если да, то ее и вернем
    SQLiteTrace* tr = _traces[traceNum];
    if (tr)
        return tr;

    // если же такой трассы еще не извлекалось, создаем новый объект трассу
    tr = new SQLiteTrace(this, traceNum);
    _traces[traceNum] = tr;
        
    return tr;
}


//------------------------------------------------------------------------------

bool SQLiteLog::fetchAttributeHor(SQLiteStmt* stmt, const char* aId, Attribute& a)
{
    using xi::attributes::DestrByteArray;
    using xi::attributes::DestrObjSharedPtr;

    if (!stmt->fetch())
        return false;                   // ну вот нетути такого

    // перебираем все колоночки
    for (int i = 0; i < stmt->getColsNum(); ++i)
    {
        const char* cName = stmt->getColumnName(i);
        if (isStrsEq(aId, cName))                   // нашли параметр
            return extractAttributeHor(stmt, i, a); // здесь ID не просим извлекать, т.к. итак его знаем
    }

    return false;
}

//------------------------------------------------------------------------------

// static
void SQLiteLog::bindAttribute(SQLiteStmt* stmt, const Attribute& a, int num)
{
    // см. тж. extractAttributeHor() для уточнения, как соотносить типы
    // декодируем по типу
    switch (a.getType())
    {
        case Attribute::tBlank:
        {
            stmt->bindText(num, "NULL");                // TODO: разобраться (т.к. пойдет в кавычках"
            break;
        }
        case Attribute::tUChar:
        case Attribute::tInt:
        case Attribute::tUInt:
        {
            stmt->bindInt(num, a.asInt());              // TODO: проверять
            break;
        }
        case Attribute::tInt64:
        case Attribute::tUInt64:
        {
            stmt->bindInt64(num, a.asInt64());          // TODO: проверять
            break;

        }
        case Attribute::tDouble:
        {
            stmt->bindDouble(num, a.asDouble());        // TODO: проверять  
            break;

        }
        default:                                // всех остальных — просто в строку превращаем
        {
            // APPNOTE: здесь обязательно надо делать копию привязываемого параметра,
            // т.к. обработка будет отделена от параметра!
            // вообще, этот момент апосля можно пооптимизировать
            std::string sPar = a.toString();
            stmt->bindTextCopy(num, sPar.c_str(), sPar.length()); // sqlPar);
        }
    }; // switch (a.getType())
}

//------------------------------------------------------------------------------

// оказалось, что для BIND-а метод - ненужный!
// static
std::string SQLiteLog::sqlizeString(const char* s, int lenHint /* = -1*/)
{
    static const char SQT = '\'';

    if (lenHint == -1)
        lenHint = strlen(s);
    std::string res;
    //res.resize(lenHint + 2);              // а низзя тут капасити-то задавать!
    res.push_back(SQT);                     // откр кувычка

    for (int i = 0; i < lenHint; ++i)
    {
        char c = s[i];
        res.push_back(c);                   // один раз всегда добавляем
        if (c == SQT)
            res.push_back(c);               // и еще один — для кувычки
    }

    res.push_back(SQT);                     // закр кувычка

    return res;
}


//------------------------------------------------------------------------------

bool SQLiteLog::extractAttributeVert(SQLiteStmt* stmt, std::string& aId, Attribute& a,
    int idNum, int valNum, int typeNum)
{
    using xi::attributes::DestrByteArray;
    using xi::attributes::DestrObjSharedPtr;


    // id (имя) атрибута    
    aId = stmt->getStr(idNum);
    if (aId.empty())
        return false;

    int pt = stmt->getInt(typeNum);             // тип атрибута, декодируем
    switch (pt)                                 // TODO: вынести в константы!
    {
        case 0:                             // целое
        {
            //a = stmt->getInt(1);    // DONE: переделать на 64!
            a = stmt->getInt64(valNum);
            return true;
        }
        case 1:                             // double
        {
            a = stmt->getDouble(valNum);
            return true;
        }
        case 2:
        {
            std::string* s = new std::string(stmt->getStr(valNum));  // завели указатель
            xi::attributes::StringSharedPtr sp(s);              // умный указатель
            a = sp;                                     // вариант
            return true;
        }
        case 3:
        {
            int dataSize;
            const void* rawData = stmt->getBlob(valNum, dataSize);
            if (!rawData)                   // может и не быть
                return false;
            DestrByteArray* bArr = new DestrByteArray((const DestrByteArray::Byte*)rawData, dataSize);
            DestrObjSharedPtr bArrPtr(bArr);
            a = bArrPtr;
            return true;
        }
    } // switch

    return false;
}

//------------------------------------------------------------------------------

bool SQLiteLog::fetchAttributeVert(SQLiteStmt* stmt, const char* reqId, 
    Attribute& a, int idNum, int valNum, int typeNum)
{
    if (!stmt->fetch())
        return false;           // ну вот нетути такого

    std::string realId;
    if (!extractAttributeVert(stmt, realId, a, idNum, valNum, typeNum))
        throw LdopaException("Error when extracting log attribute: bad data.");

    // атрибут извлекся, проверим теперь его имя
    if (realId != reqId)
        throw LdopaException("Error when extracting log attribute: bad SQL query.");

    return true;
}

//------------------------------------------------------------------------------

bool SQLiteLog::extractAttributeHor(SQLiteStmt* stmt, int iCol, Attribute& a, std::string* aId)
{
    using xi::attributes::DestrByteArray;
    using xi::attributes::DestrObjSharedPtr;


    if (iCol >= stmt->getColsNum())
        return false;       // зачем тут кидать исключение, когда просто нет такого арибута?!
        //throw LdopaException("Can't extract an attribute: no such attribute number.");


    // если просят также извлечь имя атрибута (это не всегда необходимо)
    if (aId)
        *aId = stmt->getColumnName(iCol);

    // декодируем по типу
    switch (stmt->getType(iCol))
    {
        case SQLiteDB::stInt:
        {
            a = stmt->getInt64(iCol);
            return true;
        }
        case SQLiteDB::stDouble:
        {
            a = stmt->getDouble(iCol);
            return true;
        }
        case SQLiteDB::stText:
        {
            std::string* s = new std::string(stmt->getStr(iCol));  // завели указатель
            xi::attributes::StringSharedPtr sp(s);              // умный указатель
            a = sp;                                             // вариант
            return true;
        }
        case SQLiteDB::stBlob:
        {
            int dataSize;
            const void* rawData = stmt->getBlob(iCol, dataSize);
            if (!rawData)                   // может и не быть
                return false;
            DestrByteArray* bArr = new DestrByteArray((const DestrByteArray::Byte*)rawData, dataSize);
            DestrObjSharedPtr bArrPtr(bArr);
            a = bArrPtr;
            return true;
        }
        case SQLiteDB::stNull:
        {
            a.clear();                      // NULL в поле (теперь уже не) считаем отсутствием атрибута
            return true;    // false;
        }
    } // switch

    return false;   // TODO:
}


//------------------------------------------------------------------------------

void SQLiteLog::loadConfigFromLog(const std::string& confq)
{
    try
    {
        _db.sqlExec(confq.c_str(), this, &cb_loadConfig);
    }
    catch (LdopaException& e)
    {
        // добавляем осмысленный коммент и пересылаем SQL-ошибку
        throw LdopaException::f("Error when obtaining config from a log. SQLite Error %s",
            e.what());
    }
}

//------------------------------------------------------------------------------

void SQLiteLog::processConfigParams()
{
    // высматриваем из списка презагруженных параметров конфига отдельных личностей

    bool boolflag;

    // вертикальный ли режим запрос атрибутов трассы
    if (checkConfigAutoParam(PAR_IS_REQTRACEATTRVERT, boolflag))
        setVerticalTraceAttrExtracting(boolflag);

    // вертикальный ли режим запрос атрибутов событий
    if (checkConfigAutoParam(PAR_IS_REQEVENTATTRVERT, boolflag))
        setVerticalEventAttrExtracting(boolflag);


    // предзагрузка атрибутов трасс для последующего lookup-а
    if (checkConfigAutoParam(PAR_LOOKUP_TRACE_IDS, boolflag))
    {
        if (boolflag)
        {
            setLookupTraceIDs(true);
            loadTraceIDs();
        }
    }
}




//------------------------------------------------------------------------------

int SQLiteLog::cb_loadConfig(void* sqlog, int colsNum, char** colsData, char** colNames)
{    
    if (!sqlog)
        throw LdopaException("Bad 'LoadConfig' callback.");

    SQLiteLog* so = (SQLiteLog*)sqlog;
    so->onLoadConfig(colsNum, colsData, colNames);

    return 0;           // не ноль прервал бы выполнение запроса (фетчинг строк)
}

//------------------------------------------------------------------------------

void SQLiteLog::onLoadConfig(int colsNum, char** colsData, char** colNames)
{
    // просто берем первые 2 колонки. по договоренности, 0-я — это всегда параметр,
    // 1-я — это всегда значение. если есть какие-то доп, например номер перспективы,
    // они тут не учитываются
    char* param = colsData[0];
    char* value = colsData[1];

    onLoadConfigRecord(param, value);
}

//------------------------------------------------------------------------------

void SQLiteLog::onLoadConfigRecord(const char* param, const char* value)
{
    // в новой версии сперва просто загружаем все конфигурационные параметры в мапу
    _workQueries[param] = value;
}

//------------------------------------------------------------------------------

bool SQLiteLog::checkParamLQry(const char* aParam, const char* val, 
    const char* qParam, std::string& qry)
{
    if (!isStrsEq(qParam, aParam))
        return false;
    //if (isAllowQryConfOverwrite())
        qry = val;
        
    return true;
}

//------------------------------------------------------------------------------

bool SQLiteLog::checkAutoParam(const char* aParam, const char* val, const char* qParam, 
    bool& confParam)
{
    if (!isStrsEq(qParam, aParam))
        return false;

    // положительной булкой считаем строго значение "1"
    if (!val)
        return false;
    
    confParam = isStrsEq(val, "1");

    return true;
}

//------------------------------------------------------------------------------

const std::string* SQLiteLog::getConfigParam(const char* param) const
{
    SqlQueriesMap::const_iterator it = _workQueries.find(param);
    if (it == _workQueries.end())
        return nullptr;

    // есть такой параметр
    return &(it->second);
}

//------------------------------------------------------------------------------

std::string SQLiteLog::getConfigParamOrEmpty(const char* param) const
{
    const std::string* atr = getConfigParam(param);
    if (!atr)
        return std::string();

    return *atr;
}

//------------------------------------------------------------------------------

bool SQLiteLog::checkConfigAutoParam(const char* par, bool& confParam)
{
    const std::string* parval = getConfigParam(par);
    if (!parval)
        return false;

    // есть такой ключ, проверим, есть ли в нем строго значение 1
    confParam = isStrsEq(parval->c_str(), "1");

    return true;
}


//------------------------------------------------------------------------------

void SQLiteLog::loadTraceIDs()
{
    // число трасс, возвращенных запросом на число трасс, должно совпать с таковым,
    // предоставляемым отдельным запросом. Если не совпадет — будем ругаться исключением
    int tn = getTracesNum();
    if (tn == 0)
        return;                         // нет трасс в логе, пустой он
    
    _traceIDs.reserve(tn);              // сразу выделяем память под число трасс


//    int a = 0;
    const std::string& qry = getQryTracesLog();
    if (qry.empty())
        throw LdopaException(
            "Can't extract a collection of trace IDs because there has not been set a corresponding query while 'lookup_trace_ids' param is set.");

    // запрос 
    std::shared_ptr<SQLiteStmt> stmt(_db.execSql(qry));

    // проходимся по всем фетчам, сохраняя число
    int trProcessed = 0;
    while (stmt->fetch())
    {
        ++trProcessed;
        if (trProcessed > tn)           // вернули трасс больше, чем просили — это плохо!
            throw LdopaException(
                "A query extracting traces IDs returned more records than a number of traces provided by a corresponding query.");

        Attribute a;
        if(!extractAttributeHor(stmt.get(), 0, a, nullptr))
            throw LdopaException("Error when extracting a trace ID.");

        _traceIDs.push_back(a);
    }
    if (trProcessed != tn)
        throw LdopaException(
            "A query extracting traces IDs returned less records than a number of traces provided by a corresponding query.");

}



//==============================================================================
// class SQLiteStmtAttrsVertEnumerator
//==============================================================================


SQLiteStmtAttrsVertEnumerator::SQLiteStmtAttrsVertEnumerator(SQLiteStmt* stmt, 
    bool mngStLifet /*= true*/, int idNum, int valNum, int typeNum)
    : _stmt(stmt)
    , _mngStLifetime(mngStLifet)
    , _idNum(idNum)
    , _valNum(valNum)
    , _typeNum(typeNum)
{
    _isFetched = _stmt->fetch();
}

//------------------------------------------------------------------------------

SQLiteStmtAttrsVertEnumerator::~SQLiteStmtAttrsVertEnumerator()
{
    // если передали управление памятью на stmt, удалим его
    if (_mngStLifetime)
        delete _stmt;
}

//------------------------------------------------------------------------------

bool SQLiteStmtAttrsVertEnumerator::hasNext() const
{
    return _isFetched;
}

//------------------------------------------------------------------------------

SQLiteStmtAttrsVertEnumerator::NamedAttribute SQLiteStmtAttrsVertEnumerator::getNext()
{
    if (!_isFetched)
        throw std::range_error("Enumeration has no more elements.");

    std::string aId;
    SQLiteLog::Attribute a;
    if (!SQLiteLog::extractAttributeVert(_stmt, aId, a, _idNum, _valNum, _typeNum))
        throw LdopaException("Error when extracting log attribute: bad data.");

    // атрибут извлекся, переходим на будущее к следующему
    _isFetched = _stmt->fetch();

    // и возвращаем парочку
    return std::make_pair(aId, a);
}

//------------------------------------------------------------------------------

void SQLiteStmtAttrsVertEnumerator::reset()
{
    _stmt->reset();
    _isFetched = _stmt->fetch();
}

//------------------------------------------------------------------------------


//==============================================================================
// class SQLiteStmtAttrsHorEnumerator
//==============================================================================


SQLiteStmtAttrsHorEnumerator::SQLiteStmtAttrsHorEnumerator(SQLiteStmt* stmt,
    bool mngStLifet /*= true*/)
    : _stmt(stmt)
    , _mngStLifetime(mngStLifet)
    , _curAttr(0)
{

    // если не удается сфетчить тот единственный нужный раз, то перечисление пусто
    if (!_stmt->fetch())
        _attrbsNum = 0;

    // если сфетчили, число атрибутов определяем числом колонок в запросе
    _attrbsNum = _stmt->getColsNum();


}

//------------------------------------------------------------------------------

SQLiteStmtAttrsHorEnumerator::~SQLiteStmtAttrsHorEnumerator()
{
    // если передали управление памятью на stmt, удалим его
    if (_mngStLifetime)
        delete _stmt;
}

//------------------------------------------------------------------------------

bool SQLiteStmtAttrsHorEnumerator::hasNext() const
{
    return (_curAttr < _attrbsNum);
}

//------------------------------------------------------------------------------

SQLiteStmtAttrsHorEnumerator::NamedAttribute SQLiteStmtAttrsHorEnumerator::getNext()
{
    // проверяем невыход
    if (_curAttr >= _attrbsNum)
        throw std::range_error("Enumeration has no more elements.");

    std::string aId;
    SQLiteLog::Attribute a;

    if(!SQLiteLog::extractAttributeHor(_stmt, 
        _curAttr++,                            // используем и инкрементируем!
        a, &aId))
        throw LdopaException("Error when extracting log attribute: bad data.");

    return std::make_pair(aId, a);
}

//------------------------------------------------------------------------------

void SQLiteStmtAttrsHorEnumerator::reset()
{
    // здесь сброс простой: текущая излекаемая колонка становится нулевой
    _curAttr = 0;
}

//------------------------------------------------------------------------------

}}} // namespace xi::ldopa::eventlog 

