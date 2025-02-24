////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      18.10.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Definitions of EventLog TS that are suitable for reduction.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_MODELS_EVLOG_TS_RED_H_
#define XI_LDOPA_TRSS_MODELS_EVLOG_TS_RED_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/eventlog_ts.h"


namespace xi { namespace ldopa { namespace ts {;   //



/** \brief Definition for Event Log TS containing additional attributes.
 *
 *  Additional attributes: frequency for transitions and accepting flags for states.
 */
class LDOPA_API EvLogTSWithFreqs : public BaseEventLogTS
{
public:
#pragma region Type Definitions
    typedef BaseEventLogTS Base;


    /** \brief Type for mapping transitions to integers. 
     *
     *  Mind using \a unordered_map instead (see EdgePstrMap).
     */
    typedef std::map<Transition, int> TransIntMap;

    /** \brief A wrapper class for a bitset for flags-for-a-state type */
    class StateFlags : public std::bitset<2> {
    public:
        //-----<Const>-----
        /** \brief List of pseudo-constants for numbering bits in bitset */
        enum {
            FL_ACCEPTING = 0,           ///< Accepting state.
            FL_ACCEPTING_SET,           ///< True, if accepting flag is set, false otherwise.
        };

        /** \brief Enumeration for tri-state flags. */
        enum TriState {
            triNotSet,                   ///< Flag is not set.
            triTrue,                     ///< Flag set to true (is `set').
            triFalse                     ///< Flag set to false (is `reset').
        };
    public:
        //-----<Flags operations>-----        
        TriState getAccepting() const
        {
            if (test(FL_ACCEPTING_SET))
                return test(FL_ACCEPTING) ? triTrue : triFalse;
            return triNotSet;
        }

        void setAccepting(bool fl)
        {
            set(FL_ACCEPTING, fl);
            set(FL_ACCEPTING_SET, true);
        }

        void clearAccepting() { set(FL_ACCEPTING_SET, false); }
    }; // class StateFlags

    /** \brief Mapping of states to state flags. */
    typedef std::map<State, StateFlags> StateFlagsMap;

    /** \brief Datatype for an integer resulting value that can be undefined. */
    typedef std::pair<int, bool> IntRes;

    /** \brief Datatype for an state resulting value that can be undefined. */
    typedef std::pair<StateFlags, bool> StateFlagsRes;

#pragma endregion // Type Definitions

public:
    /** \brief Constructor initializes the TS by a pool of events.
     *
     *  \param stIDsPool is a (possibly shared) pool of state IDs.
     *  \param copyPool determines whether the pool of stateIDs should be cloned (and, hence,
     *  managed locally — if true) or not (false).
     */
    EvLogTSWithFreqs(IStateIDsPool* stIDsPool); // , bool copyPool = false);


    /** \brief Copy constructor. 
     *
     *  It is crucial to make a right copy for _transFreqsMap and _stateFlags.
     */     
    EvLogTSWithFreqs(const EvLogTSWithFreqs& that);

    /** \brief Virtual Destructor. */
    virtual ~EvLogTSWithFreqs();

public:
    //----<Extended interface>----

    /** \brief Gets a transition between given pair of states \a s and \a t,
     *  marked by a label \a lbl, taking into account its frequency. 
     *
     *  If the same transition appears again and again, its frequecy is increased
     *  by adding \a addFreq value.
     *  If no transition exists, creates a new one.
     *
     *  If you do not want to change the frequency, use Base::getOrAddTrans insted.
     */
    Transition getOrAddTransF(State s, State t, const Attribute& lbl, int addFreq);

public:
    //----<Working with attributes>----

    /** \brief Returns a frequency for the given transition \a t. 
     *
     *  If a frequecy has been set, the second parameter of the resulting object
     *  is true, otherwise false.
     */
    IntRes getTransFreq(Transition t) const;

    /** \brief Sets a new value \a freq of frequency for the given transition \a t. */
    void setTransFreq(Transition t, int freq);


    /** \brief Returns a state fags for the given state \a s.
     *
     *  If a state flags is set, the second parameter of the resulting object
     *  is true, otherwise false.
     */
    StateFlagsRes getStateFlags(State s) const;

    /** \brief Sets a new value \a freq of state flags for the given state \a s. */
    void setStateFlags(State s, StateFlags sf);

    /** \brief Returns true if the given state \a s is accepting one, otherwise false. */
    bool isStateAccepting(State s) const;

    /** \brief Sets the given state \a s to be accepting one (\a accepting is true)
        or not (\a accepting is false). */
    void setAcceptingState(State s, bool accepting);

    int getMaxWS() const { return _maxWS; }     ///< Gets MaxWS attribute.
    void setMaxWS(int ws) { _maxWS = ws; }      ///< Sets MaxWS attribute.    

protected:
    //----<Construction helpers>----
    
    /** \brief Makes all state-corresponding structures valid after copying the underlying graph. 
     *
     *  \param that is a TS, from which a copy (into this TS) has been done.
     */
    void validateStates(const EvLogTSWithFreqs& that);

    /** \brief Makes all trans-corresponding structures valid after copying the underlying graph.
     *
     *  \param that is a TS, from which a copy (into this TS) has been done.
     */
    void validateTransitions(const EvLogTSWithFreqs& that);
protected:
    

    /** \brief Stores a map of edges to assigned frequencies. */
    TransIntMap _transFreqsMap;

    /** \brief Stores a map of states to flags. */
    StateFlagsMap _stateFlags;

    /** \brief Max Window Size attribute, for which the TS is built. */
    int _maxWS;


}; // class EvLogTSWithFreqs


}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_MODELS_EVLOG_TS_RED_H_
