////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      10.01.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// A builder of TSs, based on variadic WS.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_ALGOS_VARWS_TS_REBUILDER_H_
#define XI_LDOPA_TRSS_ALGOS_VARWS_TS_REBUILDER_H_

//#pragma once

// std
#include <vector>
#include <set>

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// // ldopa
#include "xi/ldopa/ts/algos/ts_simple_builder.h"      // интерфейсы функций состояния
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/utils/elapsed_time.h"


namespace xi { namespace ldopa { namespace ts {;   //

//==============================================================================
//  class VarWsTsBuilder
//==============================================================================

/** \brief Implements an algorithm for building a var-ws TS.
 *
 *  TS that is created during work of the builder is stored by the builder until next
 *  building or the TS is detached by using an appropriate method (detach).
 *  Normally, the builder manages the lifetime of a builded TS.
 */
class LDOPA_API VarWsTsBuilder 
    : public ElapsedTimeStore       // вспомогательный класс для учета времени алгоритма
{
public:
    //-----<Types>-----
    /** \brief Alias for the datatype of a currently created TS. */
    typedef EvLogTSWithFreqs TS;

    /** \brief An integer-State pair type. */
    typedef std::pair<int, TS::State> IntStatePair;

    /** \brief Vector of int-state pairs. */
    typedef std::vector<IntStatePair> IntStateVector;

    /** \brief Set of transitions. */
    typedef std::set<TS::Transition> SetOfTrans;    ///< Set of transitions.
    typedef SetOfTrans::iterator SetOfTransIter;    ///< Iterator for a set.

    typedef eventlog::IEventLog IEventLog;        ///< Alias for IEventLog2.
    typedef eventlog::IEventTrace IEventTrace;    ///< Alias for IEventTrace2.
    typedef eventlog::IEvent IEvent;              ///< Alias for IEvent2.

public:

    /** \brief Defines possible policies when a state ID is based on 0-WS. */
    enum ZeroSizeWndAction
    {
            zsaSpecState                ///< 0-ws is  allowed, use a special trash state
        ,   zsaSet1                     ///< 0-ws is not allowed, set it to 1
    };

public:

    /** \brief Constructor initializes with all necessary data.
     *
     *  \param ts is TS to be condensed.
     */
    VarWsTsBuilder(IEventLog* log, TS* ts, ITsStateFuncVarWs* sf);

    /** \brief Destrucotr. */
    ~VarWsTsBuilder();

protected:
    VarWsTsBuilder(const VarWsTsBuilder&);                 // Prevent copy-construction
    VarWsTsBuilder& operator=(const VarWsTsBuilder&);      // Prevent assignment

public:
    /** \brief Rebuilds a final TS on the basis of a given condensed TS.
     *
     *  \param vwsc is a multiplicity parameter for the 3rd phase 
     *  while a 3TS is being built from a condensed TS.
     *  \param zsa determines a policy that shoulb be applied when a 0-WS state ID is formed.
     *  \returns a TS if built successfully, otherwise nullptr.
     *  A newly built TS is managed by the builder according to the rules,
     *  presented in the class definition.
     */
    TS* build(double vwsc, ZeroSizeWndAction zsa);

    /** \brief Detaches and returns the TS built at the previous call of build() method. */
    TS* detach();

    /** \brief Cleans a previously built TS if exists. If no, does nothing. */
    void cleanTS();

protected:
    /** \brief Performs log replaying loop reconstructing TS.  */
    void replayLoop();    

    /**
     * Replays a given trace on a TS under reconstruction.
     *
     * \param traceNum a trace number to be replayed.
     * \return true, if the trace has been replayed completely, false otherwise.
     */
    bool replayTrace(int traceNum);

    /** \brief Converts temporary transitions and states to regular elements.
     *
     *  \param traceNum a trace number to be restated.
     */
    void restateTrace(int traceNum);

    /** \brief Resets and prepares a vector of traces' last event nums. */
    void reset();

    /** \brief Returns true is a given transition \a t is temporary one, otherwise false. */
    bool isTransTemp(TS::Transition t) const
    {
        return (_tempTrans.find(t) != _tempTrans.end());
    }


    /** \brief Marks the given transition \a t to be temporary (\a isTemp == true) or not. */
    inline void markTransAsTemp(TS::Transition t, bool isTemp)
    {
        if (isTemp)
            _tempTrans.insert(t);
        else
            _tempTrans.erase(t);
    }


    /** \brief To a given transition \a t, which is considered to be a temporary, increments 
     * its frequency attribute.
     */
    void incTransFreq(TS::Transition t);


    /** \brief For a given initial state \a initSt and a trace number \a traceNum, tries
     *  to iterate across the subtrace from the state \a initSt to \a traceNum-th state.
     *
     *  \returns state and true if a eventNum-th state exists, false otherwise.
     */
    TS::StateRes replayUpToNthState(TS::State initSt, int traceNum, int eventNum);

    /** \brief Returns the num of the first unreplayed event for a trace number \a traceNum. */
    int getLastEventNum(int traceNum) const { return _tracesCompl[traceNum].first; }

    /** \brief Returns the last regular replayed state for a trace number \a traceNum. */
    TS::State getLastEventState(int traceNum) const { return _tracesCompl[traceNum].second; }

    /** \brief Returns the num of the first unreplayed event for a trace number \a traceNum 
     *  and a corresponding previous state in the restated TS.
     */
    IntStatePair getLastEvNumState(int traceNum) const { return _tracesCompl[traceNum]; }

    /** \brief Sets the first unprocessed event num \a eventNum and a corresponding 
     *  previous state \a st for the given trace number \a traceNum.
     */
    void setLastEvNumState(int traceNum, int eventNum, TS::State st)
    {
        _tracesCompl[traceNum] = std::make_pair(eventNum, st);
    }

    /** \brief Sets the last processed event num \a eventNum only for the given 
     *  trace number \a traceNum.
     */
    void setLastEventNum(int traceNum, int eventNum) { _tracesCompl[traceNum].first = eventNum; }

    /** \brief Returns a dedicated 0WS state creating it if needed. */
    TS::State get0WsState();

protected:
    /** \brief Stores a ptr to an event log.  */
    IEventLog* _log;

    /** \brief Stores an ID for the Activity attribute. 
     *
     *  Also, see a similar attribute in PrefixStateFunc.
     */
    std::string _actAttrID;

    /** \brief Stores a state function. */
    ITsStateFuncVarWs* _sf;

    /** \brief Src condensed TS to be rebuilt. */
    TS* _srcTs;

    /** \brief Resulting variadic-ws TS. */
    TS* _ts;

    /** \brief Stores a vwsc parameter that is set while building. */
    double _vwsc; 

    /** \brief Stores a _zsa parameter that is set while building. */
    ZeroSizeWndAction _zsa;

    /** \brief For every trace, stores the number of the first unprocessed event 
     *  and a corresponding previous state in the restated TS . 
     */
    IntStateVector _tracesCompl;
    //IntVector _tracesCompl;

    /** \brief Stores number of traces for a current building session. */
    int _tracesNum;

    /** \brief Stores temporary transitions. */
    SetOfTrans _tempTrans;

    /** \brief Stores a dedicated state for 0WS. */
    TS::State _0wsState;

    /** \brief Flag determines whether a 0WS state is created (true) or not. */
    bool _0wsStateSet;
}; // class VarWsTsBuilder



}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_VARWS_TS_REBUILDER_H_
