////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri Nets Library: Base P/T net classes.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      15.04.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Some Base classes for P/T nets.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_MODELS_BASE_PTNET_H_
#define XI_LDOPA_PN_MODELS_BASE_PTNET_H_

#pragma once

// std
// boost

// ldopa
#include "gen_petrinet.h"   // same catalog
#include "xi/ldopa/utils.h"

// std
#include <map>              // нужно для помеченной, возможно, вынести в отд. файл


namespace xi { namespace ldopa { namespace pn {;   //


//==============================================================================
// class BasePetriNet
//==============================================================================

/** \brief A simple wrapper on GenPetriNet class adding features for checking active transitions.
 *
 *  elaborate
 */
template <
    typename TVertData = boost::no_property,        // Данные на позициях/переходах (можно только один общий тип!).
    typename TArcData = boost::no_property          // Данные на дугах.
>
class BasePetriNet : public GenPetriNet<TVertData, TArcData>
{
public:
    //-----<Types>-----
    typedef GenPetriNet<TVertData, TArcData> Base;      ///< Alias for the base class.
    typedef GenPetriNetMarking<TVertData, TArcData> Marking; ///< Corresponding marking type.
    typedef std::map<BaseArc, UInt> ArcWeights;         ///< Mapping Arcs-to-Weights.

public:
#pragma region Interface for checking active transition 
    
    /** \brief Determines whether the transition \a t is enabled in the marking \a m or not. */
    virtual bool isEnabled(Transition t, const Marking& m)
    {
        InPosIter tCur, tEnd;
        for (boost::tie(tCur, tEnd) = getPreset(t); tCur != tEnd; ++tCur)
        {
            Position pos = (*tCur).first;       // позиция и...
            PTArc arc = (*tCur).second;         // ...инцидентная дуга
            UInt tokensNum = m(pos);            // число токенов в позиции

            // смотрим на тип дуги
            ArcType at = getArcType(arc);
            if (at == PnArcBundle_traits_base::atReg)   // >> обычная дуга
            {
                UInt arcWeight = getArcWeight(arc);     // вес дуги
                

                if (tokensNum < arcWeight)              // если число токенов меньше, чем вес дуги
                    return false;                       // все, уже пока
            }
            else                                        // >> ингибиторная дуга
            {
                if (tokensNum > 0)                      // если есть хотя бы одна фишка
                    return false;                       // все, тоже пока
            }
            
            // иначе эта дуга ок, идем дальше                       
        }

        return true;                                    // если не вывалились до сих пор, то транзиция активна
    }

#pragma endregion

#pragma region Working with weighted arcs
    
    /** \brief Adds a new regular weighted arc. Weight cannot be 0. */
    PTArc addArcW(Position srcP, Transition targT, UInt weight = 1)
    {
        if (weight == 0)
            throw LdopaException("Arc's weight cannot be 0.");

        PTArc arc = Base::addArc(srcP, targT);
        setArcWeight(arc, weight);

        return arc;
    }

    /** \brief Adds a new regular weighted arc. Weight cannot be 0. */
    TPArc addArcW(Transition srcT, Position targP, UInt weight = 1)
    {
        if (weight == 0)
            throw LdopaException("Arc's weight cannot be 0.");

        TPArc arc = Base::addArc(srcT, targP);
        setArcWeight(arc, weight);

        return arc;
    }

    /** \brief For given arc \a arc, returns its weight. */
    UInt getArcWeight(const BaseArc& arc) const
    {
        // если в мапе нет информации привязки к дуге, вес 1. может ли вес быть 0 или отриц?
        ArcWeights::const_iterator wIt = _arcWeights.find(arc);
        if (wIt == _arcWeights.end())
            return 1;

        // если есть инфа в мапе, возвращаем ее
        return wIt->second;
    }

    
    /** \brief Sets weight of the arc \a arc to the value \a w. */
    void setArcWeight(const BaseArc& arc, UInt w)
    {
        _arcWeights[arc] = w;
    }

#pragma endregion

protected:
    ArcWeights _arcWeights;                         ///< Stores weights for weighted arcs.
//    PN _pn;                                         ///< Petri net itself.

}; // class BasePetriNet


//==============================================================================
// class MapLabeledPetriNet
//==============================================================================

/** \brief Petri net with string labels on all component types (poss, transs, arcs)
 *  provided by associated map.
 *
 *  There possible to compose an equivalemnt (by semantics) type, where labels are
 *  stored directly in vertices and acrs by using TVertData and TArcData template params.
 */
template <
    typename TVertData = boost::no_property,
    typename TArcData = boost::no_property
>
class MapLabeledPetriNet : public BasePetriNet < TVertData, TArcData >
{
public:
    //-----<Types>-----
    typedef BasePetriNet<TVertData, TArcData> Base;             ///< Alias for the base class.
    typedef std::map<PnVertex, std::string> VertexStringMap;    ///< Vertices-to-strings mapping.

public:

    //---<Addition methods for creating PN's structure>---

    // какая прелесть! при определении прегруженного метода в производном классе лукап останавливается,
    // как только подходящее имя будет найдено! это удобно! чтобы не переопределять описания методов,
    // можно использовать using здесь, см. 
    // https://stackoverflow.com/questions/1628768/why-does-an-overridden-function-in-the-derived-class-hide-other-overloads-of-the
    using Base::addPosition;
    using Base::addTransition;


    /// Adds a new position labeled by the label \a lbl.
    Position addPosition(const std::string& lbl)
    {
        Position p = Base::addPosition();
        _vertLabels[p] = lbl;

        return p;
    }

    /// Adds a new transition labeled by the label \a lbl.
    Transition addTransition(const std::string& lbl)
    {
        Transition t = Base::addTransition();
        _vertLabels[t] = lbl;

        return t;
    }

    /** \brief Returns a ptr to a string lable of the goven vertex \a v or null if no label exists. */
    const std::string* getLabel(const PnVertex& v) const
    {
        VertexStringMap::const_iterator it = _vertLabels.find(v);
        if (it == _vertLabels.end())
            return nullptr;

        // иначе найден — вернем указатель на персистентный объект!
        return &(it->second);
    }

protected:
    VertexStringMap _vertLabels;                                ///< Stores string labels for poss and transs.
}; // class MapLabeledPetriNet


}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_PN_MODELS_BASE_PTNET_H_
