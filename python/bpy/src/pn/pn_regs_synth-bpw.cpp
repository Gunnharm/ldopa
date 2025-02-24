////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for PN Synthesizer using Regions.
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

#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"
#include "xi/ldopa/pn/algos/regions/pn_synthesis.h"

namespace bp = boost::python;

/** \brief Wrappers for PnRegSynthesizer class. */
struct PnRegSynthesizer_PyWrappers
{
    typedef xi::ldopa::ts::EvLogTSWithFreqs EvLogTSWithFreqs;
    typedef xi::ldopa::pn::EventLogPetriNet<> EventLogPetriNet;

    // синтезатор
    typedef xi::ldopa::pn::PnRegSynthesizer < EvLogTSWithFreqs, EventLogPetriNet >
        EvLogPNSynth;
#pragma region Property Wrapper Methods

    static bool optIsMakeWFNet(EvLogPNSynth* s)
    {
        return s->options().isMakeWFNet();
    }

    static void optSetMakeWFNet(EvLogPNSynth* s, bool f)
    {
        s->options().setMakeWFNet(f);
    }

    static bool optIsOutPlaceTitles(EvLogPNSynth* s)
    {
        return s->options().isOutPlaceTitles();
    }

    static void optSetOutPlaceTitles(EvLogPNSynth* s, bool f)
    {
        s->options().setOutPlaceTitles(f);
    }


    static EvLogPNSynth::Options::SelfLoopPolicy optGetSelfLoopPolicy(EvLogPNSynth* s)
    {
        return s->options().getSelfLoopPolicy();
    }

    static void optSetSelfLoopPolicy(EvLogPNSynth* s, EvLogPNSynth::Options::SelfLoopPolicy p)
    {
        s->options().setSelfLoopPolicy(p);
    }
    
#pragma endregion Property Wrapper Methods

    //-------------------------------------------------------------------------

    static void exportClasses()
    {
        // #appnote: здесь важно определить объект bp::scope synthScope, т.к. тогда следующий за ним
        // enum будет определен в его рамках! иначе он будет в неймспейсе модуля!!!
        // Интересно, что сам объект synthScope нигде не используется, однако пока он жив, 
        // скоп работает
        // https://stackoverflow.com/questions/7254021/class-scoped-enum

        // class EvLogPNSynth
        bp::scope synthScope = bp::class_<EvLogPNSynth, boost::noncopyable>("EvLogPNSynth",
            bp::init<EvLogTSWithFreqs*>())
            .def("synthesize",      &EvLogPNSynth::synthesize) //,  bp::return_internal_reference<>())
            
            //----<Properies>----
            .add_property("pn", 
                bp::make_function(&EvLogPNSynth::getPn,
                    bp::return_internal_reference<> () ) )
            .add_property("init_marking",
                bp::make_function(&EvLogPNSynth::getInitMarking,
                    bp::return_internal_reference<>()))

            // options
            .add_property("make_wf_net", optIsMakeWFNet, optSetMakeWFNet)
            .add_property("out_place_titles", optIsOutPlaceTitles, optSetOutPlaceTitles)
            .add_property("self_loop_policy", optGetSelfLoopPolicy, optSetSelfLoopPolicy)
            .add_property("elapsed", &EvLogPNSynth::getElapsed)
        ; // class_<EvLogPNSynth

        // enum EvLogPNSynth::Options::SelfLoopPolicy
        bp::enum_<EvLogPNSynth::Options::SelfLoopPolicy>("SelfLoopPolicy")
            .value("slIgnore", EvLogPNSynth::Options::slIgnore)
            .value("slReestablish", EvLogPNSynth::Options::slReestablish)
            .value("slProcess", EvLogPNSynth::Options::slProcess)
            .value("slBreak", EvLogPNSynth::Options::slBreak)
        ;
    }
}; // struct BaseEventLogTS_PyWrapper


// export function for EventLog TSs classes
void export_PnRegsSynth()
{
    //--=регистрируем обертки для классов=-
    PnRegSynthesizer_PyWrappers::exportClasses();
}
