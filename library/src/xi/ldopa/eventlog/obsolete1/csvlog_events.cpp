// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

// ldopa
#include "xi/ldopa/eventlog/obsolete1/csvlog_events.h"
#include "xi/ldopa/utils.h"


// xilib
#include "xi/strutils/substr.h"

// std
//#include <sstream>

namespace xi { namespace ldopa { namespace eventlog { namespace obsolete1 {;   //


//==============================================================================
// class SimpleCSVLogEvent
//==============================================================================

SimpleCSVLogEvent::SimpleCSVLogEvent()
    : _attrActivity(nullptr)
{
}

//------------------------------------------------------------------------------

SimpleCSVLogEvent::SimpleCSVLogEvent(const std::string* attrAct, std::streamoff ofs,
                    int timest)
    : //_caseID(caseID),
    _ownerTr(nullptr),
    _attrActivity(attrAct),
    _evStreamOffset(ofs),
    _timeSt(timest)
{

}

//------------------------------------------------------------------------------

std::string SimpleCSVLogEvent::getActivityName()
{
    return *_attrActivity;
}

//------------------------------------------------------------------------------

void SimpleCSVLogEvent::setOwnerTrace(ITrace* tr)
{
    if (_ownerTr)
        throw LdopaException("Trace has aready assigned to a trace");

    _ownerTr = tr;
}

//------------------------------------------------------------------------------

ITrace* SimpleCSVLogEvent::getOwnerTrace()
{
    return getOwnerTraceLocal();
}

//------------------------------------------------------------------------------

bool SimpleCSVLogEvent::getAttrAsStr(short aNum, std::string& str)
{
    if (!_ownerTr)
        throw LdopaException("Can't get an attribute: orphan event (no trace assigned)");

    // annoying sequence of downcasts...
    ITraces* traces = dynamic_cast<CSVLogTrace*>(_ownerTr)->getOwnerTraces();
    CSVLog* ownerCsvLog = dynamic_cast<CSVLog*>(traces->getOwnerLog());

    // reads a line corresponded to the event
    std::string logLine, attrStr;
    ownerCsvLog->readCsvLine(_evStreamOffset, logLine);
    
    return ownerCsvLog->extractAttribute(logLine, aNum, str);
}

//------------------------------------------------------------------------------

bool SimpleCSVLogEvent::compare(IEvent* rhv)
{
    // can only compare with the same downcasted type
    SimpleCSVLogEvent* rEvent = dynamic_cast<SimpleCSVLogEvent*>(rhv);
    if (!rEvent)
        throw LdopaException("Incomparable types");

    // the core of comparision
    return (_timeSt < rEvent->_timeSt);
}

//==============================================================================
// class SimpleCSVLogEventCreator
//==============================================================================

SimpleCSVLogEventCreator::SimpleCSVLogEventCreator() 
{
}

//------------------------------------------------------------------------------

void SimpleCSVLogEventCreator::setOwnerLog(CSVLog* log)
{
    _ownerLog = log;
}

//------------------------------------------------------------------------------

IEvent* SimpleCSVLogEventCreator::createEvent(const std::string str, 
    std::streamoff ofs, IEventLog::CaseID& caseID, 
    strutils::VectorOfPairOfIndices* attrPairs)//std::string& caseID)
{
    using namespace xi::strutils;

    if (!_ownerLog)
        throw LdopaException("Log-owner is not set");

    CSVLogTraces* traces = _ownerLog->getTracesLocal();

    // splitting string to a set of attributes

    VectorOfPairOfIndices* prs = attrPairs ? attrPairs : new VectorOfPairOfIndices();
    size_t attrsNum = splitStr(str, _ownerLog->getSepChar(), *prs);

    // activity attribute
    short actNum = traces->getAttrNumAct();
    if (actNum == CSVLogTraces::ATTR_NSET || (size_t)actNum >= attrsNum)
        throw LdopaException("Wrong `activity' attribute number");
    
    const std::string* actStr = _ownerLog->getActivitiesPoolLocal()[extractSubstr(str, (*prs)[actNum])];


    // case attribute
    short caseNum = traces->getAttrNumCase();
    if (caseNum == CSVLogTraces::ATTR_NSET || (size_t)caseNum >= attrsNum)
        throw LdopaException("Wrong `case' attribute number");

    // upd: now we expect long long
    caseID = (IEventLog::CaseID)std::stoull(extractSubstr(str, (*prs)[caseNum]));
    //caseID = std::stoul(extractSubstr(str, (*prs)[caseNum]));


    // timestamp attribute: use very simple solution here
    short tmstNum = traces->getAttrNumTmst();
    if (tmstNum == CSVLogTraces::ATTR_NSET || (size_t)tmstNum >= attrsNum)
        throw LdopaException("Wrong `timestamp' attribute number");
    int timest = std::stoi(extractSubstr(str, (*prs)[tmstNum]));


    // creating a new event
    SimpleCSVLogEvent* ev = new SimpleCSVLogEvent(actStr, ofs, timest);
    
    // if no vector of pairs has been passed, delete local one
    if (!attrPairs)
        delete prs;

    
    return ev;
}

//------------------------------------------------------------------------------


}}}} // namespace xi::ldopa::eventlog::obsolete1

