////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA DOT-writer for EventLog Petri net model types.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      31.07.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// DOT-writers for EventLog Petri net models.
/// Corresponds to "evlog_ptnets.h"
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_GRVIZ_EVLOG_PTNETS_DOTWRITER_H_
#define XI_LDOPA_PN_ALGOS_GRVIZ_EVLOG_PTNETS_DOTWRITER_H_

#pragma once

// std
// boost

// ldopa
#include "xi/ldopa/utils.h"
#include "xi/ldopa/graphs/algos/grviz/gen_dot_writer.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"
#include "xi/ldopa/pn/algos/grviz/base_ptnet_dotwriter.h"


// std
#include <map>              // нужно для помеченной, возможно, вынести в отд. файл

namespace xi { namespace ldopa { namespace pn {;   //


/** \brief DOT-writer for EventLogPetriNet models. */
struct EventLogPetriNetDotVisitor :
    public graph::DefaultDotVisitor < EventLogPetriNet<> >
{
    typedef EventLogPetriNet<> PN;
    typedef PN::Marking Marking;
    typedef PN::Attribute Attribute;
    typedef std::string String;


    inline void outputPnBody(std::ostream& str, const PN& pn, const Marking& marking)
    {
        outputBody(str, pn, &marking);
    }

    inline void outputBody(std::ostream& str, const PN& pn)
    {
        outputBody(str, pn, nullptr);
    }

    void outputBody(std::ostream& str, const PN& pn, const Marking* marking)
    {
        std::string lbl;

        // перебираем все вершины
        PN::PnVertIter vIt, vEnd;
        boost::tie(vIt, vEnd) = pn.getVertices();
        for (; vIt != vEnd; ++vIt)
        {
            PN::PnVertex v = *vIt;

            //--<атрибуты>--
            ParamValueList pars;

            // метка
            const Attribute* attr = pn.getAttribute(v);
            if (attr)                               // если есть сопоставленный вершине атрибут
            {
                if (attr->getType() == Attribute::tBlank)
                    lbl = "";
                else
                    lbl = attr->toString();         // строкое представления для любого атрибута д.б.!
            }
            else
                lbl.clear();

            // если это позицию и для нее есть разметка...
            if (marking && !v.isTrans(pn))
            {
                // грязный хак!... как-то надо это элегантнее обходить...
                PN::Position& pos = *((PN::Position*)(&v));     // TODO: м.б. это надо в класс СП
                UInt posMark = (*marking)(pos);
                if (posMark)
                {
                    if (!lbl.empty())
                        lbl += "   ";
                    lbl += "(";
                    lbl += std::to_string(posMark);
                    lbl += ")";
                }
            }

            pars.append("label", makeEscapedString(lbl));

            // если это переход, спец. фигуру добавим
            if (v.isTrans(pn))
                pars.append("shape", "box");

            str << makeUintHexId((unsigned int)v.getVertex()) << " " << makeParamValueStr(pars) << std::endl;
        }


        // перебираем все дуги
        PN::BaseArcIter arcIt, arcEnd;
        boost::tie(arcIt, arcEnd) = pn.getArcs();
        for (; arcIt != arcEnd; ++arcIt)
        {
            const PN::BaseArc& arc = *arcIt;
            PN::PnVertex src = pn.getSrcVert(arc);
            PN::PnVertex targ = pn.getTargVert(arc);

            std::string srcId = makeUintHexId((unsigned int)src.getVertex());
            std::string targId = makeUintHexId((unsigned int)targ.getVertex());

            //---<атрибуты>---
            ParamValueList pars;

            std::string lbl;

            xi::ldopa::UInt arcWeight = pn.getArcWeight(arc);
            if (arcWeight != 1)
            {
                lbl += '/';
                lbl += std::to_string(arcWeight);
                pars.append("label", makeEscapedString(lbl));
            }

            // ингибиторная
            if (pn.getArcType(arc) == PN::ArcType::atInhib)
                pars.append("arrowhead", "odot");

            // выводим
            str << srcId << " -> " << targId << " " << makeParamValueStr(pars) << std::endl;
        }
    }
}; // struct EventLogPetriNet 

typedef  BasePetriNetDotWritter < EventLogPetriNet<>, EventLogPetriNetDotVisitor >
    EventLogPetriNetDotWriter;

}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_PN_ALGOS_GRVIZ_EVLOG_PTNETS_DOTWRITER_H_
