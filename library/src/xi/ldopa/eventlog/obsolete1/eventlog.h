////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA EventLog Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      23.02.2016
/// \copyright (c) xidv.ru 2014–2016.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Common definition for EventLog lib
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_EVENTLOG_EVENTLOG_H_
#define XI_LDOPA_EVENTLOG_EVENTLOG_H_

#pragma once

// str
#include <string>

// xilib is a need
#include "xi/collections/enumerators_obsolete1.hpp"     
#include "xi/strutils/set_str_pool.h"

// ldopa
#include "xi/ldopa/ldopa_dll.h"
#include "xi/ldopa/utils.h"



namespace xi { namespace ldopa { namespace eventlog { namespace obsolete1 {;   //

//==============================================================================

class ITrace;

/** \brief Declares an interface for a event object.
 *
 *  -
 */
class LDOPA_API IEvent
{
public:
    //-----<>-----
    
    /** \brief Error code for the case where it is impossible to extract the attribute directrly 
     *  through event. It means, an appropriate method from IEventLog should be used instead.
     */
    static const LdopaException::ErrorCode ERC_CANT_EXTR_ATR_DIR = LdopaException::ERC_USER + 1;
public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr */
    virtual ~IEvent() {}
public:

    /** \brief Declares operation as it declares Compare concept 
     *
     *  For compare concept see http://en.cppreference.com/w/cpp/concept/Compare .
     *  lhv (given as this) is compared with \a rhv.
     *  \TODO rename to a isLessThan().
     */
    virtual bool compare(IEvent* rhv) = 0;
public:


    /** \brief Sets event's owner trace to the given \a tr.
     *
     *  To date, event can be inserted (assigned to) only into one trace and only once. 
     *  So, if the event has already had an owner trace, an exception will be thrown
     */
    virtual void setOwnerTrace(ITrace* tr) = 0;

    /** \brief Returns a trace the event belongs to 
     *
     *  DISCUSSION: а если конкретная реализация не хранит этот указатель, что надо делать?
     *      возвращать ноль или кидать эксцепцию?.. и как реагировать клиенту на ноль, считать
     *      это нормой или аналогом той же искл ситуации?
     */
    virtual ITrace* getOwnerTrace() = 0;

public:
    //-----<Attributes>-----  

    /** \brief Returns a string with a name of the event's activity */
    virtual std::string getActivityName() = 0;

    /** \brief Extracts attribute with number \a aNum as a string and puts it into \a str
     *  \param aNum index number of attribute
     *  \param str a string extracted attribute is to put into
     *  \return true, is an extraction is succeful; false otherwise (e.g. attribute does not exist
     *  or indexing is not allowed)
     *
     *  DISCUSSION: если у ивента не хватает собственных знаний, чтобы вернуть атрибут, возможно
     *      стоит воспользоваться методом IEventLog::getEventAttrAsStr()...
     *  Is event cannot directrly implement the method, an exception with error code
     *  that is equal to ERC_CANT_EXTR_ATR_DIR can throw.
     */
    virtual bool getAttrAsStr(short aNum, std::string& str) = 0;
}; // class IEvent


//==============================================================================

class LDOPA_API ITraces;

/** \brief Declares event log trace object interface
 *
 *  -
 */
class LDOPA_API ITrace
{
public:
    //-----<Types>-----

    typedef collections::IEnumerator_obsolete1<IEvent> IEventEnumerator;
    typedef collections::IConstEnumerator_obsolete1<IEvent> IEventConstEnumerator;
    
protected:
    /** \brief Protected Destructor: It is not intended to delete an object through this interface! */
    ~ITrace() {};
public:
    /** \brief Returns a number of events in a case*/
    virtual size_t getEventsNum() = 0;
    
    /** \brief Inserts given event \a ev into current trace in compliance with event order 
     * and return the event.
     *  \param ev an event to insert
     *  \return inserted event \a ev
     */
    virtual IEvent* insertEvent(IEvent* ev) = 0;

    /** \brief Returns the i-th event  */
    virtual IEvent* operator[] (size_t i) = 0;

    /** \brief Returns enumerator for enumerating all events (enumerator should be deleted after use by caller) */
    virtual IEventEnumerator* enumerateEvents() = 0;

    /** \brief Const overloaded version of enumerateEvents() */
    virtual IEventConstEnumerator* enumerateEventsConst() const = 0;

    /** \brief Returns a traces object
     *
     *  DISCUSSION: те же вопросы, что и для IEvent
     */
    virtual ITraces* getOwnerTraces() = 0;
}; // class ITrace 



//==============================================================================


class LDOPA_API ITraces;

/** \brief Interface declares an event log object.
*
*  -
*/
class LDOPA_API IEventLog //BaseLog
{
public:
    //-----<Event Log specific Types>-----
    /** \brief Typedef for representing CaseID attribute 
     *  
     *  First real experiment on Rubin's logs shows that simple unsigned int is not enough!
     */
    typedef unsigned long long CaseID;

public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr */
    virtual ~IEventLog() { };

public:
    //-----<Main interface>-----
    
    /** \brief Open a log and prepares it to work. \returns true if a log is open, false otherwise */
    virtual bool open() = 0;

    /** \brief Close the log */
    virtual void close() = 0;

    /** \brief Returns true if a log is open and ready to work; false otherwise. */
    virtual bool isOpen() = 0;

    /** \brief Return log's traces */
    virtual ITraces* getTraces() = 0;

    /** \brief Returns a ptr to a pool of activities 
     *
     *  The method first was not intendent to declare, but later it turned out that algorithms are needed for this method
     */
    virtual xi::strutils::SetStrPool* getActivitiesPool() = 0;
}; // class IEventLog


//==============================================================================

/** \brief Declares an interface for a collection of traces.
*
*  -
*/
class LDOPA_API ITraces
{
public:
    //-----<Types>-----
    typedef collections::IEnumerator_obsolete1<ITrace> ITraceEnumerator;
    typedef collections::IConstEnumerator_obsolete1<ITrace> ITraceConstEnumerator;

protected:
    /** \brief Protected Destructor: It is not intended to delete an object through this interface! */
    ~ITraces() {};

public:

    /** \brief Inserts given event \a ev for the given Case ID \a caseId and return the event.
     *  \param caseId a Case ID the event is inserted in
     *  \param ev an event to insert
     *  \return a trace the event \a ev is inserted into
     */
    virtual ITrace* insertEvent(IEventLog::CaseID caseId, IEvent* ev) = 0;
    //virtual IEvent* insertEvent(IEventLog::CaseID caseId, IEvent* ev) = 0;

    /** \brief Return a number of traces in the log.
     *
     *  If a method is not supported, an exception is thrown.
     *  If a number of traces cannot be counted for some reasons (e.g. a log is not open), an exception can (or must?) be thrown.
     */
    virtual size_t getNumOfTraces() = 0;

    /** \brief Returns a trace for a given case \a caseId. 
     *
     *  If no corresponding trace exists, nullptr is returned 
     */
    virtual ITrace* operator[] (IEventLog::CaseID caseId) = 0;

    /** \brief Returns enumerator for enumerating all traces (enumerator should be 
     *  deleted after use by caller) 
     */
    virtual ITraceEnumerator* enumerateTraces() = 0;

    /** \brief Const overloaded version of enumerateEvents() */
    virtual ITraceConstEnumerator* enumerateTracesConst() const = 0;

    /** \brief Returns an onwer log
     *
     *  DISCUSSION: те же вопросы, что и для IEvent
     */
    virtual IEventLog* getOwnerLog() = 0;


}; // class ITraces


}}}} // namespace xi::ldopa::eventlog::obsolete1



#endif // XI_LDOPA_EVENTLOG_EVENTLOG_H_
