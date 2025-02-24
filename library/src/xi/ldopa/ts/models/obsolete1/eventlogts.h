///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Contains definitions for transition systems constructed for event logs
///
/// -
///
///////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_MODELS_OBSOLETE1_EVENTLOGTS_H_
#define XI_LDOPA_TRSS_MODELS_OBSOLETE1_EVENTLOGTS_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

#include "xi/ldopa/ts/models/obsolete1/basets.h"
#include "xi/ldopa/graphs/boost/bgl_graph_adapters.h"



// xilib
#include "xi/strutils/set_str_pool.h"

// boost
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

// std
#include <map>
#include <bitset>

namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //


// экспорт конкретизированных шаблонов в DLL, это должно быть ДО использования 
// в нижележащих классах (https://support.microsoft.com/en-us/kb/168958)
// тж. см. возможную проблему https://s.pais.hse.ru/redmine/issues/428
#ifdef LDOPA_DLL


#endif // LDOPA_DLL


/** \brief Defines a variety of TS for Event Logs
 *
 *  For convenience reasons here we use State and Transition aliases for Vertex and Edge
 *  \TODO: надо задокументировать, какие данные логируются здесь
 */
class LDOPA_API EventLogTs : public BaseTs
{
public:

    /** \brief Bimap type reflexing stateID's and Vertices of a TS */
    typedef boost::bimap<   TsPtrStateId, 
                            State
                        > StateIdVertexBimap;
    
    typedef StateIdVertexBimap::value_type StateIdVertexBimapPair;
    typedef StateIdVertexBimap::left_const_iterator StateIdVertexBimapStidConstIter;
    typedef StateIdVertexBimap::right_const_iterator StateIdVertexBimapVertexConstIter;


    //strutils::SetStrPool::ElemPtr
    /** \brief Type for map of edges to string ptrs (in string pool) */
    typedef std::map<Transition, const std::string*> TransActivitiesMap;

    /** \brief Type for mapping edges to integers. Mind \a unordered_map issue (see EdgePstrMap) */
    typedef std::map<Transition, int> TransIntMap;

    /** \brief Typedef for a returning result containing possible non-existing integer */
    typedef std::pair<int, bool> IntRetResult;


    /** \brief A wrapper class for a bitset for flags-for-a-state type */
    class StateFlags : public std::bitset<2> 
    {
    public:
        //-----<Const>-----
        /** \brief List of pseudo-constants for numbering bits in bitset */
        enum {
            FL_ACCEPTING = 0,           ///< accepting state
            FL_ACCEPTING_SET,           ///< true, if accepting flag is set, false otherwise
        }; // anonymous enums for enumerating flags

        /** \brief Enumeration for tri-state flags */
        enum TriState {
            triNotSet,                   ///< Flag is not set
            triTrue,                     ///< Flag set to true (is `set')
            triFalse                     ///< Flag set to false (is `reset')
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

        void clearAccepting() { set(FL_ACCEPTING_SET, false);  }

    }; // class StateFlags

    /** \brief Mapping of states to state flags */
    typedef std::map<State, StateFlags> StateFlagsMap;
    //

public:

    /** \brief Default constructor */
    EventLogTs();

    /** \brief Creates an TS with given activity pool
     *  \param actPool a source activity pool
     *  \param copyActPool determines if an activity pool should be copied (see _manageActPoolInternal
     *  for more details)
     */
    EventLogTs(strutils::SetStrPool* actPool, bool copyActPool = true);

    /** \brief Destructor */
    ~EventLogTs();

protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    EventLogTs(const EventLogTs&);                 // Prevent copy-construction
    EventLogTs& operator=(const EventLogTs&);      // Prevent assignment
public:
    /** \brief For a given state ID \a id, returns a vertex in the underlying graph. 
     *  If no vertex exists, a new one is added and returned.
     */
    State getOrAddNewStateById(const TsPtrStateId& id);

    /** \brief Returns a state with ID that is equal to \a id. If no such state exists, returned boolean 
     *  flag is set to false
     */
    StateRetResult getState(const TsPtrStateId& id);


    /** \brief Returns init state's vertex and a flag if the vertex is valid (true) of has not 
     *  been set (false)
     */
    StateRetResult getInitState() const { return std::make_pair(_initState, _hasInitState); }

    /** \brief Sets an init state to a given vertex. If a vertex is not in the graph, an exception is thrown */
    void setInitState(State vert);

    /** \brief Returns existing or create new (if not exists) a state that is marked as initial 
     *
     *  If no init state created yet, creates a new one with default (empty) state ID.
     *  If there are a state with default ID yet existing, an exception is thrown.
     */
    State getOrAddNewInitState();
    //BaseListBGraph::Vertex getOrAddNewInitState();


    /** \brief Gets a transition between given pair of vertices, marked be \a symb 
     *  If no transition exists, creates a new one
     *
     *  If a transition between vertices exists and marked but another symbol,
     *  a flag \a changeSymbIfExists determines if the symbol should be overwritten by \a symb (true)
     *  or not (false)
     *  Since transition are marked strictly by symbols from the single pool,
     *  only pointers are compared, not the strings itselves
     */
    Transition getOrAddNewTransition(State from, State to, 
        const std::string* symb, bool changeSymbIfExists = false);//, GetAddTransPolicy gatp = gatpAddNew);


    /** \brief Returns the assigned pool of activities */
    xi::strutils::SetStrPool* getActivitiesPool() const { return _activitiesPool; }

public:
    //-----<Transition attributes>-----

    /** \brief Returns the map of edges to string-pointers */
    TransActivitiesMap& getTransActivitiesMap() { return _transActivitiesMap;  }

    /** \brief Const overloaded version of getEdgePsrMap() */
    const TransActivitiesMap& getTransActivitiesMap() const { return _transActivitiesMap; }

    /** \brief Returns Activity mark of a given edge. If no edge or ID, nullptr is returned */
    const std::string* getTransActivity(Transition tr) const;
    //const std::string* getTransActivity(BaseListBGraph::Edge edge) const;

    /** \brief Returns the map of edges to string-pointers */
    TransIntMap& getTransFreqMap() { return _transFreqsMap; }

    /** \brief Const overloaded version of getEdgePsrMap() */
    const TransIntMap& getTransFreqMap() const { return _transFreqsMap; }

    /** \brief Returns a `freq' attribute for a given edge, if it exists*/
    IntRetResult getTransFreq(Transition tr) const;
    //IntRetResult getTransFreq(BaseListBGraph::Edge edge) const;

    /** \brief Sets a `freq' attribute for a given edge */
    //void setTransFreq(BaseListBGraph::Edge edge, int freq)
    void setTransFreq(Transition tr, int freq)
    {
        _transFreqsMap[tr] = freq;
    }

public:
    //-----<State attributes>-----

    /** \brief Returns the underlying bimap */
    StateIdVertexBimap& getStIdVertBimap() { return _stid2verts; };

    /** \brief Const overloaded version of getSidVertBimap() */
    const StateIdVertexBimap& getStIdVertBimap() const { return _stid2verts; };

    /** \brief Returns a flags object for a given state 
     *  \param st a state
     *  \param fl an ref to a flag object for returning flags through
     *  \return true if flags object has been set, false otherwise
     */
    bool getStateFlags(State st, StateFlags& fl) const;

    /** \brief Sets a flags object for a given state */
    void setStateFlags(State st, const StateFlags& fl) {  _stateFlags[st] = fl; }

    /** \brief For a given state returns if exists or firstly create and returns a flags object */
    StateFlags& getOrAddNewStateFlags(State st) { return _stateFlags[st]; };

    /** \brief Gets a const ref to a ID-object of a given state \a st  */
    const TsPtrStateId& getStateId(State st) ; // { return _stid2verts.right[st]; };
public:
    //-----<TS attributes>-----

    /** \brief Gets TS's name (attribute) */
    std::string getName() const { return _name; }

    /** \brief Sets TS's name (attribute) */
    void setName(const std::string& nm) { _name = nm; }

public:
    /** \brief Makes a string label for a given state */
    std::string makeStateLabel(State st);
protected:  
    /** \brief Adds a new state \a id to the TS and return a corresponding vertex 
     *  in the underlying graph.
     *
     *  Internal implementation w/o any excess checkings
     * \TODO // здесь надо реализовать семантику перемещения
     * \TODO возможно тут надо более общий класс передавать
     */
    State addNewStateInternal(const TsPtrStateId& id);
    //BaseListBGraph::Vertex addNewStateInternal(const TsPtrStateId& id);

    /** \brief Adds a transition between \a from state and \a to state, marked by \a symb symbol 
     *
     *  As multiple transitions between a pair of states are allowed, even if a transition already 
     *  exists, a new one is added anyway
     */
    Transition addTransitionInternal(State from, State to, const std::string* symb);


protected:
    /** \brief Stores a pool of string ids of activities 
     *
     *  Can be either independent or shared with base log
     */
    xi::strutils::SetStrPool* _activitiesPool;

    /** \brief Determines if the activity pool \a _activitiesPool should be managed internally 
     *
     *  If true, a TS object has an independent pool and should free it manually. Otherwise,
     *  the pointer points to a shared activity pool (normally, managed by an Event Log).
     *  So, memory should not be managed manully. Moreover, at some point the pool may get invalidated
     */
    bool _manageActPoolInternal;

    /** \brief Stores a dedicated state's vertex, which is initial state */
    BaseListBGraph::Vertex _initState;

    /** \brief Flag determines if an init state has been set for the TS */
    bool _hasInitState;

    /** \brief Stores StateIDs to Vertices bimap */
    StateIdVertexBimap _stid2verts;

    /** \brief Stores a map of edges to activities */
    TransActivitiesMap _transActivitiesMap;

    /** \brief Stores a map of edges to assigned frequencies */
    TransIntMap _transFreqsMap;

    /** \brief Represents a state flags */
    StateFlagsMap _stateFlags;

    /** \brief TS name: just let it be */
    std::string _name;
}; // class EventLogTs

}}}} // namespace xi::ldopa::ts::obsolete1


#endif // XI_LDOPA_TRSS_MODELS_OBSOLETE1_EVENTLOGTS_H_
