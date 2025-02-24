////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      11.10.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Implementation of freg-based TS reduction algorithm.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_ALGOS_TS_FREQ_CONDENSER_H_
#define XI_LDOPA_TRSS_ALGOS_TS_FREQ_CONDENSER_H_

//#pragma once


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/utils/elapsed_time.h"


// std
#include <set>

namespace xi { namespace ldopa { namespace ts {;   //


//==============================================================================
//  class CondensedTsBuilder
//==============================================================================


/** \brief Implements an algorithm for building a condensed TS.
*
*  TS that is created during builder work is stored by the builder until next
*  building or the TS is detached by using an appropriate method (detach).
*  Normally, the builder manages the lifetime of a builded TS.
*/
class LDOPA_API CondensedTsBuilder 
        : public ElapsedTimeStore       // вспомогательный класс для учета времени алгоритма
{
public:
    //-----<Types>-----
    /** \brief Alias for the datatype of a currently created TS. */
    typedef EvLogTSWithFreqs TS;

    /** \brief Set of transitions. */
    typedef std::set<TS::Transition> SetOfTrans;
    typedef SetOfTrans::iterator SetOfTransIter;                ///< Iterator for a set.

public:

    /** \brief Constructor initializes with all necessary data.
     *
     *  \param ts is TS to be condensed.
     */
    CondensedTsBuilder(TS* ts); // , Uint numOfTraces);


    /** \brief Destructor. */
    ~CondensedTsBuilder();

protected:
    CondensedTsBuilder(const CondensedTsBuilder&);                 // Prevent copy-construction
    CondensedTsBuilder& operator=(const CondensedTsBuilder&);      // Prevent assignment


public:
    /** \brief Builds a condensed TS using a \a threshold parameter as a threshold for cutting branches.
     *
     *  \returns a TS if built successfully, otherwise nullptr.
     *  A newly built TS is managed by the builder according to the rules,
     *  presented in the class definition.
     */
    TS* build(double threshold);

    /** \brief Detaches and returns the TS built at the previous call of build() method. */
    TS* detach();

    /** \brief Cleans a previously built TS if exists. If no, does nothing. */
    void cleanTS();

public:
    //----<Setters/Getters>----

    /** \brief Returns a ptr to the the TS built at the previous call of build() method. */
    TS* getTS() { return _ts; }

    /** \brief Const overload for getTS(). */
    const TS* getTS() const { return _ts; }

    /** \brief Returns a ptr to the source TS. */
    TS* getSrcTS() { return _srcTs; }

    /** \brief Const overload for getSrcTS(). */
    const TS* getSrcTS() const { return _srcTs; }

    /** \brief Returns a threshold that was used for the very last building. */
    double getThreshold() const { return _threshold; }

    /** \brief Gets the minimum preserver frequency calculated for the given threshold. */
    int getMinPresFreq() const { return _minPresFreq; }

protected:

    /** \brief Marks all the edges with frequecies less than minFreq. */
    void markGauntTransitions();

    /** \brief Removes all transitions that marked as "to be removed" as those 
     *  having their frequencies less than a theshold. 
     */
    void removeMarkedTransitions();

    /** \brief Removes all "widow" states from the condensed TS.
     *
     *  A state is considere as a "widow" one if 1) it does not have any input transition,
     *  and 2) it is not the Initial state.
     */
    void removeWidowStates();

protected:

    /** \brief Src TS to be condensed. */
    TS* _srcTs;

    /** \brief Resulting condensed TS. */
    TS* _ts;

    /** \brief Stores a threshold for cutting branches. */
    double _threshold;

    /** \brief Minimum preserved frequency. */
    int _minPresFreq;

    /** \brief Stores transitions with smaller frequencies, to be excluded. */
    SetOfTrans _trans2excl;
}; // class CondensedTsBuilder


}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_TS_FREQ_CONDENSER_H_
