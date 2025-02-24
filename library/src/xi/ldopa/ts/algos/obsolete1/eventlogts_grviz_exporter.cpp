// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/obsolete1/eventlogts_grviz_exporter.h"

// ldopa
#include "xi/ldopa/utils.h"

// boost
#include <boost/graph/graphviz.hpp>

#pragma warning(disable:4996)


namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //



//==============================================================================
// class EventLogTsGrVisitor
//==============================================================================


std::string EventLogTsGrVisitor::visitGraph(ParValStrCollection& attrList)
{
    // simply returns a name of TS
    return _ts->getName();
}

//------------------------------------------------------------------------------

bool EventLogTsGrVisitor::visitVertex(std::string& vid, ParValStrCollection& attrList)
{
    // if all vertices done
    if (_verticesIter.first == _verticesIter.second)
        return false;

    EventLogTs::State st = *_verticesIter.first;
    visitState(st, vid, attrList);

    ++_verticesIter.first;

    return true;
}

//------------------------------------------------------------------------------

void EventLogTsGrVisitor::visitState(EventLogTs::State st, std::string& stid,
    ParValStrCollection& attrList)
{
    // ID
    createStateId(st, stid);

    // do label only if an appropriate flag is set
    if (!_flStateActLabel)
        return;

    // label
    std::string lbl = "\"" + _ts->makeStateLabel(st) + "\"";
    attrList.push_back(std::make_pair("label", lbl));      

    // accepting flag
    EventLogTs::StateFlags fl;
    if (_ts->getStateFlags(st, fl))
    {
        // if accepting flag is set
        if (fl.getAccepting() == EventLogTs::StateFlags::triTrue)
        {
            attrList.push_back(std::make_pair("peripheries", "2"));
        }
    }
}

//------------------------------------------------------------------------------

void EventLogTsGrVisitor::createStateId(EventLogTs::State st, std::string& stid)
{
    char addrBuf[11];
    sprintf(addrBuf, "x%x", (void*)st);
    stid = addrBuf;
}


//------------------------------------------------------------------------------

void EventLogTsGrVisitor::resetVertexEnum()
{
    _verticesIter = _ts->getGraph().getVertices();
}

//------------------------------------------------------------------------------

bool EventLogTsGrVisitor::visitEdge(StrStrPair& verts, ParValStrCollection& attrList)
{
    // if all vertices done
    if (_edgesIter.first == _edgesIter.second)
        return false;

    EventLogTs::Transition tr = *_edgesIter.first;
    visitTransition(tr, verts, attrList);

    ++_edgesIter.first;

    return true;
}

//------------------------------------------------------------------------------

void EventLogTsGrVisitor::visitTransition(EventLogTs::Transition tr, 
    StrStrPair& verts, ParValStrCollection& attrList)
{
    EventLogTs::State srcs = _ts->getGraph().getSrcVertex(tr);
    EventLogTs::State dsts = _ts->getGraph().getDstVertex(tr);

    createStateId(srcs, verts.first);
    createStateId(dsts, verts.second);

    // label
    const std::string* trAct = _ts->getTransActivity(tr);
    if (trAct)    
    {
        std::string lbl = "\"" + *trAct + "\"";
        attrList.push_back(std::make_pair("label", lbl));
    }

}

//------------------------------------------------------------------------------

void EventLogTsGrVisitor::resetEdgeEnum()
{
    _edgesIter = _ts->getGraph().getEdges();
}

//------------------------------------------------------------------------------

}}}} // namespace xi::ldopa::ts::obsolete1
