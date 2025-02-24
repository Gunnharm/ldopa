// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/models/obsolete1/eventlogts.h"
#include "xi/ldopa/utils.h"

namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //

//==============================================================================
// class EventLogTs
//==============================================================================

EventLogTs::EventLogTs()
    : _manageActPoolInternal(false),
    _hasInitState(false)
{
    // its own str pool
    _activitiesPool = new xi::strutils::SetStrPool();
}

//------------------------------------------------------------------------------

EventLogTs::EventLogTs(xi::strutils::SetStrPool* actPool, bool copyActPool /*= true*/)
    : _hasInitState(false)
{
    _manageActPoolInternal = copyActPool;

    // independant (deep) copy
    if (_manageActPoolInternal)
    {
        _activitiesPool = new xi::strutils::SetStrPool(*actPool);
        return;
    }

    // shallow copy
    _activitiesPool = actPool;
}

//------------------------------------------------------------------------------

EventLogTs::~EventLogTs()
{
    // deletes the activity pool only if created it
    if (_manageActPoolInternal)
        delete _activitiesPool;
}

//------------------------------------------------------------------------------

EventLogTs::State EventLogTs::getOrAddNewStateById(const TsPtrStateId& id)
{
    // look in the map if an appropriate vertex is still existing
    StateIdVertexBimapStidConstIter it = _stid2verts.left.find(id);
    if (it != _stid2verts.left.end())               // found one
        return it->second;

    // no found
    return addNewStateInternal(id);
}

//------------------------------------------------------------------------------

EventLogTs::StateRetResult EventLogTs::getState(const TsPtrStateId& id)
{
    // look in the map if an appropriate vertex is still existing
    StateIdVertexBimapStidConstIter it = _stid2verts.left.find(id);
    if (it != _stid2verts.left.end())               // found one
        return std::make_pair(it->second, true);

    // no requested state
    return std::make_pair(State(), false);
}

//------------------------------------------------------------------------------

void EventLogTs::setInitState(State vert)
{
    // assume a given vertex should be also in _stid2verts, if no — an exception
    StateIdVertexBimapVertexConstIter it = _stid2verts.right.find(vert);
    if (it == _stid2verts.right.end())
        throw LdopaException("Given vertex doen't belong to the TS");

    _initState = vert;
    _hasInitState = true;
}

//------------------------------------------------------------------------------

EventLogTs::State EventLogTs::getOrAddNewInitState()
{
    // if no init state created yet, creates a new one with default (empty) state ID
    // if there are a state with default ID yet, an exception is thrown 
    if (!_hasInitState)
    {
        TsPtrStateId defStId;
        if (getState(defStId).second)
            throw LdopaException("State with default ID is already exsiting. Can't add one more");
        
        _initState = addNewStateInternal(defStId);
        _hasInitState = true;
    }
              
    return _initState;
}



//------------------------------------------------------------------------------

const std::string* EventLogTs::getTransActivity(Transition tr) const
{
    auto it = _transActivitiesMap.find(tr);
    if (it == _transActivitiesMap.end())
        return nullptr;

    return it->second;

}

//------------------------------------------------------------------------------

EventLogTs::Transition EventLogTs::addTransitionInternal(State from, State to,
    const std::string* symb)
{
    //BaseListBGraph::EdgeRetResult
    TransRetResult transR = _gr.addEdge(from, to);

    // result should be true
    if (!transR.second)
        throw LdopaException("Can't add a new edge unexpectedly");

    // map a new edge to a symbol
    _transActivitiesMap[transR.first] = symb;

    return transR.first;
}

//------------------------------------------------------------------------------

EventLogTs::Transition EventLogTs::getOrAddNewTransition(State from, State to, 
    const std::string* symb, bool changeSymbIfExists /*= gatpAddNew*/)
{
    //BaseListBGraph::EdgeRetResult
    TransRetResult trR = _gr.getEdge(from, to);
    if (trR.second)    // if an edge exists
    {
        // if symbol should be overwritten in any case
        if (changeSymbIfExists)
            _transActivitiesMap[trR.first] = symb;

        return trR.first;
    }

    Transition newTrans = addTransitionInternal(from, to, symb);

    return newTrans;
}


//------------------------------------------------------------------------------

EventLogTs::IntRetResult EventLogTs::getTransFreq(Transition tr) const
{
    auto res = _transFreqsMap.find(tr);
    if (res == _transFreqsMap.end())
        return std::make_pair(0, false);            // not found

    // found one
    return std::make_pair(res->second, true);
}

//------------------------------------------------------------------------------

bool EventLogTs::getStateFlags(State st, StateFlags& fl) const
{
    auto res = _stateFlags.find(st);
    if (res == _stateFlags.end())
        return false;

    fl = res->second;
    return true;
}

//------------------------------------------------------------------------------

const TsPtrStateId& EventLogTs::getStateId(State st) 
{
    auto rmap = _stid2verts.right;
 
    return rmap.at(st);                 // it's impossible here to use operator[] 
}

//------------------------------------------------------------------------------

std::string EventLogTs::makeStateLabel(State st)
{
    std::string lbl = "[";
    bool first = true;

    const TsPtrStateId& stId = getStateId(st);
    for (auto it = stId.getPtrs().begin(); it != stId.getPtrs().end(); ++it)
    {
        if (!first)
            lbl += ", ";
        else
            first = false;
        
        const std::string* st = (const std::string*)*it;
        lbl += (*st);
    }

    lbl += "]";

    return lbl;
}


//------------------------------------------------------------------------------

EventLogTs::State EventLogTs::addNewStateInternal(const TsPtrStateId& id)
{
    // do not check if a similar state is aready added

    //BaseListBGraph::Vertex 
     State v = _gr.addVertex();

    // add a pair to a bimap
    _stid2verts.insert( StateIdVertexBimapPair(id, v) );

    return v;
}

//------------------------------------------------------------------------------

}}}} // namespace xi::ldopa::ts::obsolete1
