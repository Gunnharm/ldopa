////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Common aggregator for eventlog library.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      27.02.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <boost/python.hpp>

// export methods
void export_SQLiteLog();
void export_EvLogTSs();
void export_TsFBuilder();
void export_TsReducers();
void export_TsMetricsCalc();
void export_EvLogPNs();
void export_PnRegsSynth();
void export_EventLogPetriNetDotWriter();
void export_EventLogTSDotWriter();

// о модулях (см. как сделать модуль с подчеркиванием для миксинга питона и ++):
// http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/tutorial/tutorial/techniques.html#tutorial.techniques.creating_packages
BOOST_PYTHON_MODULE(eventlog)
{
    // Transition systems
    export_SQLiteLog();
    export_EvLogTSs();          // вынести в отдельный модуль
    export_TsFBuilder();        // и этот туда же
    export_TsReducers();        // +1
    export_TsMetricsCalc();     // +1
    export_EventLogTSDotWriter();

    // Petri nets
    export_EvLogPNs();          // модели
    export_PnRegsSynth();       // синтезатор регионами
    export_EventLogPetriNetDotWriter(); // записыватель graph viz-а
}