///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Define a classes for exporting TS as GraphViz definitions
///
///////////////////////////////////////////////////////////////////////////////


#ifndef XI_LDOPA_TRSS_EVENTLOGTSGRVIZEXPORTER_H_
#define XI_LDOPA_TRSS_EVENTLOGTSGRVIZEXPORTER_H_

#pragma once


#include <iostream>
#include <fstream>

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/graphs/algos/grviz/graph_dot_writer.h"
#include "xi/ldopa/ts/models/obsolete1/eventlogts.h"


namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //


/** \brief Graph Visitor object for EventLog TS
 *
 *  Implements methods for iterating elements of underlying graph and.
 */
class LDOPA_API EventLogTsGrVisitor : public IGraphVisitor
{
public:
    //-----<>-----
    EventLogTsGrVisitor(EventLogTs* ts) : _ts(ts), _flStateActLabel(true) {}

public:
    //-----<IGraphVisitor:: implementations>-----
    virtual std::string visitGraph(ParValStrCollection& attrList) override;
    virtual bool visitVertex(std::string& vid, ParValStrCollection& attrList) override;    
    virtual void resetVertexEnum() override;
    virtual bool visitEdge(StrStrPair& verts, ParValStrCollection& attrList) override;
    virtual void resetEdgeEnum() override;

protected:
    /** \brief Typed internal version of visitVertex for a state of the TS */
    void visitState(EventLogTs::State st, std::string& stid, ParValStrCollection& attrList);
    
    /** \brief Typed internal version of visitEdge for a transition of the TS */
    void visitTransition(EventLogTs::Transition tr, IGraphVisitor::StrStrPair& verts,
        ParValStrCollection& attrList);
public:
    /** \brief For a given state \a st creates a string representation of its ID and puts it into \a stid */
    static void createStateId(EventLogTs::State st, std::string& stid);
public:
    bool getFlStateActLabel() const { return _flStateActLabel; }
    void setFlStateActLabel(bool val) { _flStateActLabel = val; }

protected:
    /** \brief Stores a ptr to a TS for exporting */
    EventLogTs* _ts;

    /** \brief A pair of iterators for iterating all graph vertices 
     *  resetVertexEnum() method should be invoked for proper initialization of the iterators pair
     */
    BaseListBGraph::VertexIteratorsPair _verticesIter;

    /** \brief A pair of iterators for iterating all graph edges 
     *  resetEdgeEnum() method should be invoked for proper initialization of the iterators pair
     */
    BaseListBGraph::EdgeIteratorsPair _edgesIter;


    /** \brief Determines if a label containing all activities should be generated 
     *  instead of simple vertix number
     *
     *  \TODO вынести в виде коллекции флагов
     */
    bool _flStateActLabel;

}; // class EventLogTsGrVisitor




}}}} // namespace xi::ldopa::ts::obsolete1


#endif // XI_LDOPA_TRSS_EVENTLOGTSGRVIZEXPORTER_H_
