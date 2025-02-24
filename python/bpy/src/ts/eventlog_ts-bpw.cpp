////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for event log TSs Implementation.
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
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"
#include "xi/ldopa/ts/models/eventlog_ts.h"
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/ts/algos/ts_sas_converter.h"

namespace bp = boost::python;

/** \brief Wrappers for state IDs related classes. */
struct StateIDs_PyWrapper
{
    typedef xi::ldopa::ts::IStateIDsPool IStateIDsPool;
    typedef xi::ldopa::ts::IStateId IStateId;
    typedef xi::ldopa::ts::AttrListStateId AttrListStateId;
    typedef xi::ldopa::ts::AttrListStateIDsPool AttrListStateIDsPool;

    //-------------------------------------------------------------------------
    static void exportClasses()
    {
        bp::class_<IStateId, boost::noncopyable>("IStateId", bp::no_init)
            .def("is_equal_to",     &IStateId::isEqualTo)
            .def("is_less_than",    &IStateId::isLessThan)
            .def("to_string",       &IStateId::toString)
        ;

        // abstract (interface), so no constructors are available
        bp::class_<IStateIDsPool, boost::noncopyable>("IStateIDsPool", bp::no_init)
            .def("get_init_stateid",    &IStateIDsPool::getInitStateId  // bp::pure_virtual(надо ли?) 
                , bp::return_internal_reference<>())
            .def("get_size",            &IStateIDsPool::getSize)        // bp::pure_virtual(надо ли?)
        ;        

        bp::class_<AttrListStateIDsPool, bp::bases<IStateIDsPool>, boost::noncopyable>("AttrListStateIDsPool")
            // inherits two base methods
            .def("get_init_stateid",    &AttrListStateIDsPool::getInitStateId,
                    bp::return_internal_reference<>())
            .def("get_size",            &AttrListStateIDsPool::getSize)
        ;
    }
}; // struct StateIDs_PyWrapper


/** \brief Converters of TSs evelement (such as state, trans) to python objects. */
struct ExLogTsEls2PyObj
{
    typedef xi::ldopa::ts::BaseEventLogTS BaseEventLogTS;
    typedef BaseEventLogTS::State State;
    typedef BaseEventLogTS::StateRes StateRes;

    /** \brief Converter for State. */
    static PyObject* convert(State st)
    {
        // знаем, что в нашем случае (State == void*)
        // приведем к виду шестнадцатеричного указателя
        bp::object pyObj = bp::object(uintptr_t(st));           // <cstdint>
        return bp::incref(bp::make_tuple(pyObj).ptr());
    }

    /** \brief Converter for StateRes. */
    static PyObject* convert(const StateRes& stRes)
    {
        if (!stRes.second)
            return bp::incref(bp::object().ptr());              // None

        // иначе под каждый конкретный тип что-нибудь хорошенькое
        bp::object pyObj = bp::object((uintptr_t)stRes.first);
        return bp::incref(bp::make_tuple(pyObj).ptr());
    }
};

/** \brief Wrappers for eventlog TS related classes. */
struct EvLogTSs_PyWrappers
{
    typedef xi::ldopa::ts::BaseEventLogTS BaseEventLogTS;
    typedef xi::ldopa::ts::EvLogTSWithFreqs EvLogTSWithFreqs;
    typedef xi::ldopa::ts::IStateIDsPool IStateIDsPool;
    typedef BaseEventLogTS::State State;
    typedef xi::ldopa::ts::TsSasConverter<EvLogTSWithFreqs>  EvLogTsSasConverter;

    //-------------------------------------------------------------------------

    // альтернативный подход к методу с State...
    static bool isStateReg(BaseEventLogTS& ts, uintptr_t addr)
    {
        // т.к. она не умеет напрямую с void*, сделаем ее из числа...
        State st = (State)addr;
        return ts.isStateRegular(st);
    }

    // альтернативный подход к методу с State...
    static bool isStateAnon(BaseEventLogTS& ts, uintptr_t addr)
    {
        // т.к. она не умеет напрямую с void*, сделаем ее из числа...
        State st = (State)addr;
        return ts.isStateAnon(st);
    }

    //-------------------------------------------------------------------------

    /** \brief Make single accepting states for the given TS. */
    static xi::ldopa::UInt makeSas(EvLogTSWithFreqs* ts)
    {
        // переводим принимающие состояния отдельным конвертером
        EvLogTSWithFreqs::Attribute wfAccepted;                       // single exit attribute
        EvLogTsSasConverter sasConv(wfAccepted);
        return sasConv.convert(ts);
    }

    //-------------------------------------------------------------------------

    static void exportClasses()
    {
        // class BaseEventLogTS
        bp::class_<BaseEventLogTS>("BaseEventLogTS",
            bp::init<IStateIDsPool*>())
            // methods
            .def("get_init_state",              &BaseEventLogTS::getInitState,
                        bp::return_value_policy<bp::return_by_value>())
            .def("get_init_state_id",           &BaseEventLogTS::getInitStateID,
                        bp::return_internal_reference<>())
            .def("get_state",                   &BaseEventLogTS::getState,
                        bp::return_value_policy<bp::return_by_value>())
            .def("is_state_reg",                isStateReg)
            .def("is_state_anon",               isStateAnon)
            // properties
            .add_property("states_num",         &BaseEventLogTS::getStatesNum)
            .add_property("reg_states_num",     &BaseEventLogTS::getRegStatesNum)
            .add_property("anon_states_num",    &BaseEventLogTS::getAnonStatesNum)
            .add_property("trans_num",          &BaseEventLogTS::getTransitionsNum)
        ; // class_<BaseEventLogTS>

        // class EvLogTSWithFreqs
        bp::class_<EvLogTSWithFreqs, bp::bases<BaseEventLogTS>>("EvLogTSWithFreqs",
            bp::init<IStateIDsPool*>())
            .add_property("max_ws",             &EvLogTSWithFreqs::getMaxWS,
                                                &EvLogTSWithFreqs::setMaxWS)
            // combined routines
            .def("make_sas",                    makeSas)
        ; // class_<EvLogTSWithFreqs>
    }
}; // struct EvLogTSs_PyWrapper

// export function for EventLog TSs classes
void export_EvLogTSs()
{
    //--=регистрируем конвертеры=-    
    bp::to_python_converter <ExLogTsEls2PyObj::State, ExLogTsEls2PyObj >();
    bp::to_python_converter <ExLogTsEls2PyObj::StateRes, ExLogTsEls2PyObj >();

    //--=регистрируем обертки для классов=-
    StateIDs_PyWrapper::exportClasses();
    EvLogTSs_PyWrappers::exportClasses();
}
