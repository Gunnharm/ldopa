////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for the TS metrics calculator.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.03.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// boost
#include <boost/python.hpp>

// ldopa
#include "xi/ldopa/ts/algos/ts_metrics_calc.h"

namespace bp = boost::python;

/** \brief Wrappers for TsMetricsCalc classes. */
struct TsMetricsCalc_PyWrappers
{
    typedef xi::ldopa::ts::EvLogTSWithFreqs EvLogTSWithFreqs;
    typedef xi::ldopa::ts::TsMetricsCalc TsMetricsCalc;
    typedef xi::ldopa::eventlog::IEventLog IEventLog;

    //-------------------------------------------------------------------------

    static void exportClasses()
    {
        // class TsMetricsCalc
        bp::class_<TsMetricsCalc, boost::noncopyable>("TsMetricsCalc", 
                    bp::init<IEventLog*, EvLogTSWithFreqs*>())
            .def("calc_simplicity",     &TsMetricsCalc::calcSimplicity)
            .def("calc_precision",      &TsMetricsCalc::calcPrecision)
            .def("calc_general",        &TsMetricsCalc::calcGeneralization)
        ; // bp::class_<TsMetricsCalc>
    }
}; // struct TsMetricsCalc_PyWrappers


// export function for TsMetricsCalc classes
void export_TsMetricsCalc()
{
    //--=регистрируем обертки для классов=-
    TsMetricsCalc_PyWrappers::exportClasses();
}
