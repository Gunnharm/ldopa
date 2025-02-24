////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for PN DOT writter.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      28.08.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// boost
#include <boost/python.hpp>
// ldopa.log

#include "xi/ldopa/pn/models/evlog_ptnets.h"
#include "xi/ldopa/pn/algos/grviz/evlog_ptnets_dotwriter.h"

namespace bp = boost::python;

/** \brief Wrappers for PnRegSynthesizer class. */
struct EventLogPetriNetDotWriter_PyWrappers
{
    typedef xi::ldopa::pn::EventLogPetriNet<> EventLogPetriNet;

    // DOT-записыватель
    typedef xi::ldopa::pn::EventLogPetriNetDotWriter EventLogPetriNetDotWriter;

    static void exportClasses()
    {
        // class EventLogPetriNetDotWriter
        bp::class_<EventLogPetriNetDotWriter, boost::noncopyable>("EventLogPetriNetDotWriter")
            
            .def("write_pn", &EventLogPetriNetDotWriter::writePn)

        ; // class_<EventLogPetriNetDotWriter
    }
}; // struct BaseEventLogTS_PyWrapper


// export function for EventLog PN DOT Writter python definitions
void export_EventLogPetriNetDotWriter()
{
    //--=регистрируем обертки для классов=-
    EventLogPetriNetDotWriter_PyWrappers::exportClasses();
}
