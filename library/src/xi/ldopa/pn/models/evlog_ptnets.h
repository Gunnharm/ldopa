////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri Nets Library: Eventlog Petri nets.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      15.08.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Some Petri net model types specified for Event logs.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_MODELS_EVLOG_PTNETS_H_
#define XI_LDOPA_PN_MODELS_EVLOG_PTNETS_H_

#pragma once

// std
// boost

// ldopa
#include "xi/ldopa/eventlog/eventlog.h"
#include "base_ptnet.h"     // same catalog
#include "xi/ldopa/utils.h"

// std
#include <map>              // нужно для помеченной, возможно, вынести в отд. файл

namespace xi { namespace ldopa { namespace pn {;   //

//==============================================================================
// class EventLogPetriNet
//==============================================================================

/** \brief Petri net with Attributes labels on all component types (poss, transs, arcs)
 *  provided by associated map.
 *
 *  TODO: to move into special module
 */
template <
    typename TVertData = boost::no_property,
    typename TArcData = boost::no_property
>
class EventLogPetriNet : public BasePetriNet < TVertData, TArcData >
{
public:
    //-----<Types>-----
    typedef eventlog::IEventLog::Attribute Attribute;           ///< EventLog Attribute type.
    typedef BasePetriNet<TVertData, TArcData> Base;             ///< Alias for the base class.
    typedef std::map<PnVertex, Attribute> VertexAttributeMap;   ///< Vertices-to-Attributes mapping.

public:

    //---<Addition methods for creating PN's structure>---

    using Base::addPosition;
    using Base::addTransition;


    /// Adds a new position labeled by the attribute \a a.
    Position addPosition(const Attribute& a)
    {
        Position p = Base::addPosition();
        _vertLabels[p] = a;

        return p;
    }

    /// Adds a new transition labeled by the attribute \a a.
    Transition addTransition(const Attribute& a)
    {
        Transition t = Base::addTransition();
        _vertLabels[t] = a;

        return t;
    }

    /** \brief Returns a ptr to a string lable of the goven vertex \a v or null if no label exists. */
    const Attribute* getAttribute(const PnVertex& v) const
    {
        VertexAttributeMap::const_iterator it = _vertLabels.find(v);
        if (it == _vertLabels.end())
            return nullptr;

        // иначе найден — вернем указатель на персистентный объект!
        return &(it->second);
    }

protected:
    /// Stores string labels for poss and transs.
    VertexAttributeMap _vertLabels;
}; // class EventLogPetriNet


}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_PN_MODELS_EVLOG_PTNETS_H_
