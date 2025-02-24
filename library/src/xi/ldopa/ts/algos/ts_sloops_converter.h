////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      22.08.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Eliminates self-loops in TS, converts them into equivalent behavioral structures.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_ALGOS_TS_SLOOPS_CONVERTER_H_
#define XI_LDOPA_TRSS_ALGOS_TS_SLOOPS_CONVERTER_H_

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

/** \brief Converts self-loops in a TS in such a way that are become behavioral-
 *  equivalented strcutures.
 */
template<typename TS>
class /* LDOPA_API */ TsSLoopsConverter {
public:

    typedef typename TS::TrLabel    Label;          ///< TS label type. 
    typedef typename TS::State      TsState;        ///< TS state type.
    typedef typename TS::StateIter  TsStateIter;    ///< TS state iterator type.
    typedef typename TS::Transition TsTransition;   ///< TS transition type.
    typedef typename TS::TransIter  TsTransIter;    ///< TS transition iterator type.
    typedef typename TS::OtransIter  TsOtransIter;    ///< TS output-transition iterator type.

public:
    //----<Constructors and destructor>----
    
    /** \brief Default constructor. */
    TsSLoopsConverter() { } ;
public:
    //----<Main interface>----
    
    /** \brief Converts a TS for eliminating self-loops. 
     *
     *  \returns the number of eliminated self-loops.
     *  #appnote: не принимаем во внимание какие-то специальные атрибуты пребразуемых 
     *  состояний/переходов, т.к. работаем с системой переходов общего вида
     *  
     *  #todo: В систему переходов можно добавить методы по клонированию состояний/дуг в том смысле,
     *  что в TS добавляется новое состояния/дуга такие, что их свойства будут такими же, как и
     *  у исходной.
     *  
     */
    UInt convert(TS* ts)
    {
        UInt num = 0;

        TsTransIter trIt, trEnd;
        boost::tie(trIt, trEnd) = ts->getTransitions();
        while (trIt != trEnd)
        {
            const TsTransition& t = *(trIt++);      // здесь же сразу и двинемся
            TsState s = ts->getSrcState(t);
            if (s != ts->getTargState(t))           // если не петля
                continue;                           // двигаем дальше

            //----если это петля----
            const Label& lbl = ts->getTransLbl(t);  // пометка дуги-петли
             
            // создаем новое анонимное состояние
            TsState s1 = ts->addAnonState();

            if (ts->isStateAccepting(s))
                ts->setAcceptingState(s1, true);
            TsOtransIter otIt, otEnd;
            for (boost::tie(otIt, otEnd) = ts->getOutTransitions(s); otIt != otEnd; ++otIt)
            {
                const TsTransition& curOt = *otIt;

                // саму петлю надо не забыть пропустить
                if (curOt == t)
                    continue;

                // для всех остальных — доппельгангеры
                const Label& otLbl = ts->getTransLbl(curOt);
                ts->getOrAddTrans(s1, ts->getTargState(curOt), otLbl);
            }

            // прокидываем дуги от исходного состояния к новому и обратно
            ts->getOrAddTrans(s, s1, lbl);
            ts->getOrAddTrans(s1, s, lbl);


            // напоследок собственно убираем петлю, не забывая, что для хранения
            // дуг используется listS-контенер, поэтому удаление элементов из него
            // должно выполняться по правилам: https://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
            // т.к. уже ранее двинули итератор, удалим по объекту
            ts->removeTrans(t);
            ++num;
        }
        return num;
    }
public:
    //----<Setters/Getters>----
}; //  class TsSLoopsConverter

}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_TS_SLOOPS_CONVERTER_H_
