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

#ifndef XI_LDOPA_TRSS_ALGOS_TS_METRICS_CALC_H_
#define XI_LDOPA_TRSS_ALGOS_TS_METRICS_CALC_H_

#pragma once


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"

namespace xi { namespace ldopa { namespace ts {;   //

//==============================================================================
//  class TsMetricsCalc
//==============================================================================

/** \brief Implements an algorithm for building a condensed TS.
*
*  TS that is created during builder work is stored by the builder until next
*  building or the TS is detached by using an appropriate method (detach).
*  Normally, the builder manages the lifetime of a builded TS.
*/
class LDOPA_API TsMetricsCalc {
public:
    //-----<Types>-----

    /** \brief Alias for the datatype of a currently created TS. */
    typedef EvLogTSWithFreqs TS;

    /** \brief Alias for IEventLog2.*/
    typedef eventlog::IEventLog IEventLog;

    /** \brief Declares a set of log attributes. */
    typedef std::set<IEventLog::Attribute> AttributesSet;

public:
    /** \brief Constructor. */
    TsMetricsCalc(IEventLog* log, TS* _fullTs = nullptr);

protected:
    TsMetricsCalc(const TsMetricsCalc&);                 // Prevent copy-construction
    TsMetricsCalc& operator=(const TsMetricsCalc&);      // Prevent assignment

public:
    /** \brief Calculates simplicity of the given TS. 
     * 
     *  \param ts is a TS, from which simplicity metric is calculated. If it is not given,
     *  the method uses the last passed ts. If no ts has been passed previously, raises an exception.
     */
    double calcSimplicity(TS* ts = nullptr);

    /** \brief Calculates precision of the given TS. 
     * 
     *  \param ts is a TS, from which simplicity metric is calculated. If it is not given,
     *  the method uses the last passed ts. If no ts has been passed previously, raises an exception.
     */
    double calcPrecision(TS* ts = nullptr);

    /** \brief Calculates generalization of the given TS.
     *
     *  \param ts is a TS, from which simplicity metric is calculated. If it is not given,
     *  the method uses the last passed ts. If no ts has been passed previously, raises an exception.
     */
    double calcGeneralization(TS* ts = nullptr);
public:
    /** \brief Return a TS, for which last metric has been calculated. */
    const TS* getTs() const { return _ts; }

protected:
    /** \brief Sets the given \a ts as one, fro which a metric is calculated. 
     * 
     *  If \a ts is null, doesn't change the previously set value. If the value is null
     *  itself, raises an exception.
     */
    void setAndCheckTs(TS* ts);

    /** \brief For a given state of _ts, calculates its in-flow. */
    int getStateInFlow(TS::State st) const;
protected:
    /** \brief Stores a ptr to an event log.  */
    IEventLog* _log;

    // /** \brief Stores a ptr to a set to all unique log activities. */
    // const AttributesSet* _logActivities;

    /** \brief Stores a ptr to a TS, metrics of which are calculated. */
    TS* _ts;

    /** \brief Stores a ptr to a full TS for calculation of precision, which is needed it as a ref TS. */
    TS* _fullTs;

    /** \brief Stores the last calculated simplicity. */
    double _calculatedSimplicity;

    /** \brief Stores the last calculated precision. */
    double _calculatedPrecision;

    /** \brief Stores the last calculated generalization. */
    double _calculatedGeneralization;

}; // class TsMetricsCalc




}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_TS_METRICS_CALC_H_
