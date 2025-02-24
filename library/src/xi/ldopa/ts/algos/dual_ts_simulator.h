////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      17.01.2018
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Classes for calculating metrics of TSs
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_ALGOS_DUAL_TS_SIMULATOR_H_
#define XI_LDOPA_TRSS_ALGOS_DUAL_TS_SIMULATOR_H_

#pragma once


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"


namespace xi { namespace ldopa { namespace ts {;   //


//==============================================================================
//  class DualTsSimulator
//==============================================================================


/** \brief Implements an algorithm for building a condensed TS.
*
*  TS that is created during builder work is stored by the builder until next
*  building or the TS is detached by using an appropriate method (detach).
*  Normally, the builder manages the lifetime of a builded TS.
*/
class LDOPA_API DualTsSimulator {
public:
    //-----<Types>-----

    /** \brief Alias for the datatype of a currently created TS. */
    typedef EvLogTSWithFreqs TS;
    typedef TS::State State;
    typedef TS::TransRes TransRes;
    typedef TS::Attribute Attribute;
    
    /** \brief Defines a dbl-int pair. */
    typedef std::pair<double, int> DblInt;

    /** \brief Defines a datatype that maps states to <dbl, int> pair. */
    typedef std::map<TS::State, DblInt> StateDblIntMap;
public:
    /** \brief Default constructor. */
    DualTsSimulator();
protected:
    DualTsSimulator(const DualTsSimulator&);                 // Prevent copy-construction
    DualTsSimulator& operator=(const DualTsSimulator&);      // Prevent assignment

public:

    /** \brief Calculates precision of ts2 by simulating ts1 on ts2. */
    double calcPrecision(TS* ts1, TS* ts2);

protected:

    /** \brief Calculates partial states precision. 
     * 
     *  \param s1 is a state of TS1 to calc precision of.
     *  \param s2 is a corresponding (to \a s1) state of TS2 to calc precision of.
     */
    void calcStatePrecision(TS::State s1, TS::State s2);

    /** \brief Returns corresponding for \a tr transition that is output from the state \a s2. */
    inline TS::TransRes getMatchingTransOfTs2(TS::Transition tr, TS::State s2)
    {
        const Attribute& a = _ts1->getTransLbl(tr);
        return _ts2->getFirstOutTrans(s2, a);
    }

    /** \brief For a given state \a s1 and its semipart precision \a semiPartStatePrec,
     *  increments the value of part "precision".
     */
    void incStatePrecision(State s1, double semiPartStatePrec);

    /** \brief For a given state \a s, returns its partial precision/num of calculations.
     *  If no partial precision for the state has been previously set, returns (0, 0) by
     *  reference (with adding a corresponding pair to a map), so it can be directly assigned.
     */
    DblInt& getStatePrecision(State s);

    /** \brief Calculates and returns a sum of partial precision of the _ts1 after simulating. */
    double sumPartialPrecisions();


protected:
    /** \brief TS1 simulates TS2. */
    TS* _ts1;

    /** \brief TS2 is simulated by TS1. */
    TS* _ts2;

    /** \brief Represents a map of states to associated partial state precisions. */
    StateDblIntMap _partStatePrecs;

}; // class DualTsSimulator




}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_DUAL_TS_SIMULATOR_H_
