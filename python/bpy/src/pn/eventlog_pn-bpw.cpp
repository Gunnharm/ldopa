////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for event log PNs Implementations.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      28.08.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

// ldopa.log
#include "xi/ldopa/pn/models/gen_petrinet.h"
#include "xi/ldopa/pn/models/base_ptnet.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"

namespace bp = boost::python;
//==============================================================================

/** \brief Converters of PNs elelement (such as pos, trans, arc) to python objects. */
struct EvLogPnEls2PyObj
{
    typedef xi::ldopa::pn::GenPetriNet<> GenPetriNet;
    typedef GenPetriNet::PnVertex PnVertex;

    /** \brief Converter for PnVertex. */
    static PyObject* convert(PnVertex v)
    {
        // знаем, что в нашем случае (Vertex == void*)
        // приведем к виду шестнадцатеричного указателя
        bp::object pyObj = bp::object(uintptr_t(v.getVertex()));           // <cstdint>
        return bp::incref(bp::make_tuple(pyObj).ptr());
    }
};

//==============================================================================

/** \brief Wrappers for eventlog PN related classes. */
struct EvLogPNs_PyWrappers
{
    typedef xi::ldopa::pn::GenPetriNet<> GenPetriNet;
    typedef xi::ldopa::pn::GenPetriNetMarking<>  GenPetriNetMarking;
    typedef xi::ldopa::pn::BasePetriNet<> BasePetriNet;
    typedef xi::ldopa::pn::EventLogPetriNet<> EventLogPetriNet;
    //-------------------------------------------------------------------------

    static void exportClasses()
    {
        // class GenPetriNet
        bp::class_<GenPetriNet>("GenPetriNet")
            // properties
            .add_property("verts_num",          &GenPetriNet::getVerticesNum)
            .add_property("poss_num",           &GenPetriNet::getPositionsNum)
            .add_property("transs_num",         &GenPetriNet::getTransitionsNum)
            .add_property("arcs_num",           &GenPetriNet::getArcsNum)
        ; // class_<GenPetriNet>
        
        // class GenPetriNet
        bp::class_<GenPetriNetMarking>("GenPetriNetMarking")
        ; // class_<GenPetriNetMarking>

        // class BasePetriNet
        bp::class_<BasePetriNet, bp::bases<GenPetriNet>>("BasePetriNet")
        ; // class_<BasePetriNet>

        // class BasePetriNet
        bp::class_<EventLogPetriNet, bp::bases<BasePetriNet>>("EventLogPetriNet")
        ; // class_<EventLogPetriNet>
    } // exportClasses()
}; // struct EvLogPNs_PyWrapper


// export function for EventLog PNs classes
void export_EvLogPNs()
{
    //--=регистрируем конвертеры=-    
    bp::to_python_converter <EvLogPnEls2PyObj::PnVertex, EvLogPnEls2PyObj>();

    //--=регистрируем обертки для классов=-
    EvLogPNs_PyWrappers::exportClasses();
}
