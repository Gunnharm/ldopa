////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA EventLog Library
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      23.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Common definition for EventLog lib Version 2.
///
////////////////////////////////////////////////////////////////////////////////


#ifndef XI_LDOPA_EVENTLOG_SQLITE_EVENTLOG2_H_
#define XI_LDOPA_EVENTLOG_SQLITE_EVENTLOG2_H_

#pragma once

// std
#include <map>
//#include <string>

// xilib is a need
#include "xi/collections/enumerators.hpp"
#include "xi/attributes/boost_attr.h"

#include "xi/strutils/set_str_pool.h"

// ldopa
#include "xi/ldopa/ldopa_dll.h"
#include "xi/ldopa/utils.h"


/** \brief 2nd version of an Event Log interface. */
namespace xi { namespace ldopa { namespace eventlog {;   //


/** \brief Auxilliary class for event log interfaces. */
class LDOPA_API IEventLog_traits {
public:
    /** \brief Unsigned int for numerating 0-based positions. */
    typedef unsigned int UInt;

    /** \brief Log attribute type. */
    typedef ::xi::attributes::VarAttribute1 Attribute;

    /** \brief Tpe for Log attribute with name (id). */
    typedef std::pair<std::string, Attribute> NamedAttribute;

    /** \brief Declares a map of attributes (id — attribute). */
    typedef std::map < std::string, Attribute > AttributesMap;

    /** \brief Typedef for a structure with an attribute that can be switched off.  */
    typedef std::pair<Attribute, bool> AttributeRes;
}; // class IEventLog2_traits


}}} // namespace xi { namespace ldopa  { namespace eventlog


//==============================================================================


/** \brief Partial template specialization for NamedAttribute.
 *
 *  Must be defined outside of xi:ldopa namespace
 */
template <>
class xi::collections::IConstEnumerator_traits<xi::ldopa::eventlog::IEventLog_traits::NamedAttribute> {
public:
    typedef xi::ldopa::eventlog::IEventLog_traits::NamedAttribute   TRet;    ///< Return type.
};

//==============================================================================


namespace xi { namespace ldopa { namespace eventlog {;   //


typedef xi::collections::IConstEnumerator<IEventLog_traits::NamedAttribute> 
    IAttributesEnumerator;
class LDOPA_API IEventLog;
//class /*LDOPA_API*/ IAttributesEnumerator;
class LDOPA_API IEvent;

//==============================================================================
// class IEventTrace2
//==============================================================================

/** \brief New approach to have an objected version of event log traces.
 *
 *  -
 */
class LDOPA_API IEventTrace {
public:
    //----<Types>-----

    /** \brief Unsigned integer type. */
    typedef IEventLog_traits::UInt UInt;

    /** \brief Log attribute type. */
    typedef IEventLog_traits::Attribute Attribute;

public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr. */
    virtual ~IEventTrace() { };
public:
    //-----<Main interface>-----
    
    /** \brief Returns a number of trace attributes.
     *
     *  If an event log does not allow to obtain a number of trace attributes (possibly due to too
     *  complex algorithm of calculation), returns -1.
     */
    virtual int getAttrsNum() = 0;

    /** \brief Extract single attribute with id \a id from this trace.
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     */
    virtual bool getAttr(const char* id, Attribute& a) = 0;

    /** \brief Extracts all trace attributes and iterates all over them.
     *
     *  \returns the iterable enumerator with attributes. The enumerator must
     *  be freed by a caller.
     */
    virtual IAttributesEnumerator* getAttrs() = 0;


    /** \brief Returns an event object with the num \a eventNum (0-based).
     *
     *  Method checks if the event number does not exceed the total number of events
     *  returned by getSize() method; if so, returns nullptr.
     *  If a log is not open, returns nullptr.
     *
     *  Returned event object is managed by a log object and
     *  must not be deleted by a caller.
     */
    virtual IEvent* getEvent(UInt eventNum) = 0;


    /** \brief Returns a number of events that this trace contains. */
    virtual int getSize() = 0;

    /** \brief Returns an event log this trace is a part of. */
    virtual IEventLog* getLog() = 0;

}; // class IEventTrace2


//==============================================================================
// class IEvent2
//==============================================================================

/** \brief New approach to have an objected version of event log events.
 *
 *  -
 */
class LDOPA_API IEvent {
public:
    //----<Types>-----

    /** \brief Log attribute type. */
    typedef IEventLog_traits::Attribute Attribute;

public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr. */
    virtual ~IEvent() { };
public:
    //-----<Main interface>-----

    /** \brief Extract single attribute with id \a id from this event.
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     *
     *  If the log is not open, returns false.
     */
    virtual bool getAttr(const char* id, Attribute& a) = 0;

    /** \brief Returns a number of attributes in the event. 
     * 
     *  Is for some reasons the number of attributes can't be given, returns -1.
     */
    virtual int getAttrsNum() = 0;


    /** \brief Returns all event attributes in the form of attribute enumerator.
     *
     *  \returns the iterable enumerator with attributes. 
     *  The enumerator must be deleted by a caller.
     */
    virtual IAttributesEnumerator* getAttrs() = 0;

    /** \brief Returns a trace this event is a part of. */
    virtual IEventTrace* getTrace() = 0;
}; // class IEvent2


class LDOPA_API IEventLog {
public:
    //----<Types>-----
    
    // TODO: везде 0-based позицию на UInt надо переделать!

    /** \brief Unsigned int for numerating 0-based positions. */
    typedef IEventLog_traits::UInt UInt;

    /** \brief Log attribute type. */
    typedef IEventLog_traits::Attribute Attribute;
    //typedef ::xi::attributes::VarAttribute1 Attribute;
    
    /** \brief Type for Log attribute with name (id). */
    typedef IEventLog_traits::NamedAttribute NamedAttribute;
    //typedef std::pair<std::string, Attribute> NamedAttribute;

    /** \brief Type for a structure with an attribute that can be switched off.  */
    typedef IEventLog_traits::AttributeRes AttributeRes;
public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr. */
    virtual ~IEventLog() { };

public:
    //-----<Main interface>-----

    /** \brief Open a log and prepares it to work. 
     *
     *  If a log is already open, does nothing.
     */
    virtual void open() = 0;

    /** \brief Close the log. 
     *
     *  If no log opened, does nothing.
     */
    virtual void close() = 0;

    /** \brief Returns true if a log is open and ready to work; false otherwise. */
    virtual bool isOpen() = 0;

public:
    //-----<Main interface>-----
    
    /** \brief Returns a total number of events in the event log.
     *
     *  If an event log does not allow to obtain a number of events (possibly due to too 
     *  complex algorithm of calculation), returns -1.
     *  
     *  If the log is not open, returns 0.
     */
    virtual int getEventsNum() = 0;

    /** \brief Returns a total number of cases in the event log.
     *
     *  If an event log does not allow to obtain a number of cases (possibly due to too
     *  complex algorithm of calculation), returns -1.
     *
     *  If the log is not open, returns 0.
     */
    virtual int getTracesNum() = 0;

    /** \brief Returns a total number of all activity classes (activities).
     *
     *  If an event log does not allow to obtain a number of activities (possibly due to too
     *  complex algorithm of calculation), returns -1.
     *
     *  If the log is not open, returns 0.
     */
    virtual int getActivitiesNum() = 0;

    /** \brief Returns a number of log attributes.
     *
     *  If an event log does not allow to obtain a number of attributes (possibly due to too
     *  complex algorithm of calculation), returns -1.
     *
     *  If the log is not open, returns 0.
     */
    virtual int getLogAttrsNum() = 0;

    /** \brief Extract attribute associated with the log, with id \a id.
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     *
     *  If the log is not open, returns false.
     */
    virtual bool getLogAttr(const char* id, Attribute& a) = 0;

    /** \brief Extracts all log attributes and iterates all over them.
     *
     *  \returns the iterable enumerator with attributes. The enumerator must
     *  be freed by a caller.
     */
    virtual IAttributesEnumerator* getLogAttrs() = 0;


    /** \brief Extract attribute with id \param id associated with a trace \param traceNum.
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     *
     *  If the log is not open, returns false.
     */
    // virtual bool getLogAttr(int traceNum, const char* id, Attribute& a) = 0; // TODO вернуться сюда

    /** \brief Extract single attribute with id \a id associated with an event at
     *  \param eventNum offset (0-based) in a trace \a traceNum offset (0-based).
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     *
     *  If the log is not open, returns false.
     */
    virtual bool getEventAttr(int traceNum, int eventNum, const char* id, Attribute& a) = 0;


    /** \brief Extract single attribute with id \a id associated with an event that 
     *  is identified by uniq param \a eventId.
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     *
     *  If the log is not open, returns false.
     */
    virtual bool getEventAttr(int eventId, const char* id, Attribute& a) = 0;

    /** \brief Extracts all events attributes associated with an event at
     *  \a eventNum offset (0-based) in a trace \a traceNum offset (0-based)
     *  and iterates all over them.
     *
     *  \returns the iterable enumerator with attributes. The enumerator must
     *  be freed by a caller.
     */
    virtual IAttributesEnumerator* getEventAttrs(int traceNum, int eventNum) = 0;

    /** \brief Extracts all event attributes associated with an event with 
     *  id \a id associated with an event that is identified by uniq 
     *  param \a eventId.
     *
     *  \returns the iterable enumerator with attributes. The enumerator must
     *  be freed by a caller.
     */
    virtual IAttributesEnumerator* getEventAttrs(int eventId) = 0;

    /** \brief Extract single attribute with id \a id associated with a trace 
     *   \a traceNum offset (0-based). 
     *
     *  \param[out] a — extracted attribute if done.
     *  \return true if an attribute exists and successfully extracted, false otherwise.
     *  Here and everywhere trace num is the same as trace id.
     *
     *  If the log is not open, returns false.
     */
    virtual bool getTraceAttr(int traceNum, const char* id, Attribute& a) = 0;

    /** \brief Extracts all trace attributes associated with a trace 
     *  that is identified by uniq id \a traceNum (0-based).
     *
     *  \returns the iterable enumerator with attributes. The enumerator must
     *  be freed by a caller.
     */
    virtual IAttributesEnumerator* getTraceAttrs(int traceNum) = 0;

    /** \brief Returns a number of events that the given trace \a traceNum contains. 
     *
     *  If the log is not open, returns 0.
     */
    virtual int getTraceSize(int traceNum) = 0;

    /** \brief Returns an Event Activity Attribute ID if it provided by the log. */
    virtual std::string getEvActAttrId() const = 0;

    /** \brief Returns an Event Timestamp Attribute ID if it provided by the log. */
    virtual std::string getEvTimestAttrId() const = 0;

    /** \brief Returns an Event Case Attribute ID if it provided by the log. */
    virtual std::string getEvCaseAttrId() const = 0;

    /** \brief Gets some information string about custom log (name of the file e.g.). */
    virtual std::string getInfoStr() const = 0;

    #pragma region Object-oriented interface

    /** \brief Returns a trace object for a trace with the num \a traceNum (0-based).
     *
     *  Method checks if the trace number does not exceed the total number of traces,
     *  if so, returns nullptr.
     *  If a log is not open, returns nullptr.
     *  
     *  Returned event object is managed by a log object and
     *  must not be deleted by a caller.
     */
    virtual IEventTrace* getTrace(int traceNum) = 0;

    #pragma endregion // Object-oriented interface

}; // class IEventLog2


/** \brief Base Attributes Enumerator defining some additional datatype aliases. */
class /*LDOPA_API*/ BaseAttributesEnumerator :
    public IAttributesEnumerator
{    
public:
    typedef IEventLog::Attribute Attribute;
    typedef IEventLog::NamedAttribute NamedAttribute;
}; // class BaseAttributesEnumerator


}}} // namespace xi { namespace ldopa { namespace ts {



#endif // XI_LDOPA_EVENTLOG_SQLITE_EVENTLOG2_H_
