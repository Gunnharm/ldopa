////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA EvLogTs DOT-writer.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      31.07.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// DOT-writers for some TS models.
/// Corresponds to "evlog_ts_red_dotwriter.h"
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_GRVIZ_BASE_PTNET_DOT_WRITER_H_
#define XI_LDOPA_PN_ALGOS_GRVIZ_BASE_PTNET_DOT_WRITER_H_

#pragma once

// std
// boost

// ldopa
#include "xi/ldopa/graphs/algos/grviz/gen_dot_writer.h"
#include "xi/ldopa/ts/models/evlog_ts_red.h"


namespace xi { namespace ldopa { namespace ts {;   //



/** \brief DOT-writer for EvLogTSWithFreqs models. */
struct EvLogTSWithFreqsDotVisitor :
    public xi::ldopa::graph::DefaultDotVisitor < xi::ldopa::ts::EvLogTSWithFreqs >
{
    typedef xi::ldopa::ts::EvLogTSWithFreqs TS;

    void outputBody(std::ostream& str, const TS& ts)
    {
        // перебираем все вершины
        TS::StateIter stIt, stItEnd;
        boost::tie(stIt, stItEnd) = ts.getStates();
        for (; stIt != stItEnd; ++stIt)
        {
            TS::State s = *stIt;

            // если состояние "анонимное", то метку не делаем
            std::string lbl;
            if(ts.isStateRegular(s))
                lbl = ts.makeStateIDStr(s);

            //--<атрибуты>--
            ParamValueList pars;

            // метка
            pars.append("label", makeEscapedString(lbl));

            // принимающее состояние
            if (ts.isStateAccepting(s))
                pars.append("peripheries", "2");

            str << makeUintHexId((unsigned int)s) << " " << makeParamValueStr(pars) << std::endl;
        }


        // перебираем все дуги
        TS::TransIter trIt, trItEnd;
        boost::tie(trIt, trItEnd) = ts.getTransitions();
        for (; trIt != trItEnd; ++trIt)
        {
            TS::Transition& tr = *trIt;
            TS::State src = ts.getSrcState(tr);
            TS::State targ = ts.getTargState(tr);

            std::string srcId = makeUintHexId((unsigned int)src);
            std::string targId = makeUintHexId((unsigned int)targ);

            //---<атрибуты>---
            ParamValueList pars;

            // метка
            const TS::Attribute& tlLbl = ts.getTransLbl(tr);
            std::string lbl;
            if (!tlLbl.isEmpty())
                lbl = tlLbl.toString();

            TS::IntRes trFreq = ts.getTransFreq(tr);
            if (trFreq.second)
            {
                lbl += '/';
                lbl += std::to_string(trFreq.first);
            }

            pars.append("label", makeEscapedString(lbl));

            // выводим
            str << srcId << " -> " << targId << " " << makeParamValueStr(pars) << std::endl;
        }
    }
};

typedef xi::ldopa::graph::GenDotWriter < xi::ldopa::ts::EvLogTSWithFreqs, EvLogTSWithFreqsDotVisitor >
            EvLogTSWithFreqsDotWriter;

}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_PN_ALGOS_GRVIZ_BASE_PTNET_DOT_WRITER_H_
