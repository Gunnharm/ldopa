// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/obsolete1/eventlogts_builder.h"

namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //



//==============================================================================
// class TsBuilder
//==============================================================================



//------------------------------------------------------------------------------

EventLogFixWsTsBuilder::EventLogFixWsTsBuilder()
    : EventLogFixWsTsBuilder(nullptr)
{

}
 
//------------------------------------------------------------------------------

EventLogFixWsTsBuilder::EventLogFixWsTsBuilder(IEventLog* evLog)
    : _evLog(evLog),
    //_ws(ws),
    _resTs(nullptr),
    _resDetached(false)
{

}

//------------------------------------------------------------------------------

EventLogFixWsTsBuilder::~EventLogFixWsTsBuilder()
{
    // final clearing
    resetInternal();
}


//------------------------------------------------------------------------------

void EventLogFixWsTsBuilder::setLog(IEventLog* evLog)
{
    resetInternal();
    _evLog = evLog;
}




//------------------------------------------------------------------------------


EventLogTs* EventLogFixWsTsBuilder::build(WindowSize ws)
{
    // check for log
    if (!_evLog)
        throw LdopaException("Log is not set");

    // if log still has not been open, do it
    if (!_evLog->isOpen())
        _evLog->open();

    _ws = ws;

    // if previous results are still active, resets them
    if (_resTs)
        resetInternal();
   
    // simply creates an empty TS and copy a pool of activities as atr representation
    _resTs = new EventLogTs(_evLog->getActivitiesPool());
    
    // iterate all log's traces and process every trace    
    ITraceEnumShPtr enp(_evLog->getTraces()->enumerateTraces());
    //i = 0;
    while (enp->hasNext())
    {
        ITrace* tr = enp->getNext();
        processTrace(tr);
        //++i;
    }

    return _resTs;
}


//------------------------------------------------------------------------------

void EventLogFixWsTsBuilder::resetInternal()
{
    if (_resTs && !_resDetached)
        delete _resTs;
    _resTs = nullptr;
    _resDetached = false;
}


//------------------------------------------------------------------------------


void EventLogFixWsTsBuilder::processTrace(ITrace* tr)
{
    size_t traceSize = tr->getEventsNum();
    //for (size_t i = 0;)
    
    // pool of str activities
    strutils::SetStrPool& actvsPool = *_resTs->getActivitiesPool();
    
    // init state is processed separately    
    EventLogTs::State prevState = _resTs->getOrAddNewInitState();

    //
    //IEventEnumShPtr enp(tr->enumerateEvents());
    size_t i = 0;                                   // dealing with a number behind the margin
    //while (enp->hasNext())
    for (IEventEnumShPtr enp(tr->enumerateEvents()); enp->hasNext(); )
    {
        IEvent* ev = enp->getNext();

        // make state ID
        const std::string* actName = actvsPool[(*tr)[i++]->getActivityName()];
        TsPtrStateId stId = makeStateId(tr, _ws, i, actvsPool);
        BaseListBGraph::Vertex curState = _resTs->getOrAddNewStateById(stId);
        

        // add a new transition marked with a symbol
        addTransitionWithFreq(prevState, curState, actName, 1);

        // if it is a last state, set an accepting mark
        if (i == traceSize)
        {
            EventLogTs::StateFlags& fl = _resTs->getOrAddNewStateFlags(curState);
            fl.setAccepting(true);
        }


        // towards to the next state
        prevState = curState;
        //++i;                                      // done above
    } // while (enp->hasNext())

}

//------------------------------------------------------------------------------

void EventLogFixWsTsBuilder::addTransitionWithFreq(BaseListBGraph::Vertex from,
    BaseListBGraph::Vertex to, const std::string* actName, int freqToAdd)
{
    BaseListBGraph::Edge tr = _resTs->getOrAddNewTransition(from, to, actName);
    //int freq 
    EventLogTs::IntRetResult freqR = _resTs->getTransFreq(tr);
    if (freqR.second)        
        freqR.first += freqToAdd;       // if set previously
    else
        freqR.first = freqToAdd;        // initially 0 +

    _resTs->setTransFreq(tr, freqR.first);
}

//------------------------------------------------------------------------------

TsPtrStateId EventLogFixWsTsBuilder::makeStateId(ITrace* tr,
    WindowSize ws, WindowSize ind,  strutils::SetStrPool& actvsPool)
{

    TsPtrStateId id;
    WindowSize beg;

    if (ws == WS_INF)
        beg = 0;
    else
    {
        beg = ind - ws;
        if (beg < 0)
            beg = 0;
    }

    makeStateIdInternal(id, tr, beg, ind, actvsPool);

    return id;
}

//------------------------------------------------------------------------------


void EventLogFixWsTsBuilder::makeStateId(TsPtrStateId& id, ITrace* tr, 
    WindowSize beg, WindowSize end, strutils::SetStrPool& actvsPool)
{
    size_t evNum = tr->getEventsNum();

    if (beg > evNum)
        beg = evNum;
    if (beg < 0)
        beg = 0;

    if (end > evNum)
        end = evNum;
    if (end < 0)
        end = 0;

    makeStateIdInternal(id, tr, beg, end, actvsPool);

}

//------------------------------------------------------------------------------

void EventLogFixWsTsBuilder::makeStateIdInternal(TsPtrStateId& id, ITrace* tr, 
    WindowSize beg, WindowSize end,  strutils::SetStrPool& actvsPool)
{
    for (WindowSize i = beg; i < end; ++i)
    {        
        const std::string* actNamePtr = actvsPool[(*tr)[i]->getActivityName()];
        id.append(actNamePtr);
    }
}



}}}} // namespace xi::ldopa::ts::obsolete1
