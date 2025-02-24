#ifndef XI_LDOPA_TRSS_TSBUILDER_H_
#define XI_LDOPA_TRSS_TSBUILDER_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/eventlog/obsolete1/eventlog.h"
#include "xi/ldopa/ts/models/obsolete1/eventlogts.h"

namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //


/** \brief Transition system builder class
 *
 *  Builds a TS from an event log using prefix, postfix or infix strategy with fixed window size.
 */
class LDOPA_API EventLogFixWsTsBuilder
{
public:
    //-----<Types>-----
    /** \brief Type for window size */
    typedef unsigned short WindowSize;

    /** \brief Shared_ptr for ITraceEnum object */
    typedef std::shared_ptr<eventlog::obsolete1::ITraces::ITraceEnumerator> ITraceEnumShPtr;
    typedef std::shared_ptr<eventlog::obsolete1::ITrace::IEventEnumerator> IEventEnumShPtr;

    typedef eventlog::obsolete1::IEventLog IEventLog;
    typedef eventlog::obsolete1::ITrace ITrace;
    typedef eventlog::obsolete1::IEvent IEvent;

public:
    //-----<Consts>-----
    static const WindowSize WS_INF = -1;
public:

    /** \brief Defalt constructor. */
    EventLogFixWsTsBuilder();

    /** \brief Constructs with an event log. */
    EventLogFixWsTsBuilder(IEventLog* evLog);
    ~EventLogFixWsTsBuilder();
protected:
    // Lockes CC and CO
    EventLogFixWsTsBuilder(const EventLogFixWsTsBuilder&);
    EventLogFixWsTsBuilder& operator= (EventLogFixWsTsBuilder&);

public:

    /** \brief Build a TS from the log that has been set, and a set of options */
    EventLogTs* build(WindowSize ws = WS_INF);

   
    /** \brief Resets a buider before another TS building */
    void reset() { resetInternal(); };

    /** \brief Marks a res TS as detached, i.e. it will not be deleted by the class */
    EventLogTs* detachRes() { _resDetached = true; return _resTs; };

    /** \brief Returns the resulting transition system */
    EventLogTs* getResTs() const { return _resTs; }

    /** \brief Returns true if a resulting TS has been detached */
    bool isResDetached() const { return _resDetached;  }
public:

    /** \brief Get and event log. */
    IEventLog* getLog() const { return _evLog; }

    /** \brief Set a new event log. For built TS resets its. */
    void setLog(IEventLog* evLog);


public:
    /** \brief Creates a state ID based on a given trace \a tr, current position \a ind and  
     *  a windows size \a ws using a pool of activities \a actvsPool
     *
     *  Possition \a ind and WS \a ws recalculated to a \a begPos and \a endPos and
     *  an overloaded version of makeStateId() is called. See it to know how 
     *  \a begPos and \a endPos are used to determine a range of events.
     *
     *  Used move semantics for TsPtrStateId! So, don't be affraid to use it.
     *  Also, use std::move() for moving operations (for example, see TEST(TsPtrStateId1, 
     *  moveSemantics1))
     *
     *  For the method, to be effective, effectiveness of ITrace::operator[] is a need.
     *  Here, \a tr cannot be constant as it normally changes the state of a trace 
     *  (for example, CSVLog trace is "prepared" during its first invokation)
     */
    static TsPtrStateId makeStateId(ITrace* tr, WindowSize ws, WindowSize ind,
         strutils::SetStrPool& actvsPool);

    /** \brief Logical sibling of makeStateId() method
     *
     *  Obtains id object as a parameter and begin \a beg and end \a end positions.
     *  Resulting state includes all events belong to a set [beg; end), so as it
     *  normally occures with sequence iterator concept
     */
    static void makeStateId(TsPtrStateId& id, ITrace* tr,
        WindowSize beg, WindowSize end,  strutils::SetStrPool& actvsPool);

protected:

    /** \brief Internal version of makeStateId() (2)  */
    static void makeStateIdInternal(TsPtrStateId& id,  ITrace* tr,
        WindowSize beg, WindowSize end,  strutils::SetStrPool& actvsPool);


    /** \brief Internal version of reset() */
    void resetInternal();

    /** \brief Builds a branch in a TS tree for a given trace \a tr */
    void processTrace(ITrace* tr);
    
    /** \brief For a given pair of vertices add (if no exists) a new transition and increase its freq
     *  attribute to a value of \a freqToAdd
     */
    void addTransitionWithFreq(BaseListBGraph::Vertex from, BaseListBGraph::Vertex to, 
        const std::string* actName, int freqToAdd);
protected:

    /** \brief Stores a ptr to an event log */
    IEventLog* _evLog;

    /** \brief Stores window size */
    WindowSize _ws;

    /** \brief Stores a ptr to a resulting TS */
    EventLogTs* _resTs;

    /** \brief Flag retermines is the resulting TS is detached, i.e. the TS should not be deleted */
    bool _resDetached;

}; // class EventLogFixWsTsBuilder



}}}} // namespace xi::ldopa::ts::obsolete1



#endif // XI_LDOPA_TRSS_TSBUILDER_H_
