////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for TS state-function builder.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.03.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

// ldopa.log
#include "xi/ldopa/ts/algos/ts_simple_builder.h"

namespace bp = boost::python;

/** \brief Wrappers for BaseEventLogTS class. */
struct TsFBuilder_PyWrappers
{
    typedef xi::ldopa::ts::ITsStateFunc ITsStateFunc;
    typedef xi::ldopa::ts::ITsStateFuncVarWs ITsStateFuncVarWs;
    typedef xi::ldopa::ts::PrefixStateFunc PrefixStateFunc;
    typedef xi::ldopa::ts::IStateIDsPool IStateIDsPool;
    typedef xi::ldopa::ts::AttrListStateIDsPool AttrListStateIDsPool;
    typedef xi::ldopa::ts::TsBuilder TsBuilder;
    typedef xi::ldopa::eventlog::IEventLog IEventLog;

    //-------------------------------------------------------------------------
    static void exportClasses()
    {
        // interface ITsStateFunc
        bp::class_<ITsStateFunc, boost::noncopyable>("ITsStateFunc", bp::no_init);
        
        // interface ITsStateFuncVarWs
        bp::class_<ITsStateFuncVarWs, bp::bases<ITsStateFunc>, boost::noncopyable>("ITsStateFuncVarWs", bp::no_init);
        
        // class PrefixStateFunc
        bp::class_<PrefixStateFunc, bp::bases<ITsStateFuncVarWs>, boost::noncopyable>("PrefixStateFunc", 
                    bp::init<IEventLog*, AttrListStateIDsPool*, int>())
            .add_property("act_attr_id",    &PrefixStateFunc::getActAttrID, &PrefixStateFunc::setActAttrID)
            .add_property("ws",             &PrefixStateFunc::getWS, &PrefixStateFunc::setWS)
            .add_property("max_ws",         &PrefixStateFunc::getMaxWndSize)
            .add_static_property("UNLIM_WND_SIZE", bp::make_getter(&PrefixStateFunc::UNLIM_WND_SIZE))                   
        ; //

        // class TsBuilder
        bp::class_<TsBuilder, boost::noncopyable>("TsBuilder", 
                    bp::init<IEventLog*, ITsStateFunc*, IStateIDsPool*>())
            .def("build",   &TsBuilder::build,  bp::return_internal_reference<>())
            .def("detach",  &TsBuilder::detach, bp::return_value_policy<bp::manage_new_object>())
            .add_property("elapsed", &TsBuilder::getElapsed)
        ; // bp::class_<TsBuilder>
    }
}; // struct TsFBuilder_PyWrappers


// export function for EventLog TSs classes
void export_TsFBuilder()
{
    //--=регистрируем обертки для классов=-
    TsFBuilder_PyWrappers::exportClasses();
}
