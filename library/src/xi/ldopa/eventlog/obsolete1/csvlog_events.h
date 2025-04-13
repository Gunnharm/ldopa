///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief A few out-of-the-box (OOTB) event types and helper types 
/// (e.g. creators) for CSV log.
///
/// -
///
///////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_EVENTLOG_CSVLOGEVENTS_H_
#define XI_LDOPA_EVENTLOG_CSVLOGEVENTS_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"


// ldopa.log
#include "xi/ldopa/eventlog/obsolete1/csvlog.h"

namespace xi { namespace ldopa { namespace eventlog { namespace obsolete1 {;   //


/** \brief Represents a aimple event in a CSV-based log
 *  using relative integer position for representing timestamps.
 */
class LDOPA_API SimpleCSVLogEvent : public IEvent
{
public:
    /** \brief Default constructor */
    SimpleCSVLogEvent();

    /** \brief Init constructror */
    SimpleCSVLogEvent(const std::string* attrAct, std::streamoff ofs, int timest);

    /** \brief Virtual destructor */
    virtual ~SimpleCSVLogEvent() {}
public:
    //-----<IEvent:: implementations>-----
    virtual bool compare(IEvent* rhv) override;
    virtual bool getAttrAsStr(short aNum, std::string& str) override;
    virtual void setOwnerTrace(ITrace* tr) override;
    virtual ITrace* getOwnerTrace() override;
    virtual std::string getActivityName() override;
public:

    /** \brief Local variant for getOwnerTrace() */
    ITrace* getOwnerTraceLocal() { return _ownerTr; }
protected:
    SimpleCSVLogEvent(const SimpleCSVLogEvent&);                 // Prevent copy-construction
    SimpleCSVLogEvent& operator=(const SimpleCSVLogEvent&);      // Prevent assignment

protected:
    /** \brief Represents a relative offset from a begin of a stream for the event line */
    std::streamoff _evStreamOffset;

    /** \brief Ptr to a activity attribute-representing string from a activities string pool */
    const std::string* _attrActivity;

    /** \brief Stores simle representation (as a regular integral) timestamp attribute */
    int _timeSt;

    /** \brief Stores ptr to an owner trace... pity, but it is a need... */
    ITrace* _ownerTr;

}; // class SimpleCSVLogEvent 


class LDOPA_API CSVLog;

//==============================================================================


/** \brief Event creator based on event desciption given as a ...-separated string.
 *
 *  Creating procedure obtains a string descibing event, interprets it as a set of 
 *  decribing parameters separated with a given separate character. The procedure creates
 *  an event attributed with activity (as a shared string stored in a common pool that 
 *  are passed during initialization), converted timestamp and a number 
 */
class LDOPA_API SimpleCSVLogEventCreator : public IStrOffsEventCreator
{
public:
    /** \brief Initialize with a log.
     *
     *  Set itself for a given log as a event creator object.
     */
    SimpleCSVLogEventCreator(); // CSVLog* ownerLog);
public:
    //-----<IStrOffsEventCreator:: implementations>-----
    virtual IEvent* createEvent(const std::string str, std::streamoff ofs, //std::string& caseID) override;
        IEventLog::CaseID& caseID, strutils::VectorOfPairOfIndices* attrPairs) override;
    virtual void setOwnerLog(CSVLog* log) override;

protected:
    SimpleCSVLogEventCreator(const SimpleCSVLogEventCreator&);                 // Prevent copy-construction
    SimpleCSVLogEventCreator& operator=(const SimpleCSVLogEventCreator&);      // Prevent assignment

protected:
    
    /** \brief Stores a log-owner. */
    CSVLog* _ownerLog;

}; // class SimpleCSVLogEventCreator


}}}} // namespace xi::ldopa::eventlog::obsolete1



#endif // XI_LDOPA_EVENTLOG_CSVLOGEVENTS_H_
