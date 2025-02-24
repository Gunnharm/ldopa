////////////////////////////////////////////////////////////////////////////////
// Module Name:  csvlog.h/cpp
// Authors:      Sergey Shershakov
// Version:      0.1.0
// Date:         23.02.2016
// Copyright (c) xidv.ru 2014–2016.
//
// This source is for internal use only — Restricted Distribution.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////


// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

// ldopa
#include "xi/ldopa/eventlog/obsolete1/csvlog.h"
#include "xi/ldopa/utils.h"


// xilib
#include "xi/strutils/substr.h"

// std
#include <algorithm>    // std::sort

namespace xi { namespace ldopa { namespace eventlog { namespace obsolete1 {;   //


//==============================================================================
// class CSVLogCaseDescriptor
//==============================================================================

CSVLogTrace::CSVLogTrace(CSVLogTraces* ownerTracer, IEventLog::CaseID caseId)
    : //_eventNum(0),
    _ownerTracer(ownerTracer),
    _caseID(caseId),
    _prepared(true)
{
}

//------------------------------------------------------------------------------

CSVLogTrace::~CSVLogTrace()
{
    deleteEvents();
}

//------------------------------------------------------------------------------

void CSVLogTrace::deleteEvents()
{
    for (IEvent* ev : _events)
        delete ev;

    _events.clear();
}


//------------------------------------------------------------------------------

void CSVLogTrace::prepareInternal()
{
    // without any extra checks
    
    // sorts events
    std::sort(_events.begin(), _events.end(), compareEvents);
    _prepared = true;
}

//------------------------------------------------------------------------------

ITraces* CSVLogTrace::getOwnerTraces()
{
    return _ownerTracer;
}

//------------------------------------------------------------------------------


ITrace::IEventEnumerator* CSVLogTrace::enumerateEvents()
{
    if (!_prepared)
        prepareInternal();
    return new MyEnumerator(_events.begin(), _events.end());

}

//------------------------------------------------------------------------------

ITrace::IEventConstEnumerator* CSVLogTrace::enumerateEventsConst() const
{
    if (!_prepared)
        throw LdopaException("Can't enumerate unprepared sequence as well as prepare it. " \
            "Prepare should be invoked in advance");

    return new MyConstEnumerator(_events.begin(), _events.end());
}

//------------------------------------------------------------------------------

size_t CSVLogTrace::getEventsNum()
{
    return getEventsNumLocal();
}

//------------------------------------------------------------------------------

IEvent* CSVLogTrace::insertEvent(IEvent* ev)
{
    return insertEventLocal(ev);
}

//------------------------------------------------------------------------------

IEvent* CSVLogTrace::insertEventLocal(IEvent* ev)
{

    // simply add new event into the vector of events and invalidate prepare state
    _events.push_back(ev);
    _prepared = false;


    return ev;
}

//------------------------------------------------------------------------------

bool CSVLogTrace::compareEvents(IEvent* lhv, IEvent* rhv)
{
    return lhv->compare(rhv);
}

//------------------------------------------------------------------------------



//==============================================================================
// class CSVLogTraces
//==============================================================================
CSVLogTraces::CSVLogTraces(CSVLog* owner)
    : _owner(owner),
    //_numOfTraces(SIZE_MAX),         // special value saying that the real number is not counted yet
    _attrNumCase(ATTR_NSET),        // attributes isn't mapped
    _attrNumAct(ATTR_NSET),
    _attrNumTmst(ATTR_NSET)
{

}

//------------------------------------------------------------------------------

CSVLogTraces::~CSVLogTraces()
{
    // frees all dynamically created traces
    clearTraces();
}


//------------------------------------------------------------------------------

void CSVLogTraces::clearTraces()
{
    //for (auto tr : _case2TraceMaps)
    for (Case2TraceMapVal tr : _case2TraceMaps)
        delete tr.second;
        
    _case2TraceMaps.clear();    
}

//------------------------------------------------------------------------------


IEventLog* CSVLogTraces::getOwnerLog()
{
    return _owner;
}

//------------------------------------------------------------------------------

ITraces::ITraceEnumerator* CSVLogTraces::enumerateTraces()
{
    return new MyEnumerator(_case2TraceMaps.begin(), _case2TraceMaps.end());
}

//------------------------------------------------------------------------------

ITraces::ITraceConstEnumerator* CSVLogTraces::enumerateTracesConst() const
{
    return new MyConstEnumerator(_case2TraceMaps.begin(), _case2TraceMaps.end());
}

//------------------------------------------------------------------------------

size_t CSVLogTraces::getNumOfTraces()
{    
    return _case2TraceMaps.size();
}

//------------------------------------------------------------------------------

//IEvent* CSVLogTraces::insertEvent(IEventLog::CaseID caseId, IEvent* ev)
ITrace* CSVLogTraces::insertEvent(IEventLog::CaseID caseId, IEvent* ev)
{
    return insertEventLocal(caseId, ev);
}

//------------------------------------------------------------------------------

//IEvent* CSVLogTraces::insertEventLocal(IEventLog::CaseID caseId, IEvent* ev)
ITrace* CSVLogTraces::insertEventLocal(IEventLog::CaseID caseId, IEvent* ev)
{
    // directly ask the map to give existing trace or to add a new one withh nullptr
    // we can trust on default value for pointers that is nullptr:
    // http://stackoverflow.com/questions/4523959/stdmap-default-value-for-build-in-type
    CSVLogTrace* trace = _case2TraceMaps[caseId];

    // if a need, creates a new trace object
    if (!trace)
    {
        trace = new CSVLogTrace(this, caseId);
        _case2TraceMaps[caseId] = trace;
    }

    ev->setOwnerTrace(trace);               // new: assign an owner trace to the event
    trace->insertEventLocal(ev);
    
    return trace;
}

//------------------------------------------------------------------------------

ITrace* CSVLogTraces::getTrace(IEventLog::CaseID caseId)
{
    Case2TraceMapConstIter caseIt = _case2TraceMaps.find(caseId);
    if (caseIt == _case2TraceMaps.end())
        return nullptr;

    return caseIt->second;
}



//------------------------------------------------------------------------------

void CSVLogTraces::mapMainAttributes(short nCase /*= ATTR_NSET*/, 
    short nAct /*= ATTR_NSET*/, short nTmst /*= ATTR_NSET*/)
{
    if (nCase != _attrNumCase)
    {
        _attrNumCase = nCase;
        
        //if (_owner->isOpen())
        //    markUpLog();
        _owner->markUpLog();
    }

    // TODO: with other attrs, it also can be something special
    _attrNumAct = nAct;
    _attrNumTmst = nTmst;
}


//==============================================================================
// class CSVLog
//==============================================================================


//---<Const definitions>---

const CSVLog::StdStrChar CSVLog::DEF_SEP_CHAR = ';';        // TCHAR is needed here!

//------------------------------------------------------------------------------


CSVLog::CSVLog(std::istream* istr, IStrOffsEventCreator* evcr) 
    : _istr(istr),
    _ifstr(nullptr),
    _settings(DEF_SETTINGS),
    _sepChar(DEF_SEP_CHAR),
    _ready(false),
    _evCreator(evcr),
    _evAttrCacher(nullptr)
{
    if (evcr)
        evcr->setOwnerLog(this);

    createAssociatedObjects();
    markUpLog();
}


//------------------------------------------------------------------------------

CSVLog::CSVLog(const std::string& fileName, IStrOffsEventCreator* evcr)
    : _istr(nullptr),
    _ifstr(nullptr),
    _fileName(fileName),
    _settings(DEF_SETTINGS),
    _sepChar(DEF_SEP_CHAR),
    _ready(false),
    _evCreator(evcr),
    _evAttrCacher(nullptr)
{
    if (evcr)
        evcr->setOwnerLog(this);

    createAssociatedObjects();
}

//------------------------------------------------------------------------------

CSVLog::CSVLog(IStrOffsEventCreator* evcr)
    : CSVLog("", evcr)//(const char*)nullptr)                  // delegating
{
}

//------------------------------------------------------------------------------


CSVLog::~CSVLog()
{    
    //closeIstrFile();
    close();
    
    destroyAssociatedObjects();
}

//------------------------------------------------------------------------------

bool CSVLog::open()
{
    // if a log is already open, do nothing
    if (isOpen())
        return true;

    // if a log isn't open, then no istr is associated, so we need to create a new one
    // based on input file stream
    createIstrFile();

    bool openOk = isOpenInt();
    if (openOk)
        markUpLog();
    
    return openOk; // isOpen();
}

//------------------------------------------------------------------------------

void CSVLog::close()
{
    // if no log open, do nothing
    if (!isOpen())
        return;

    demarkLogInt();
    closeIstrFile();
}

//------------------------------------------------------------------------------

bool CSVLog::isOpen()
{
    // log is open iff a valid input string is associated with
    return isOpenInt();
}


//------------------------------------------------------------------------------

ITraces* CSVLog::getTraces()
{
    return _traces;
}


//------------------------------------------------------------------------------

xi::strutils::SetStrPool* CSVLog::getActivitiesPool()
{
    return &_activitiesPool;
}

//------------------------------------------------------------------------------

void CSVLog::setFileName(const std::string& fn)
{
    if (isOpenInt())
        throw LdopaException("Can't change a file name while a log is open.");

    _fileName = fn;
}


//------------------------------------------------------------------------------

void CSVLog::createIstrFile()
{
    if (_ifstr)
        throw LdopaException("Cannot create second ifstream with active one.");

    _ifstr = new std::ifstream(_fileName);
    if (_ifstr->fail())
    {
        delete _ifstr;
        _ifstr = nullptr;

        throw LdopaException::f("File %s cannot be open", _fileName.c_str());
    }
    
    // if it's opened successfully, connect f-stream with i-stream
    _istr = _ifstr;
}


//------------------------------------------------------------------------------


void CSVLog::closeIstrFile()
{
    if (!_ifstr)
        return;             // just do nothing, hence it is alaready ok

    delete _ifstr;
    _ifstr = nullptr;
    _istr = nullptr;        // as it is rigidly connected with a file stream
}




//------------------------------------------------------------------------------
void CSVLog::initSettings()
{
}

//------------------------------------------------------------------------------

void CSVLog::createAssociatedObjects()
{
    _traces = new CSVLogTraces(this);
}


//------------------------------------------------------------------------------

void CSVLog::destroyAssociatedObjects()
{
    delete _traces;
    _traces = nullptr;
}



//------------------------------------------------------------------------------

void CSVLog::checkForOpenWithExc()
{
    if (!isOpenInt())
        throw LdopaException("Log is not open");
}

//------------------------------------------------------------------------------

void CSVLog::markUpLog()
{
    if (!_istr || !_traces->isAttrsMappingOk())
        return;
    
    markUpLogInt();
}


//------------------------------------------------------------------------------

void CSVLog::markUpLogInt()
{

    // if no attribute number for case id is set...
    if (_traces->_attrNumCase == -1)
        throw LdopaException("No attribute number for a CaseID is set");


    _istr->seekg(0);
    std::streamoff newOffs = _istr->tellg();   //0;
    std::streamoff offs = newOffs;         // previous offset

    bool firstLine = true;
    size_t lineCount = 0;

    // iterates all lines
    std::string line;
    while (std::getline(*_istr, line))
    {
        newOffs = _istr->tellg();
        ++lineCount;

        // here we ignore the first line... may something should be done with a header?
        if (isFirstLineAsHeader() && firstLine)
        {
            offs = newOffs;
            firstLine = false;
            extractAttrHeaders(line);
            continue;
        }

        IStrOffsEventCreator* evCreator = getEventCreator();
        if (!evCreator)
            throw LdopaException("No event creator object is set");
            //continue;
                
        //std::string caseID;
        strutils::VectorOfPairOfIndices attrPairs;
        IEventLog::CaseID caseId;
        IEvent* newEv = evCreator->createEvent(line, offs, caseId, &attrPairs);

        // inserts new event to an appropriate trace (regardless of the internal representation)
        ITrace* tr = _traces->insertEvent(caseId, newEv);


        // if a attribute cacher is assigned, call it
        if (_evAttrCacher)
            _evAttrCacher->parseEventLine(newEv, tr, caseId, line, attrPairs, offs);

        offs = newOffs;

    }

    _ready = true;
}

//------------------------------------------------------------------------------


void CSVLog::demarkLogInt()
{
    _ready = false;
    _traces->clearTraces();
    _attrHeaders.clear();

    // if there is an attribute extractor:
    if (_evAttrCacher)
        _evAttrCacher->invalidateAllEvents();
}

//------------------------------------------------------------------------------

void CSVLog::extractAttrHeaders(const std::string& str)
{
    // protected internal method with no extra checks
    strutils::splitStr(str, _sepChar, _attrHeaders);
}

//------------------------------------------------------------------------------

void CSVLog::setEventCreator(IStrOffsEventCreator* evcr)
{
    _evCreator = evcr;
    if (evcr)
        evcr->setOwnerLog(this);
}

//------------------------------------------------------------------------------

bool CSVLog::readCsvLine(std::streamoff ofs, std::string& str)
{
    if (!_istr)
        throw LdopaException("Can't read a line: stream is not open");

    // perform soft reset here... may be there is a need for a more "clear" solution
    _istr->clear();

    // positioning
    if (!_istr->seekg(ofs))
        return false;                           // can't event doing positioning
        
    // let it be explicit coversion!
    return !(std::getline(*_istr, str).fail());
}

//------------------------------------------------------------------------------

bool CSVLog::extractAttribute(const std::string& line, short caseAttrNumb,
    std::string& attrStr)
{
    using namespace xi::strutils;
    
    VectorOfPairOfIndices prs;

    size_t attrsNum = splitStr(line, getSepChar(), prs);
    if ((size_t)caseAttrNumb >= attrsNum)
        return false;

    //std::string attrName = 
    attrStr = extractSubstr(line, prs[caseAttrNumb]);

    return true;
}


}}}} // namespace xi::ldopa::eventlog::obsolete1

