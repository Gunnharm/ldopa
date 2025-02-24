////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      18.08.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// TS converter thas makes the single accepting state for the TS.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_ALGOS_TS_SAS_CONVERTER_H_
#define XI_LDOPA_TRSS_ALGOS_TS_SAS_CONVERTER_H_

//#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/eventlog/eventlog.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"
#include "xi/ldopa/utils/progress_callback.h"


namespace xi { namespace ldopa { namespace ts {;   //

//==============================================================================
//  class TsSasConverter
//==============================================================================


/** \brief Converts a TS in such a way that the only one accepting state is left
 *  and all previous ones has a single outcoming transition to it making
 *  with the special attribute.
 */
template<typename TS>
class /* LDOPA_API */ TsSasConverter {
public:

    typedef typename TS::TrLabel Label;          ///< TS label type. 
    typedef typename TS::State TsState;          ///< TS state type.
    typedef typename TS::StateIter TsStateIter;  ///< TS state iterator type.

public:
    //----<Constructors and destructor>----
    
    /** \brief Default constructor. */
    TsSasConverter() { } ;

    /** \brief Constructor initializes the converter with the SAS label. */
    TsSasConverter(const Label& lbl) : _sasLabel(lbl) { };

public:
    //----<Main interface>----
    
    /** \brief Converts a TS. 
     *
     *  \returns the number of converted accepted states.
     *  If no accepted states has been presented in the given TS, do not
     *  add news one.
     *  
     */
    UInt convert(TS* ts)
    {
        // создадим искусственное принимающее состояние
        TsState sas = ts->addAnonState();
        UInt res = 0;

        // перебираем все состояния
        TsStateIter stIt, stEnd;
        boost::tie(stIt, stEnd) = ts->getStates();
        for (; stIt != stEnd; ++stIt)
        {
            TsState s = *stIt;
            if (ts->isStateAccepting(s))
            {
                ts->getOrAddTrans(s, sas, _sasLabel);
                ts->setAcceptingState(s, false);               // больше не принимающий
                ++res;
            }
        }

        // если было хоть одно преобразовано.
        if (res)
            // делаем принимающим в конце, чтобы в цикл доп. условие по "не равен самому себе" не добавлять
            ts->setAcceptingState(sas, true);
        else
            ts->removeState(sas);

        return res;
    }


public:
    //----<Setters/Getters>----
    /** \brief \returns an attribute , by which arcs leading to the artificial 
     *  single accepted state labeled. 
     */
    const Label& getSASLabel() const { return _sasLabel;  }

    /** \brief Sets an single accpt trans attribute. 
     *   See getSingleAcptTransAttr() for explanation. 
     */
    void setSASLabel(const Label& lbl ) { _sasLabel = lbl;  }
protected:


    /** \brief Stores an attribute , by which transitions leading to the artificcial 
     * single accepted state labeled. */
    Label _sasLabel;

}; //  class TsSasConverter

}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_TS_SAS_CONVERTER_H_
