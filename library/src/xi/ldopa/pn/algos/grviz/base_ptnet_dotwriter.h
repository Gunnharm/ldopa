////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA P/N DOT-writer.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      31.07.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// DOT-writers for base Petri net models.
/// Corresponds to "base_ptnet.h"
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_GRVIZ_BASE_PTNET_DOTWRITER_H_
#define XI_LDOPA_PN_ALGOS_GRVIZ_BASE_PTNET_DOTWRITER_H_

#pragma once


// ldopa
#include "xi/ldopa/graphs/algos/grviz/gen_dot_writer.h"
#include "xi/ldopa/pn/models/base_ptnet.h"
#include "xi/ldopa/utils.h"


// std
#include <map>              // нужно для помеченной, возможно, вынести в отд. файл
//#include <iostream>       // std::endl

namespace xi { namespace ldopa { namespace pn {;   //


/** \brief DOT-writer for MapLabeledPetriNet models. */
struct MapLabeledPetriNetDotVisitor :
    public graph::DefaultDotVisitor < MapLabeledPetriNet<> >
{
    typedef MapLabeledPetriNet<> PN;
    typedef PN::Marking Marking;

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
        // перебираем все вершины
        PN::PnVertIter vIt, vEnd;
        boost::tie(vIt, vEnd) = pn.getVertices();
        for (; vIt != vEnd; ++vIt)
        {
            std::string label;                  // строка метки

            PN::PnVertex v = *vIt;
            const std::string* lbl = pn.getLabel(v);
            if (lbl)
                label = *lbl;

            // если это позицию и для нее есть разметка...
            if (marking && !v.isTrans(pn))
            {
                // грязный хак!... как-то надо это элегантнее обходить...
                PN::Position& pos = *((PN::Position*)(&v));     // TODO: м.б. это надо в класс СП
                UInt posMark = (*marking)(pos);
                if (posMark)
                {
                    if (!label.empty())
                        label += "   ";
                    label += "(";
                    label += std::to_string(posMark);
                    label += ")";
                }
            }
            
            //--<атрибуты>--
            ParamValueList pars;

            // метка только в том случае, если есть ассоциация с вершиной
            //if (lbl)
            pars.append("label", makeEscapedString(label));

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
}; // struct BasePetriNetDotVisitor 



/** \brief Extension for DOT-writter aimed to be used with PN, where a marking
 *  should be taken into consideration.
 * 
 */
template <typename PN, typename PNVisitor = DefaultDotVisitor<PN> >
class BasePetriNetDotWritter : public graph::GenDotWriter<PN, PNVisitor>
{
public:
    /// Base class alias.
    typedef graph::GenDotWriter<PN, PNVisitor> Base;
    
    /// Petri net marking class.
    typedef typename PN::Marking Marking;
public:

    // Constructor.
    BasePetriNetDotWritter(const PNVisitor& gv = PNVisitor())
        : Base(gv)
    {
    }
public:
    /** \brief Writes a dump of the given PN \a gr to a file with the name \a fn, 
     *  having the label \a grLbl and using the given visitor object \a gv.
     *
     *  Apply a marking, which is obtained as the param \a marking.
     */
    void writePn(const std::string& fn, const PN& pn, const Marking& marking, 
        const char* grLbl = nullptr)
    {
        //std::invalid_argument
        std::ofstream dfile(fn.c_str());
        if (!dfile.is_open())
            throw std::invalid_argument("Can't open dump file for GraphViz");

        // заголовок
        outputHeader(dfile, pn, grLbl);

        // тело
        _gv.outputPnBody(dfile, pn, marking);   // в Visitor д.б. метод и такой

        // хвост
        outputTail(dfile, pn);

        dfile.flush();
    }

}; // class BasePetriNetDotWritter

typedef BasePetriNetDotWritter < MapLabeledPetriNet<>, MapLabeledPetriNetDotVisitor >
    MapLabeledPetriNetDotWriter;

}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_PN_ALGOS_GRVIZ_BASE_PTNET_DOTWRITER_H_
