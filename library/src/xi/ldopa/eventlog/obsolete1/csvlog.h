////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     CSV file event log Implementation
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      23.02.2016
/// \copyright (c) xidv.ru 2014–2016.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// [A more elaborate module description]
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_EVENTLOG_CSVLOG_H_
#define XI_LDOPA_EVENTLOG_CSVLOG_H_

//#pragma once

// std
#include <iostream>
#include <fstream>
#include <bitset>
#include <map>
//#include <set>
#include <string>
#include <vector>


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa.log
#include "xi/ldopa/eventlog/obsolete1/eventlog.h"

// xilib
#include "xi/strutils/substr.h"
#include "xi/strutils/set_str_pool.h"


namespace xi { namespace ldopa { namespace eventlog { namespace obsolete1 {;   //


class LDOPA_API CSVLogTrace;


#ifdef LDOPA_DLL
// PIMPL (https://support.microsoft.com/en-us/kb/168958)

LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::vector<xi::ldopa::eventlog::obsolete1::IEvent *>;
LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::map<IEventLog::CaseID, CSVLogTrace*>;
//LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::bitset<CSVLog::SET__LAST>;    // не получается так!
LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::bitset<1>;
LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::vector<std::string>;

#endif // LDOPA_DLL


//==============================================================================



class LDOPA_API CSVLog;

/** \brief Declares an event creator interface dealing with input str and a number
 *
 *
 */
class LDOPA_API IStrOffsEventCreator
{
protected:
    /** \brief Protected Destructor: It is not intended to delete an object through this interface! */
    ~IStrOffsEventCreator() {};

public:
    //-----<Main interface>-----

    /** \brief Creates an event as based on input string description and a number. Also extracts
     *  caseID and returns it through param \a caseID   
     *  \param str init string parameter
     *  \param ofs stream offset for a line corresponding to this event
     *  \param caseID — extracted case id
     *  \param attrPairs if \a attrPairs is not nullptr this vector is used to put into it
     *  pairs of attribute substrings' position indexes obtained during splitting the string
     *  \returns a new event object
     */
    virtual IEvent* createEvent(const std::string str, std::streamoff ofs, //std::string& caseID) = 0;
        IEventLog::CaseID& caseID, strutils::VectorOfPairOfIndices* attrPairs = nullptr) = 0;

    /** \brief Sets an owner log for event creator */
    virtual void setOwnerLog(CSVLog* log) = 0;
}; // class IStrOffsEventCreator


//==============================================================================

/** \brief Declares an interface for extracting and caching additional attributes
 *
 *  elaborate
 */
class LDOPA_API ICSVLogAttrCacher {
protected:
    /** \brief Protected Destructor: It is not intended to delete an object through this interface! */
    ~ICSVLogAttrCacher() {};
public:
    //-----<Main interface>-----
    
    /** \brief Event log calls this methos while parse event lines given from the CSV file
     *  \param ev an event object created in the main marking procedure
     *  \param tr a trace where the event is put in
     *  \param caseId an ID for a case the event is assigned to (normally the same case id
     *  is also assigned to the trace \a tr)
     *  \param line text line with event description
     *  \param attrPairs helper vector of pairs of substrings' positions for event attributes
     *  \param offs an offset in the file the text line is started with
     */
    virtual void parseEventLine(IEvent* ev, ITrace* tr, IEventLog::CaseID caseId,
        std::string& line, strutils::VectorOfPairOfIndices& attrPairs, std::streamoff offs) = 0;

    /** \brief Method should be invoked when events are not more valid (e.g. log is reopen) */
    virtual void invalidateAllEvents() = 0;

    /** \brief Invoked where given event \a ev is no more valid (deleted) */
    virtual void invalidateEvent(IEvent* ev) = 0;
}; // class  ICSVLogAttrCacher

//==============================================================================

class LDOPA_API CSVLogTraces;

/** \brief Trace implementation for CSV-based log 
 *
    The class manages lifetime of events added to the trace the object is for.
 */
//class CSVLogCaseDescriptor : public ICaseDescriptor
class LDOPA_API CSVLogTrace : public ITrace
{
public:
    //-----<Types>-----
    /** \brief Declare a type for vector of events */
    typedef std::vector<IEvent*> VectorOfEvents;
    typedef VectorOfEvents::const_iterator VectorOfEventsConstIter;

    typedef collections::Enumerator4Iterator_obsolete1<IEvent, VectorOfEventsConstIter> MyEnumerator;
    typedef collections::ConstEnumerator4Iterator_obsolete1<IEvent, VectorOfEventsConstIter> MyConstEnumerator;
public:
    /** \brief Default constructor */
    CSVLogTrace(CSVLogTraces* ownerTracer, IEventLog::CaseID caseId);

    /** \brief Destructor */
    ~CSVLogTrace();
public:
    //-----<ITrace:: overrides>-----

    virtual size_t getEventsNum() override;
    virtual IEvent* insertEvent(IEvent* ev) override;
    
    // the question is if it would be rational to get this method declared as inline and having 
    // such a checking?
    virtual IEvent* operator [](size_t i) override
    {
        if (!_prepared)
            prepareInternal();
        return _events[i];
    }

    virtual IEventEnumerator* enumerateEvents() override;
    
    /** \brief Since the method is constant, it can't prepare event sequence if it still 
     *  has not been prepared. In this case an exception is thrown
     */
    virtual IEventConstEnumerator* enumerateEventsConst() const override;
    virtual ITraces* getOwnerTraces() override;

public:
    /** \brief Internal (non-virtual) method that \returns a number of events in the case */
    size_t getEventsNumLocal() const { return _events.size(); }

    /** \brief Local version of event inserting — w/o any excess polymorphism */
    IEvent* insertEventLocal(IEvent* ev);

    /** \brief Return true if the trace is prepared; false otherwise */
    bool isPrepared() const { return _prepared;  }

    /** \brief Prepare trace in the sense as it discussed in docs for \a _prepared member */
    void prepare() { if (!_prepared) prepareInternal();  }

public:
    
    /** \brief Compares to given event to determine their order in a trace. 
     *
     *  Implements compare concept (http://en.cppreference.com/w/cpp/concept/Compare)
     */
    inline static bool compareEvents(IEvent* lhv, IEvent* rhv);

protected:
    CSVLogTrace(const CSVLogTrace&);                 // Prevent copy-construction
    CSVLogTrace& operator=(const CSVLogTrace&);      // Prevent assignment
protected:

    /** \brief Deletes all events added to the trace and frees their memory */
    void deleteEvents();       

    /** \brief Internal prepare method */
    void prepareInternal();

protected:
    /** \brief Stores a Case ID for the trace */
    IEventLog::CaseID _caseID;

    /** \brief Stores events of the trace as a vector */
    VectorOfEvents _events;

    /** \brief Flag determines if a trace is prepared with respect to events containing inside. 
     *
     *  We use a vector data structure for storing events internally. It allows to add new events
     *  rather quickly. As events can be inserted randomly, the order of them being pushed to the end 
     *  of the vector is not preserved. Toghether with that, appending of event is being done 
     *  normally once. So, it makes sense to have all events unsorted during appending process and
     *  sort them once at the very end. The flag determines if the vector has (true) or has not
     *  been sorted.
     *  Assume, the sorting is done automatically before very first referenece to an event from the trace.
     */
    bool _prepared;

    /** \brief Stores an owner collection of traces */
    CSVLogTraces* _ownerTracer;
}; // class CSVLogTrace

//==============================================================================


/** \brief Implementation of ITraces for CSV-based log
 *
 *  Supports...
 *  Stores all extracted activities as a set (collection) of Activity IDs. 
 *  A mapping of act ids to files lines number is done with using a single multimap object
 *  in order to exclude extra data structures (e.g. one collection of events per activity)
 */
class LDOPA_API CSVLogTraces : public ITraces
{
    friend class CSVLog;
public:
    //-----<types>-----
    
    /** \brief Typedef for representing ActivityID attribute */
    typedef std::string ActivityID;
    
    /** \brief Type for (multi) mapping activity id to a number (e.g. line number) */
    typedef std::multimap<ActivityID, size_t> ActID2NumMMap;

    /** \brief Type for (multi) mapping case id to a number (e.g. line number) */
    typedef std::multimap<IEventLog::CaseID, size_t> CaseID2NumMMap;

    /** \brief Type for map case ids to case descriptors */
    typedef std::map<IEventLog::CaseID, CSVLogTrace*> Case2TraceMap;

    /** \brief Value type for iterating Case2TraceMap map (pair<...>) */
    typedef Case2TraceMap::value_type Case2TraceMapVal;

    /** \brief Iterator type moniker for Case2TraceMap */
    typedef Case2TraceMap::iterator Case2TraceMapIter;

    /** \brief Const Iterator type moniker for Case2TraceMap */
    typedef Case2TraceMap::const_iterator Case2TraceMapConstIter;

    // types monikers for auto-enumerator template concretization
    
    /** \brief Type monikers for auto-enumerator template concretization */
    typedef collections::Enumerator4Iterator_obsolete1<ITrace, Case2TraceMapConstIter> MyEnumerator;
        
    /** \brief Type monikers for const auto-enumerator template concretization */
    typedef collections::ConstEnumerator4Iterator_obsolete1<ITrace, Case2TraceMapConstIter> MyConstEnumerator;    
public:
    // some special consts

    /** \brief A number for attribute that is not set */
    static const short ATTR_NSET = -1;
public:
    // constructors
    CSVLogTraces(CSVLog* owner);

    /** \brief Destructor */
    ~CSVLogTraces();
public:
    //-----<ITraces:: implementations>-----
    
    virtual size_t getNumOfTraces() override;
    virtual ITrace* insertEvent(IEventLog::CaseID caseId, IEvent* ev) override;
    virtual ITrace* operator [](IEventLog::CaseID caseId) override
    {
        return getTrace(caseId);
    }
    virtual ITraceEnumerator* enumerateTraces() override;
    virtual ITraceConstEnumerator* enumerateTracesConst() const override;
    virtual IEventLog* getOwnerLog() override;
public:
    /** \brief Maps mainPM attributes to a columns number. Default value -1 determines that attribute is not set */
    void mapMainAttributes(short nCase = ATTR_NSET, short nAct = ATTR_NSET, short nTmst = ATTR_NSET);

    /** \brief \returns true, is a mapping for necessary attributes has been made successfully; 
     *  false otherwise
     */
    bool isAttrsMappingOk() const { return _attrNumCase != ATTR_NSET; }

    /** \brief Returns a number of case attribute in the event-representing string */
    short getAttrNumCase() const { return _attrNumCase; }

    /** \brief Returns a number of action attribute in the event-representing string */
    short getAttrNumAct() const { return _attrNumAct; }

    /** \brief Returns a number of timestamp attribute in the event-representing string */
    short getAttrNumTmst() const { return _attrNumTmst; }

    /** \brief Local version of event inserting — w/o any excess polymorphism */
    ITrace* insertEventLocal(IEventLog::CaseID caseId, IEvent* ev);

    /** \brief Local version of a method for getting a number of traces */
    size_t getNumOfTracesLocal() const { return _case2TraceMaps.size(); } // _numOfTraces; }

    /** \brief Local version of operator[] */
    ITrace* getTrace(IEventLog::CaseID caseId);
protected:
    CSVLogTraces(const CSVLogTraces&);                 // Prevent copy-construction
    CSVLogTraces& operator=(const CSVLogTraces&);      // Prevent assignment               
protected:
    /** \brief Internal method for deleting traces object (e.g., before new mark up) */
    void clearTraces();
protected:
    /** \brief Owner log. */
    CSVLog* _owner;

    /** \brief Case attribute number */
    short _attrNumCase;

    /** \brief Activity attribute number */
    short _attrNumAct;

    /** \brief Timestamp attribute number */
    short _attrNumTmst;

    /** \brief CaseIDs to case descriptors map */
    Case2TraceMap _case2TraceMaps;
}; // class CSVLogTraces

//==============================================================================

/** \brief Event Log based on CSV format
 *
 *  -
 */
class LDOPA_API CSVLog : public IEventLog
{
public:
    friend class CSVLogTraces;
public:
    //-----<Types>-----
    
    /** \brief Enumerates setting bit number constants */
    enum 
    {
        SET_FIRST_LINE_IS_HEADER = 0,           ///< Treat first line as a attribute headers
        SET__LAST                               ///< Technical internal value used to determine size for underlying bitset
    };


    /** \brief A typename for bitset type for storing settings */
    typedef std::bitset<SET__LAST> SettingsBitset;

    /** \brief Moniker for char type */
    typedef std::string::value_type StdStrChar;

public:
    //-----<Const>-----
    
    // default settings
    static const unsigned int DEF_SETTINGS =    
        (0x1 << SET_FIRST_LINE_IS_HEADER);      // treat first line as a header
        // |

    /** \brief Default deparator character */
    static const StdStrChar DEF_SEP_CHAR;

public:
    /** \brief Default constructor */
    CSVLog(IStrOffsEventCreator* evcr = nullptr);// IStrOffsEventCreator* evcr);


    /** \brief Initialize with a name of CSV file */
    CSVLog(const std::string& fileName, IStrOffsEventCreator* evcr); // , IStrOffsEventCreator* evcr);

    /** \brief Initialize with a given stream object */
    CSVLog(std::istream* istr, IStrOffsEventCreator* evcr); // , IStrOffsEventCreator* evcr);

    /** \brief Destructor */
    ~CSVLog();                      
public:
    //-----<IEventLog:: interface implementation>-----
    virtual bool open() override;
    virtual void close() override;
    virtual bool isOpen() override;
    virtual ITraces* getTraces() override;
    virtual xi::strutils::SetStrPool* getActivitiesPool() override;
public:
    //-----<Specefifc for working with CSV-log>-----
    
    /** \brief Sets a filename for a log file. If a log is already open, an exception is thrown */
    void setFileName(const std::string& fn);

    /** \brief Returns a filename for a log */
    const std::string getFileName() const { return _fileName;  }

    /** \brief Gets an event creator object for this log */
    IStrOffsEventCreator* getEventCreator() const { return _evCreator; }
    
    /** \brief Sets an event creator object */
    void setEventCreator(IStrOffsEventCreator* evcr); // { _evCreator = val; }

    /** \brief Local version of getTraces() method returning a typed object representing traces */
    CSVLogTraces* getTracesLocal() const { return _traces; }

    /** \brief Reads a line from CSV file starting from \a ofs offset to a string \a str 
     *  \return true if read succesfully; false otherwise
     */
    bool readCsvLine(std::streamoff ofs, std::string& str);

public:
    //-----<Helpers for work with settings>-----
    
    /** \brief Returns true is the first line of the log should be considered as a header */
    bool isFirstLineAsHeader() const { return _settings[SET_FIRST_LINE_IS_HEADER]; }

    /** \brief Returns a character used as a separator */
    StdStrChar getSepChar() const { return _sepChar;  }

    /** \brief Returns a ref to a underlying str pool of activities */
    xi::strutils::SetStrPool& getActivitiesPoolLocal() { return _activitiesPool; }


    /** \brief Returns a const-ref to a underlying str pool of activities */
    const xi::strutils::SetStrPool& getActivitiesPoolLocal() const { return _activitiesPool; }

    /** \brief Returns true if the log has been marked up successfully; false otherwise */
    bool isReady() const { return _ready; }

    /** \brief Returns a vector of attribute headers */
    const strutils::VectorOfStrings& getAttrHeaders() const { return _attrHeaders;  }

    ICSVLogAttrCacher* getEventAttrCacher() const { return _evAttrCacher; }
    void setEventAttrCacher(ICSVLogAttrCacher* val) { _evAttrCacher = val; }

    /** \brief For a given line of a CSV file, \a line, extracts attribute with number
     *  \a caseAttrNumb and puts to \a attrStr. 
     *  \returns true if attribute extracted succesfully; false otherwise
     *
     *  The method is no static as it uses a separator char. Nevertheless, it makes sense, once,
     *  to split it to a static version with passing a separator as a parameter.
     */
    bool extractAttribute(const std::string& line, short caseAttrNumb, std::string& attrStr);

protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    CSVLog(const CSVLog&);                 // Prevent copy-construction
    CSVLog& operator=(const CSVLog&);      // Prevent assignment
protected:

    /** \brief Tries to create an istream based on file with a filename that is set in advance */
    void createIstrFile();
    
    /** \brief Tries to close an ifstream file if exists */
    void closeIstrFile();

    /** \brief Internal rapid variant of a method for determining openness */
    bool isOpenInt() const { return (_istr != nullptr); }

    /** \brief Performs initialization setting flags / TODO: delete?!*/
    void initSettings();


    /** \brief Creates associated objects */
    void createAssociatedObjects();

    /** \brief Destroys associated objects */
    void destroyAssociatedObjects();

    /** \brief Checks if a log is open; if not, an exception is thrown */
    void checkForOpenWithExc();

    /** \brief Checks, if a istream is ready to read from and case attr mapping is done, then read cases */
    void markUpLog();

    /** \brief Iterates all the events and map every event to a corresponding activity cluster */
    void markUpLogInt();

    /** \brief Deletes all log markup */
    void demarkLogInt();

    /** \brief Extracts attributes' headers from the first line of a file */
    void extractAttrHeaders(const std::string& str);
protected:
    /** \brief Input stream for CSV-text input */
    std::istream* _istr;

    /** \brief A file-base input stream for the case of local creating */
    std::ifstream* _ifstr;

#pragma warning(push)
#pragma warning(disable: 4251)

    /** \brief File name for maintaining an internal file stream */
    std::string _fileName;

#pragma warning(pop)

    /** \brief Settings (flags) object */
    SettingsBitset _settings;

    /** \brief Deterines a separator character */
    StdStrChar _sepChar;

    /** \brief Object represented a collection of traces */
    CSVLogTraces* _traces;

    /** \brief Stores a pool of string ids of activities */
    xi::strutils::SetStrPool _activitiesPool;

    /** \brief Stores flag determining that the log has been marked up successfully */
    bool _ready;

    /** \brief Event creator */
    IStrOffsEventCreator* _evCreator;

    /** \brief Event attributes extractor and cachier delegate object */
    ICSVLogAttrCacher* _evAttrCacher;


    /** \brief Stores headers of acctibutes extracted from the first line of a file */
    strutils::VectorOfStrings _attrHeaders;
}; // class CSVLog

}}}} // namespace xi::ldopa::eventlog::obsolete1


/** \brief Traits for CSVLogTrace */
template <>
struct LDOPA_API xi::collections::Enumerator4Iterator_traits_obsolete1 
    < xi::ldopa::eventlog::obsolete1::CSVLogTrace::VectorOfEventsConstIter >
{
    ldopa::eventlog::obsolete1::CSVLogTrace::VectorOfEventsConstIter::value_type
        getValue(const ldopa::eventlog::obsolete1::CSVLogTrace::VectorOfEventsConstIter & cur)
    {
        return *cur;
    }
}; // template <> struct StorageIterator_traits<...>


/** \brief Traits for MapNamedVerticesStorage */
template <>
struct LDOPA_API xi::collections::Enumerator4Iterator_traits_obsolete1 
    < xi::ldopa::eventlog::obsolete1::CSVLogTraces::Case2TraceMapConstIter >
{
    
    ldopa::eventlog::obsolete1::CSVLogTrace*
        getValue(const ldopa::eventlog::obsolete1::CSVLogTraces::Case2TraceMapConstIter & cur)
    {
        return cur->second;
    }
}; // template <> struct StorageIterator_traits<...>


#endif // XI_LDOPA_EVENTLOG_CSVLOG_H_
