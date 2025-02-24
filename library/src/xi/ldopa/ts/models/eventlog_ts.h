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
/// Declaration for a base type of event log TS.
///
////////////////////////////////////////////////////////////////////////////////


#ifndef XI_LDOPA_TRSS_MODELS_EVENTLOG_TS_H_
#define XI_LDOPA_TRSS_MODELS_EVENTLOG_TS_H_

// #pragma once


// ldopa
#include "xi/ldopa/ts/models/labeledts.h"
#include "xi/ldopa/eventlog/eventlog.h"


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"



namespace xi { namespace ldopa { namespace ts {;   //


/** \brief Declaration of a most base interface type for TS state definition.
 *
 */
class LDOPA_API IStateId {
public:
#pragma region Type Definitions

    /** \brief Type declaration (alias) for an attribute. */
    typedef eventlog::IEventLog::Attribute Attribute;

#pragma endregion // Type Definitions

public:
    /** \brief Virtual Destructor: can be deleted. */
    virtual ~IStateId() {};

public:
    /** \brief Declares equality operation. */
    virtual bool isEqualTo(const IStateId* that) const = 0;

    /** \brief Declares operation <. */
    virtual bool isLessThan(const IStateId* that) const = 0;

    /** \brief Makes a string representation of a state ID. */
    virtual std::string toString() const = 0;

}; // class IStateId



/** \brief Declares an interface for a pool of stateIDs to be used by EventLogTSs.
 *
 *  The pool manages the lifetime of stateIDs, which can be freely aggregated
 *  across several TSs.
 *  
 */
class LDOPA_API IStateIDsPool {
public:

    /** \brief Can be managed by the pointer to this class. */
    virtual ~IStateIDsPool() {}

public:
    //----<Main interface>----
    
    /** \brief Returns a ptr to the initial state ID (special case). */
    virtual const IStateId* getInitStateId() = 0;

    /** \brief Returns a size of the pool. */
    virtual size_t getSize() const = 0;


}; // class IStateIDsPool


/** \brief Define special traits class for IStateId*. */
template<>
class StateID_traits<IStateId*> {
public:
    typedef const IStateId*     TVal;       ///< Value type.
    typedef const IStateId*     TСArg;      ///< Const argument type.
    typedef const IStateId* &   TRef;       ///< Ref-argument type.
    typedef const IStateId*     TCRes;      ///< Constant res.
};


/** \brief Base implementation of a TS built for an event log. 
 *
 *  Manages the lifetime of all state identifiers.
 *
 *  #todo: возможно, сделать копирование пула состояний с указанием в конструкторе (как
 *  это сделано в EventLogTs. Для этого в интерфейсе IStateIDsPool реализовать метод clone().
 */
class LDOPA_API BaseEventLogTS {
public:
#pragma region Type Definitions

    /** \brief Type declaration (alias) for an attribute. */
    typedef eventlog::IEventLog::Attribute Attribute;
    typedef Attribute Activity;         // переходы СП для логов событий помечаются активностями
    typedef Activity TrLabel;           // обобщенный тип — пометка переходов


    /** \brief Defines a specific type of an event log TS. 
     *
     *  No states and transitions property.
     */
    typedef LabeledTS<const IStateId*, Attribute> LogTS;
    typedef LogTS::State State;             ///< State type alias.
    typedef LogTS::StateRes StateRes;       ///< StateRes type alias.
    typedef LogTS::Transition Transition;   ///< Transition type alias.
    typedef LogTS::TransRes TransRes;       ///< Transition Result type alias.

    // state and transition iterators
    typedef LogTS::StateIter StateIter;
    typedef LogTS::OtransIter OtransIter;
    typedef LogTS::ItransIter ItransIter;
    typedef LogTS::TransIter TransIter;

    // state and transition sequencies
    typedef LogTS::StateIterPair  StateIterPair;
    typedef LogTS::OtransIterPair OtransIterPair;
    typedef LogTS::ItransIterPair ItransIterPair;
    typedef LogTS::TransIterPair  TransIterPair;

    typedef unsigned int Uint;              ///< Alias for unsigned int.

#pragma endregion // Type Definitions
public:

    /** \brief Constructor initializes the TS by a pool of events. 
     *
     *  \param stIDsPool is a (possibly shared) pool of state IDs.
     *  \param copyPool determines whether the pool of stateIDs should be cloned (and, hance,
     *  managed locally — if true) or not (false).
     */
    BaseEventLogTS(IStateIDsPool* stIDsPool); // , bool copyPool = false);

    /** \brief Copy constructor. */
    BaseEventLogTS(const BaseEventLogTS& that);
    /** \brief Virtual Destructor. */
    virtual ~BaseEventLogTS();

public:
    /** \brief Assignment operation. */
    BaseEventLogTS& operator=(const BaseEventLogTS& that);
public:
    //----<Basic TS interface>----
    /** \brief Return a number of states in the TS. */
    size_t getStatesNum() const { return _ts->getStatesNum(); }

    /** \brief Return a number of regular states in the TS. */
    size_t getRegStatesNum() const { return _ts->getRegStatesNum(); }

    /** \brief Return a number of anonymous states in the TS. */
    size_t getAnonStatesNum() const { return _ts->getAnonStatesNum(); }

    /** \brief Return a number of edges in the TS. */
    size_t getTransitionsNum() const  { return _ts->getTransitionsNum(); }

    //----<Manipulation with TS (mainly shortcuts)>----

    /** \brief Returns the initial state. */
    State getInitState() const { return _ts->getState(_initStId).first; }

    /** \brief Returns the ID of the initial state. */
    const IStateId* getInitStateID() const { return _initStId; }

    /** \brief For a given state ID \a stId, returns a state in TS.
     *  If no state exists, a new one with the given id is added and returned.
     */
    State getOrAddState(const IStateId* stId) { return _ts->getOrAddState(stId); }

    /** \brief Looks for a state with id \a stId and returns it if exists.
     *
     *  If a state exists, sets a second field of StateRes to true, otherwise sets it to false.
     */
    StateRes getState(const IStateId* stId) { return _ts->getState(stId); }

    /** \brief Returns the ID of the given state \a s. */
    const IStateId* getStateID(State s) { return _ts->getStateID(s); }

    /** \brief Adds a new anonymous state to the TS. */
    State addAnonState() { return _ts->addAnonState();  }

    /** \brief Returns true if a given state \a s is anonymous one, false otherwise (regular one). */
    bool isStateAnon(State s) const
    {
        return _ts->isStateAnon(s);
    }

    /** \brief Returns true if a given state \a s is regular one, false otherwise (anon one). */
    bool isStateRegular(State s) const
    {
        return _ts->isStateRegular(s);
    }

    /** \brief Adds a new unlabeled transition. 
     *
     *  If one need to track all amonymous transitions individually, 
     *  a set (or other mapping) can be added in a derived structure.
     */
    Transition addAnonTrans(State s, State t)
    {
        return _ts->addAnonTrans(s, t);
    }


    /** \brief Gets a transition between given pair of states \a s and \a t,
     *  marked by a label \a lbl.
     *
     *  If no transition exists, creates a new one.
     */
    Transition getOrAddTrans(State s, State t, const Attribute& lbl) { return _ts->getOrAddTrans(s, t, lbl); }


    /** \brief Looks over all parallel transitions between pair of states \a s and \a t
     *  for one labeled with the given label \a lbl. If such a transition exists,
     *  returns true as a second part of a returning value, otherwise returns false there.
     */
    TransRes getTrans(State s, State t, const Attribute& lbl) { return _ts->getTrans(s, t, lbl); }

    /** \brief Looks over all output transitions of a given state \a s for the first
     *  one, which is labeled by the label \a lbl.
     *
     *  \returns If at least one such a transition if found, returns its (arbitrary first).
     */
    TransRes getFirstOutTrans(State s, const Attribute& lbl) { return _ts->getFirstOutTrans(s, lbl); }

    /** \brief Acts like getFirstOutTrans(), but returns the state, to which a found transition lead.
     *
     *  It is just a shortcut for a chain of methods getFirstOutTrans() and getTargState().
     *  \returns if a corresponding transition found, returns a corresponding state and true as a second parameter.
     */
    StateRes getFirstOutState(State s, const Attribute& lbl) { return _ts->getFirstOutState(s, lbl); }

    /** \brief Returns the source State (vertex) of the given transitions \param t. */
    State getSrcState(const Transition& t) const { return _ts->getSrcState(t); }

    /** \brief Returns the target State (vertex) of the given transitions \param t. */
    State getTargState(const Transition& t) const { return _ts->getTargState(t); }

    /** \brief Returns a const ref to the label of the given transition \param t. */
    const Attribute& getTransLbl(const Transition& t) const { return _ts->getBundle(t); }
        
    /** \brief Returns a collection of States (by a pair of state iterators). */
    /*LogTS::*/StateIterPair getStates() { return _ts->getStates(); }

    /** \brief Returns a collection of States (by a pair of state iterators)/const. */
    /*LogTS::*/StateIterPair getStates() const { return _ts->getStates(); }

    /** \brief Returns a collection of Transitions (by a pair of state iterators). */
    /*LogTS::*/TransIterPair getTransitions() { return _ts->getTransitions(); }

    /** \brief Returns a collection of Transitions (by a pair of state iterators) / const. */
    /*LogTS::*/TransIterPair getTransitions() const { return _ts->getTransitions(); }


    /** \brief Shortcut for giving all input transitions of a state \a s. */
    /*LogTS::*/ItransIterPair getInTransitions(State s) { return _ts->getInTransitions(s); }

    /** \brief Shortcut for giving all input transitions of a state \a s (const). */
    /*LogTS::*/ItransIterPair getInTransitions(State s) const { return _ts->getInTransitions(s); }

    /** \brief Shortcut for giving all output transitions of a state \a s. */
    /*LogTS::*/OtransIterPair getOutTransitions(State s) { return _ts->getOutTransitions(s); }

    /** \brief Shortcut for giving all output transitions of a state \a s (const). */
    /*LogTS::*/OtransIterPair getOutTransitions(State s) const { return _ts->getOutTransitions(s); }


    /** \brief Shortcut for removing the given state \a s. */
    void removeState(State s) { _ts->removeState(s); }

    /** \brief Shortcut for removing the given transition \a t. */
    void removeTrans(const Transition& t) { _ts->removeTrans(t); }

    /** \brief Returns the numbers of input transitions of a state \a s. */
    size_t getInTransNum(State s) const { return _ts->getInTransNum(s); }

    /** \brief Returns the numbers of output transitions of a state \a s. */
    size_t getOutTransNum(State s) const { return _ts->getOutTransNum(s); }

    /** \brief Returns true if the given state \a s has input transitions, otherwise false. */
    bool hasInTransitions(State s) const { return _ts->hasInTransitions(s); }

    /** \brief Returns true if the given state \a s has output transitions, otherwise false. */
    bool hasOutTransitions(State s) const { return _ts->hasOutTransitions(s); }

public:
    //----<Helpers>----
    
    /** \brief Makes a string for the ID of the given state \param st. */
    std::string makeStateIDStr(State st) const
    {
        // UPD: 23.08.2018: для анонимных состояний возвращаем пустую строку покамест.
        if (isStateAnon(st))
            return std::string();

        LogTS::StateIDCRes stId = _ts->getStateID(st);
        return stId->toString();
        //return _ts->getStateID(st)->toString();
    }

public:
    //----<Special setters and getters>----
        
    LogTS& getLts() { return *_ts; }                    ///< Returns the underlying labeled TS.
    const LogTS& getLts() const { return *_ts; }        ///< Returns the underlying const labeled TS.

    /** \brief Returns the associated (when constructed) stateIDs pool. */
    IStateIDsPool* getStateIDsPool() const { return _stIDsPool; }


    /** \brief Returns a number of traces for this TS. */
    Uint getTracesNum() const { return _tracesNum; }
    
    /** \brief Sets a number of traces for this TS. */
    void setTracesNum(Uint tn) { _tracesNum = tn; }

protected:
    //----<Construction Helpers>----

    /** \brief Swap method for assignment operator. */
    static void swap(BaseEventLogTS& lhv, BaseEventLogTS& rhv);

protected:
    
    /** \brief Initial state ID. */
    const IStateId* _initStId;

    /** \brief Common pointer to an underlying TS. */
    LogTS* _ts;

    /** \brief Ptr to the associated stateIDs pool. */
    IStateIDsPool* _stIDsPool;

    // additional attributes
    
    /** \brief Number of traces that are used to constitute this TS. */
    Uint _tracesNum;
}; // class BaseEventLogTS


}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_MODELS_EVENTLOG_TS_H_
