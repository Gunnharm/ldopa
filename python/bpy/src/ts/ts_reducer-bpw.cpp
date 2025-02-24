////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for TS condenser/reducer.
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

// ldopa.log
#include "xi/ldopa/ts/algos/freq_condenser.h"
#include "xi/ldopa/ts/algos/varws_ts_rebuilder.h"

namespace bp = boost::python;

/** \brief Wrappers for BaseEventLogTS class. */
struct TsReducers_PyWrappers
{
    typedef xi::ldopa::ts::ITsStateFuncVarWs ITsStateFuncVarWs;
    typedef xi::ldopa::ts::CondensedTsBuilder CondensedTsBuilder;
    typedef xi::ldopa::ts::VarWsTsBuilder VarWsTsBuilder;
    typedef xi::ldopa::ts::EvLogTSWithFreqs EvLogTSWithFreqs;
    
    typedef xi::ldopa::eventlog::IEventLog IEventLog;

    //-------------------------------------------------------------------------
    static void exportClasses()
    {
        // class CondensedTsBuilder
        bp::class_<CondensedTsBuilder, boost::noncopyable>("CondensedTsBuilder", 
                    bp::init<EvLogTSWithFreqs*>())
            .def("build",   &CondensedTsBuilder::build,  bp::return_internal_reference<>())
            .def("detach",  &CondensedTsBuilder::detach, bp::return_value_policy<bp::manage_new_object>())
            .add_property("elapsed", &CondensedTsBuilder::getElapsed)
        ; // bp::class_<CondensedTsBuilder>

        // class VarWsTsBuilder with a scope in it
        bp::scope in_VarWsTsBuilder = bp::class_<VarWsTsBuilder, boost::noncopyable>("VarWsTsBuilder",
            bp::init<IEventLog*, EvLogTSWithFreqs*, ITsStateFuncVarWs*>())
            .def("build",   &VarWsTsBuilder::build, bp::return_internal_reference<>())
            .def("detach",  &VarWsTsBuilder::detach, bp::return_value_policy<bp::manage_new_object>())
            .add_property("elapsed", &VarWsTsBuilder::getElapsed)
        ; // bp::class_<VarWsTsBuilder>
     
        // enum VarWsTsBuilder::ZeroSizeWndAction
        bp::enum_<VarWsTsBuilder::ZeroSizeWndAction>("Zsa")//("ZeroSizeWndAction")
            .value("spec_state",  VarWsTsBuilder::ZeroSizeWndAction::zsaSpecState)
            .value("set1",       VarWsTsBuilder::ZeroSizeWndAction::zsaSet1)
        ;
    }
}; // struct BaseEventLogTS_PyWrapper

// export function for EventLog TSs classes
void export_TsReducers()
{
    //--=регистрируем обертки для классов=-
    TsReducers_PyWrappers::exportClasses();
}
