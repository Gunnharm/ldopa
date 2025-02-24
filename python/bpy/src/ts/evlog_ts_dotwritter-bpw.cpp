////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for TS DOT writter.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      12.09.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// boost
#include <boost/python.hpp>
// ldopa.log

#include "xi/ldopa/ts/algos/grviz/evlog_ts_red_dotwriter.h"

namespace bp = boost::python;

/** \brief Wrappers for PnRegSynthesizer class. */
struct EventLogTSDotWriter_PyWrappers
{
    typedef xi::ldopa::ts::EvLogTSWithFreqs EvLogTSWithFreqs;

    // синтезатор
    typedef xi::ldopa::ts::EvLogTSWithFreqsDotWriter EvLogTSWithFreqsDotWriter;

    static void exportClasses()
    {
        // class EvLogTSWithFreqsDotWriter
        bp::class_<EvLogTSWithFreqsDotWriter, boost::noncopyable>("EvLogTSWithFreqsDotWriter")
            
            .def("write_ts", &EvLogTSWithFreqsDotWriter::write)

        ; // class_<EvLogTSWithFreqsDotWriter
    }
}; // struct EventLogTSDotWriter_PyWrappers


// export function for EventLog PN DOT Writter python definitions
void export_EventLogTSDotWriter()
{
    //--=регистрируем обертки для классов=-
    EventLogTSDotWriter_PyWrappers::exportClasses();
}
