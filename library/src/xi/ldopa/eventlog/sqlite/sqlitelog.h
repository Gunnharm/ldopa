////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     SQLite-based event log Implementation
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      23.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// [A more elaborate module description]
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_EVENTLOG_SQLITE_SQLITELOG_H_
#define XI_LDOPA_EVENTLOG_SQLITE_SQLITELOG_H_

#pragma once

// std
#include <bitset>
#include <string>
#include <vector>
#include <map>


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa.log
#include "xi/ldopa/eventlog/eventlog.h"
#include "xi/ldopa/eventlog/sqlite/sqlitehelpers.h"


namespace xi { namespace ldopa { namespace eventlog {;   //


// экспорт конкретизированных шаблонов в DLL
// это должно быть ДО использования в нижележащих классах
// PIMPL (https://support.microsoft.com/en-us/kb/168958)
#ifdef LDOPA_DLL

LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::bitset<4>;

#endif // LDOPA_DLL



// forwards
class LDOPA_API SQLiteLog;


//==============================================================================
// class IEventLog2_traits
//==============================================================================

/** \brief Auxilliary class for SQLIte event log classes. */
class LDOPA_API SQLiteLog_traits {
public:
    // typedefs

    /** \brief Alias for Attribute. */
    typedef IEventLog_traits::NamedAttribute NamedAttribute;

    /** \brief Declares a map of attributes (id — attribute). */
    typedef std::map < std::string, IEventLog::Attribute > AttributesMap;

    /** \brief Declares a vector of unnamed attributes. */
    typedef std::vector <IEventLog::Attribute> AttributesVector;

    /** \brief Collection datatype for storing attributes in a vector. */
    typedef std::vector<NamedAttribute> NmAttributesVector;

    /** \brief Alias for a constant enumerator over an iterator of attributes vector. */
    typedef xi::collections::
        Enumerator4ConstIterator< NmAttributesVector::const_iterator > NmAttrsVecCEnum;
}; // class SQLiteLog_traits




//==============================================================================
// class SQLiteTrace
//==============================================================================

/** \brief Implementation of a trace object for SQLite Event Log.
 *
 *  -
 */
class LDOPA_API SQLiteTrace : public IEventTrace {
    friend class SQLiteLog;
public:
    /** \brief Vector of traces. */
    typedef std::vector<IEvent*> VectorOfEvents;
    
    /** \brief Collection datatype for storing attributes. */
    typedef SQLiteLog_traits::NmAttributesVector AttributesVec;

    /** \brief Alias for Attribute. */
    typedef SQLiteLog_traits::NamedAttribute NamedAttribute;    

    /** \brief Alias for a constant enumerator over an iterator of attributes vector. */
    typedef SQLiteLog_traits::NmAttrsVecCEnum AttrsVecCEnum;
public:
    //-----<String Const>-----    

    /** \brief Name of parameter for the query for getting all events of a trace. */
    static const char* PAR_LQRY_GETTRACE_EVENTS;

protected:
    
    /** \brief Constructor initializes with onwer log and a number of 
     *  a corresponding trace.
     */
    SQLiteTrace(SQLiteLog* owner, UInt traceNum);
protected:
    // в принципе, здесь можно и КК определить
    SQLiteTrace(const SQLiteTrace&);                    // Prevent copy-construction
    SQLiteTrace& operator=(const SQLiteTrace&);         // Prevent assignment

public:

    /** \brief Destructor. */
    ~SQLiteTrace();
public:
    //-----<IEventTrace:: interface implementation>-----
    
    virtual int getAttrsNum() override;
    virtual bool getAttr(const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getAttrs() override;

    virtual IEvent* getEvent(UInt eventNum) override;
    virtual int getSize() override;
    virtual IEventLog* getLog() override;
protected:
    
    /** \brief Extracts the trace size from a log. */
    void requestSize();

    /** \brief Requests a number of all events and allocate storage for them. */
    void obtainEventsNum();

    /** \brief Allocates a storage for traces according to the number of traces extracted
     *  prior to invocation of this method.
     */
    void allocEventsStorage();

    /** \brief Deletes all traces objects extracted and stored in the traces storage. 
     * 
     *  Paramter \a needResize determines whether a collection of events should be resized 
     *  (set to 0) after freeing or not.
     * 
     */
    void freeEvents(bool needResize);

    /** \brief Fetches up to eventNum (inclusively) events for the current trace. */
    void fetchUpTo(int eventNum);

    /** \brief Creates a SQLite3 statement for extracting events for this trace. */
    void createExtractor();

    /** \brief Extracts attributes of a trace. */
    void extractAttributes();


public:
    //-----<Queries for obtaining trace data>-----

    /** \brief Query for getting all events contatining in a trace. 
     *
     *  Query has a single parameter determining the 0-based number of a trace.
     */
    //std::string& getQryGetTraceEvents() { return _owner->getConfigParamOrDef(PAR_LQRY_GETTRACE_EVENTS); }
protected:

    SQLiteLog* _owner;      ///< Stores a ptr to an owner log.
    int _traceNum;          ///< Stores the number of the corr. trace.
    
    /** \brief Stores the number of events for the trace.
     *
     *  Value -1 means that the row number has not been still extracted.
     *  Extracting of a number of event is done by executing a separate SQL
     *  query, which must be reconciled with a query that extracts trace's 
     *  event themselves.
     */
    int _size;             

    /** \brief Stores a pointers to extracted traces. */
    VectorOfEvents _events;

    /** \brief Statement for extracting event for the query. */
    SQLiteStmt* _evExtractStmt;

    /** \brief Stores a collection of log attributes in the form of a vector. */
    AttributesVec _attributes;

    /** \brief Flag determines if attributes of a log were extracted.  */
    bool _attrsExtracted;
}; // class SQLiteTrace


//==============================================================================
// class SQLiteEvent
//==============================================================================

/** \brief Implementation of an event object for SQLite Event Log.
*
*  -
*/
class LDOPA_API SQLiteEvent : public IEvent {
    friend class SQLiteTrace;
public:
    /** \brief Alias for Attribute. */
    typedef SQLiteLog_traits::NamedAttribute NamedAttribute;
    
    /** \brief Collection datatype for storing attributes. */
    typedef SQLiteLog_traits::NmAttributesVector AttributesVec;

    /** \brief Alias for a constant enumerator over an iterator of attributes vector. */
    typedef SQLiteLog_traits::NmAttrsVecCEnum AttrsVecCEnum;
    
protected:
    /** \brief Constructor initializes an event object with a owner trace and initializing
     *  SQLite3 statement.
     */
    SQLiteEvent(SQLiteTrace* owner, SQLiteStmt* initStmt);
protected:
    // в принципе, здесь можно и КК определить
    SQLiteEvent(const SQLiteEvent&);                    // Prevent copy-construction
    SQLiteEvent& operator=(const SQLiteEvent&);         // Prevent assignment

public:
    //-----<IEvent2:: interface implementation>-----
    virtual bool getAttr(const char* id, Attribute& a) override;
    virtual int getAttrsNum() override;
    //virtual AttributesMap getAttrsAsMap() override;
    virtual IAttributesEnumerator* getAttrs() override;
    virtual IEventTrace* getTrace() override;
protected:

    /** \brief Extracts attributes from a prepared (and preliminarily fetched) statement. */
    void extractAttributes(SQLiteStmt* stmt);

protected:
    SQLiteTrace* _owner;            ///< Owner trace.    
    AttributesVec _attributes;      ///< Collection of attributes in the form of a vector.
}; // class SQLiteEvent


//==============================================================================
// class SQLiteLog
//==============================================================================

/** \brief SQLite based event log. 
 *
 *  #TODO: расписать про отличия "вертикального" извлечение атрибутов от "горизонтального".
 */
class LDOPA_API SQLiteLog : public IEventLog {
    friend class SQLiteTrace;
public:
    //-----<Types>-----

    /** \brief Enumerates setting bit number constants. */
    enum
    {
        SET_AUTO_LOAD_SETS          = 0,    ///< Force settings loading after opening connection.
        SET_ALLOW_QRY_CONF_OVERWR   = 1,    ///< Allow overwriting main (log-based) query from DB config.        
        SET_VERT_EVENT_ATTR_EXTR    = 2,    ///< Determines if extraction of event attributes is "vertical".
        SET_VERT_TRACE_ATTR_EXTR    = 3,    ///< Determines if extraction of traces attributes is "vertical".
        SET_LOOKUP_TRACE_IDS        = 4,    ///< Determines whether a trace-id-lookup mode is enabled (1).
        //
        //
        SET__LAST                           ///< Technical internal value used to determine size for underlying bitset
    };

    /** \brief A typename for bitset type for storing settings */
    typedef std::bitset<SET__LAST> SettingsBitset;

    /** \brief Type (shortcut) for SQLite callback function. */
    typedef SQLiteDB::SqlVoidCallback SqlVoidCallback;
    //typedef int(*SqlCallback)(void*, int, char**, char**);

    /** \brief Alias for a map of attributes (id — attribute). */
    typedef SQLiteLog_traits::AttributesMap AttributesMap;

    /** \brief Alias for a vector of NAMED attributes. */
    typedef SQLiteLog_traits::NmAttributesVector NmAttributesVectors;

    /** \brief Alias for a vector of UNNAMED attributes. */
    typedef SQLiteLog_traits::AttributesVector AttributesVector;

    /** \brief Maps a name of an SQL-query to the query itself. */
    typedef std::map<std::string, std::string> SqlQueriesMap;

    /** \brief Vector of traces. */
    typedef std::vector<SQLiteTrace*> VectorOfTraces;

    /** \brief Alias for a constant enumerator over an iterator of attributes vector. */
    typedef SQLiteLog_traits::NmAttrsVecCEnum NmAttrsVecCEnum;
public:
    //-----<Const>-----

    // default settings
    static const unsigned int DEF_SETTINGS =
        (   0x1 << SET_AUTO_LOAD_SETS           // Load settings after opening a log db.
          | 0x1 << SET_ALLOW_QRY_CONF_OVERWR    // Config overwriting.
          | 0x0 << SET_VERT_EVENT_ATTR_EXTR     // horizontal event attr extracting
          | 0x0 << SET_VERT_TRACE_ATTR_EXTR     // horizontal trace attr extracting
          | 0x0 << SET_LOOKUP_TRACE_IDS         // trace-id-lookup mode disabled
        );      

    #pragma region Strings
    //-----<String Const>-----    
    
    static const char* PAR_LQRY_EVENTS_NUM;         ///< Name of parameter for the query for getting number of events of a log.
    static const char* PAR_LQRY_TRACES_NUM;         ///< Name of parameter for the query for getting number of traces.
    static const char* PAR_LQRY_ACTIVITIES_NUM;     ///< Name of parameter for the query for getting number of activities.    
    static const char* PAR_LQRY_TRACES;             ///< Name of parameter for the query for getting traces.
    static const char* PAR_LQRY_GETLOGATTR;         ///< Name of parameter for the query for getting a log attribute.
    static const char* PAR_LQRY_GETALLLOGATTRS;     ///< Name of parameter for the query for getting all log attributes.
    static const char* PAR_LQRY_GETEVENTATTR;       ///< Name of parameter for the query for getting an event attribute by tracenum and relnum.
    static const char* PAR_LQRY_GETEVENTATTR_BYID;  ///< Name of parameter for the query for getting an event attribute by its uniq id.
    static const char* PAR_LQRY_GETALLEVENTATTRS;   ///< Name of parameter for the query for getting all atributes of an event by a trace num and event offset.
    static const char* PAR_LQRY_GETALLEVENTATTRS_BYID;  ///< Name of parameter for the query for getting all atributes of an event by its uniq id.
    static const char* PAR_LQRY_GETTRACEATTR;       ///< Name of parameter for the query for getting trace attributes by its uniq id.
    static const char* PAR_LQRY_GETALLTRACEATTRS;   ///< Name of parameter for the query for getting all attributes of a trace.
    static const char* PAR_LQRY_GETTRACE_EVENTSNUM; ///< Name of parameter for the query for getting all attributes of a trace.

    static const char* PAR_EV_ACT_ATTRID;           ///< Name of a parameter that determines an ID of Activity attribute of event attributes.
    static const char* PAR_EV_TIMEST_ATTRID;        ///< Name of a parameter that determines an ID of TimeStamp attribute of event attributes.
    static const char* PAR_EV_CASE_ATTRID;          ///< Name of a parameter that determines an ID of Case attribute of event attributes.


    static const char* COMP_LABEL;                  ///< Represents a label for this class-component.


    /** \brief Name of the boolean autoconfig parameter that determines whether it should 
     *  request trace attributes in "vertical" mode or not.
     */
    static const char* PAR_IS_REQTRACEATTRVERT;

    /** \brief Name of the boolean autoconfig parameter that determines whether it should
     *  request event attributes in "vertical" mode or not.
     */
    static const char* PAR_IS_REQEVENTATTRVERT;
    
    /** \brief Determines whether a preeptive loading of all trace/case attributes should
     *  be performed.
     *  
     *  Setting this parameter switches the log subsystem into "trace-id-lookup" mode. This
     *  means that when addressing an individual trace by its 0-based number, a lookup 
     *  procedure is performed. This way, this number is translated to an attribute, which
     *  is further used for inquering in all queries with the clause "WHERE trace = ?1".
     */
    static const char* PAR_LOOKUP_TRACE_IDS;
    
    #pragma endregion // Strings

public:

    /** \brief Default constructor. */
    SQLiteLog();

    /** \brief Constructor initializes with a log file (name). */
    SQLiteLog(const std::string& fn);

    /** \brief Destructor. */
    virtual ~SQLiteLog();
protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    SQLiteLog(const SQLiteLog&);                    // Prevent copy-construction
    SQLiteLog& operator=(const SQLiteLog&);         // Prevent assignment

public:
    //-----<Various static helpers>-----
    
    /** \brief Extracts an attribute "vertically" with its id from the executed 
     *  SQLite statement \a stmt and places it into \a a and its 
     *  id into \a aId if succeed.
     *
     *  Does not fetch the dataset!
     *  "Vertically" means, that each row of a givev dataset is interpreted as
     *  a single attribute.
     *  \param[out] a — ...
     *  \param[out] aId — ...
     *  \returns true, if a valid attribute has been extracted; false otherwise.
     *  \a idNum, \a valNum, \a typeNum determine corresplondly
     *  position of the attribute id (name), attribute value and attribute type.
     */
    static bool extractAttributeVert(SQLiteStmt* stmt, std::string& aId, Attribute& a,
        int idNum, int valNum, int typeNum);

    /** \brief Extract an attribute from the given statement "vertically" and checks
     *  whether its id is the as requested.
     *
     *  Parameters are almost the same as for SQLiteLog::extractAttributeVert().
     *  \a reqId determines a requsted id of an attribute.
     *  Method itself fetches the dataset from the given query.
     */
    static bool fetchAttributeVert(SQLiteStmt* stmt, const char* reqId,
        Attribute& a, int idNum, int valNum, int typeNum);


    /** \brief Extracts an attribute number \a iCol "horizontally" with from the 
     *  executed  SQLite statement \a stmt and places it into \a a and 
     *  its id into \a aId (if given) if succeed.
     *
     *  Does not fetch the dataset!
     *  "Horizontally" means that every column of a (first row of a) resulting dataset
     *  is interpreted as a single attribute.
     *  Routine looks for a columnt with the name that is equal to \param aId and
     *  returns a value of a first row.
     *  \param[out] a — ...
     *  \param[out] aId — ...
     *  \returns true, if a valid attribute has been extracted; false otherwise.
     */
    static bool extractAttributeHor(SQLiteStmt* stmt, int iCol, Attribute& a, 
        std::string* aId = nullptr);

    /** \brief Fetches the given statment \a stmt and looks among the result dataset
     *  for an event attribute with id \param aId.
     *
     *  If the requested attribute is found, places if into \param[out] a and
     *  returns true. Otherwise returns false.
     */
    bool fetchAttributeHor(SQLiteStmt* stmt, const char* aId, Attribute& a);

    /** \brief Binds a value of the given attribute \a a as a parameter of
     *  the statements \a stmt with number \a num
     *
     *  -
     */
    static void bindAttribute(SQLiteStmt* stmt, const Attribute& a, int num);

    /** \brief Escapes the given string replacing ' to '' as it required by SQLite. 
     * 
     *  if the parameter \a lenHint is equal to -1, string length is calculated, otherwise
     *  \a lenHint is used as the string's length.
     */
    static std::string sqlizeString(const char* s, int lenHint = -1);

    /** \brief Overloaded version for std::string. */
    static std::string sqlizeString(const std::string& s)
    {
        return sqlizeString(s.c_str(), s.length());
    }

public:
    //-----<Utilities>-----

    /** \brief Returns true if the two given c-strings are equal. */
    static bool isStrsEq(const char* s1, const char* s2)
    {
        return (strcmp(s1, s2) == 0);
    }

    /** \brief Return true of a given str ptr is null or string-object is empty. */
    static bool isStrNullOrEmpty(const std::string* str)
    {
        return (str == nullptr || str->empty());
    }

    /** \brief Copies all attributes from the given enumerator \a aen to the 
     *  given map \a amap.
     */ 
    static void copyAttributes2Map(IAttributesEnumerator* aen, AttributesMap& amap);

    /** \brief Copies all attributes from the given enumerator \a aen to the 
     *  given vector \a avec.
     */ 
    static void copyAttributes2Vec(IAttributesEnumerator* aen, NmAttributesVectors& avec);

public:
    //-----<IEventLog2:: interface implementation>-----

    virtual void open() override;
    virtual void close() override;
    virtual bool isOpen() override;
    virtual int getEventsNum() override;
    virtual int getTracesNum() override;
    virtual int getActivitiesNum() override;
    virtual int getLogAttrsNum() override;
    virtual bool getLogAttr(const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getLogAttrs() override;
    virtual bool getEventAttr(int traceNum, int eventNum, 
        const char* id, Attribute& a) override;
    virtual bool getEventAttr(int eventId, const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getEventAttrs(int traceNum, int eventNum) override;
    virtual IAttributesEnumerator* getEventAttrs(int eventId) override;
    virtual bool getTraceAttr(int traceNum, const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getTraceAttrs(int traceNum) override;
    virtual int getTraceSize(int traceNum) override;
    virtual std::string getEvActAttrId() const override;
    virtual std::string getEvTimestAttrId() const override;
    virtual std::string getEvCaseAttrId() const override;
    virtual std::string getInfoStr() const override;


    // новый (от 16.02.2018) ООП-интерфейс
    virtual IEventTrace* getTrace(int traceNum) override;


public:
    // extensions of IEventLog2 interface 
    // (возможно, это тоже надо будет вынести в интерфейс с промежуточным абстрактным классом)
    
    /** \brief Obtains attributes associated with the log as a map. 
     *
     *  Since std::map has a move-constructor, suppose it is used here.
     *  If log is not active, returns empty map.
     */
    AttributesMap getLogAttrsAsMap();

    /** \brief Obtains attributes associated with the event (given by 
     *  a trace num and an offset) as a map.
     *
     *  Since std::map has a move-constructor, suppose it is used here.
     *  If log is not active, returns empty map.
     */
    AttributesMap getEventAttrsAsMap(int traceNum, int eventNum);

    /** \brief Obtains attributes associated with a trace (given by
     *  the unique id of an event) as a map.
     *
     *  Since std::map has a move-constructor, suppose it is used here.
     *  If log is not active, returns empty map.
     */
    AttributesMap getEventAttrsAsMap(int eventId);

    /** \brief Obtains attributes associated with a trace (given by
     *  the unique id of an event) as a map.
     *
     *  Since std::map has a move-constructor, suppose it is used here.
     *  If log is not active, returns empty map.
     */
    AttributesMap getTraceAttrsAsMap(int traceNum);

    /** \brief Returns a const ref to the vector with extracted trace IDs for trace-id-lookup mode. */
    const AttributesVector& getTraceIDs() const { return _traceIDs; }


public:
    //-----<Specefifc for working with SQLite log>-----

    /** \brief Sets a filename for a log file. If a log is already open, an exception is thrown. */
    void setFileName(const std::string& fn) { _db.setFileName(fn); }

    /** \brief Returns a filename for a log */
    const std::string getFileName() const { return _db.getFileName(); }
    //const std::string getFileName() const { return _fileName; }
    

    /** \brief Loads a part of config from a log by using given query \a confq.
     *
     *  While loading, only parameters, which are defined in a new config, are overwritten.
     *  Other parameters a preserved.
     *  That way, config overloading can be used for merging some "default" paramters with
     *  custom ones.
     *
     *  For proper config overloading, AutoLoadConfig parameter should be set.
     *
     *  If the log is not open, throws an exception.
     */
    void overloadConfigFromLog(const std::string& confq);

public:
    //-----<Helpers for work with settings>-----

    /** \brief Returns true is the first line of the log should be considered as a header. */
    bool isAutoLoadConfig() const { return _settings[SET_AUTO_LOAD_SETS]; }
    
    /** \brief Sets if a config should be loaded from a db immediately after opening db. 
     *
     *  If _qryLoadConfig is not defined, configuration will not be loaded with no warning.
     */
    void setAutoLoadConfig(bool v) { _settings[SET_AUTO_LOAD_SETS] = v; }

    /** \brief Returns true if extracting of event attributes is vertical, otherwise horizontal. */
    bool isVerticalEventAttrExtracting() const { return _settings[SET_VERT_EVENT_ATTR_EXTR]; }

    /** \brief Determines whether event attributes extracting should be horizontal 
     *  (false) or vertical (true). 
     */
    void setVerticalEventAttrExtracting(bool v) { _settings[SET_VERT_EVENT_ATTR_EXTR] = v; }


    /** \brief Returns true if extracting of trace attributes is vertical, otherwise horizontal. */
    bool isVerticalTraceAttrExtracting() const { return _settings[SET_VERT_TRACE_ATTR_EXTR]; }

    /** \brief Determines whether trace attributes extracting should be horizontal
     *  (false) or vertical (true).
     */
    void setVerticalTraceAttrExtracting(bool v) { _settings[SET_VERT_TRACE_ATTR_EXTR] = v; }


    /** \brief Returns true if it is trace-id-lookup mode, otherwise false. */
    bool isLookupTraceIDs() const { return _settings[SET_LOOKUP_TRACE_IDS]; }

    /** \brief Determines whether trace attributes extracting should be horizontal
     *  (false) or vertical (true).
     */
    void setLookupTraceIDs(bool v) { _settings[SET_LOOKUP_TRACE_IDS] = v; }
public:
    //-----<Main queries for obtaining log data.>-----
    
    /** \brief Looks into the config parameters collection if a parameter
     *  with key \a param exists. If so, returns a pointer to the parameter value,
     *  otherwise returns nullptr.
     */
    const std::string* getConfigParam(const char* param) const;

    /** \brief Overloaded version of getConfigParam() for string object. */
    const std::string* getConfigParam(const std::string& param) const
    {
        return getConfigParam(param.c_str());
    }

    /** \brief Returns a config param if exists, or empty mutable string. */
    std::string getConfigParamOrEmpty(const char* param) const;

    /** \brief Returns a config param or default empty string if no param exists. */
    std::string& getConfigParamOrDef(const std::string& param)
    {
        return _workQueries[param];
    }

    /** \brief Sets a parameter \a param to the value \a val. */
    void setConfigParam(const std::string& param, const std::string& val)
    {
        _workQueries[param] = val;
    }

    /** \brief Sets AutoLoad config query string.
     *
     *  The query must be formed so that the first (0) column is a parameter,
     *  and the second (1) is a value.
     */
    void setAutoLoadConfigQry(const std::string& s) { _qryLoadConfig = s; }


    /** \brief Returns AutoLoad config query string. */
    std::string getAutoLoadConfigQry() { return _qryLoadConfig; }


 #pragma region Qry Shortcuts
    // shortcuts
    /** \brief (Log) Query for obtaining a number of events in the log. */
    std::string& getQryEventsNumLog() { return getConfigParamOrDef(PAR_LQRY_EVENTS_NUM); }
    
    /** \brief (Log) Query for obtaining a number of traces in the log.*/
    std::string& getQryTracesNumLog() { return getConfigParamOrDef(PAR_LQRY_TRACES_NUM); }
    
    /** \brief (Log) Query for obtaining a number of activities in the log.*/
    std::string& getQryActivitiesNumLog() { return getConfigParamOrDef(PAR_LQRY_ACTIVITIES_NUM); }

    /** \brief (Log) Query for obtaining traces in the log. */
    std::string& getQryTracesLog() { return getConfigParamOrDef(PAR_LQRY_TRACES); }

    /** \brief (Log) Query for obtaining a log attribute. */
    std::string& getQryGetLogAttr() { return getConfigParamOrDef(PAR_LQRY_GETLOGATTR); }

    /** \brief (Log) Query for obtaining all log attributes. */
    std::string& getQryGetAllLogAttrs() { return getConfigParamOrDef(PAR_LQRY_GETALLLOGATTRS); }

    /** \brief (Log) Query for obtaining an event attribute by a trace num and and ofs num. */
    std::string& getQryGetEventAttr() { return getConfigParamOrDef(PAR_LQRY_GETEVENTATTR); }

    /** \brief (Log) Query for obtaining an event attribute by its uniq ID. */
    std::string& getQryGetEventAttrByID() { return getConfigParamOrDef(PAR_LQRY_GETEVENTATTR_BYID); }

    /** \brief (Log) Query for obtaining all attributes of an event by a trace num and and ofs num. */
    std::string& getQryGetAllEventAttrs() { return getConfigParamOrDef(PAR_LQRY_GETALLEVENTATTRS); }


    /** \brief (Log) Query for obtaining all attributes of an event by its id. */
    std::string& getQryGetAllEventAttrsByID() { return getConfigParamOrDef(PAR_LQRY_GETALLEVENTATTRS_BYID); }

    /** \brief (Log) Query for obtaining a trace attribute by a trace ID. */
    std::string& getQryGetTraceAttr() { return getConfigParamOrDef(PAR_LQRY_GETTRACEATTR); }
    //std::string& getGetTraceAttr() { return _qrylGetTraceAttr; }

    /** \brief (Log) Query for obtaining of all attributes of a trace. */
    std::string& getQryGetAllTraceAttrs() { return getConfigParamOrDef(PAR_LQRY_GETALLTRACEATTRS); }

    /** \brief (Log) Query for getting a number of events contatining in a trace. */
    std::string& getQryGetTraceEventsNum() { return getConfigParamOrDef(PAR_LQRY_GETTRACE_EVENTSNUM); }


    #pragma endregion // Qry Shortcuts

protected:
    /** \brief Internal rapid variant of a method for determining if the log is open or not. */
    bool isOpenIntern() const { return _db.isOpen(); }

    /** \brief Loads configuration from a log if the corresponding query is set. */
    void loadConfigFromLog();

    /** \brief Loads configuration from a log by a given query \a confq. */
    void loadConfigFromLog(const std::string& confq);

    /** \brief Processes a collection of config parameters after they to be loaded from the log. */
    void processConfigParams();

    /** \brief Loads a collection of trace IDs and stores them. */
    void loadTraceIDs();

    /** \brief Resets some fields when event log is closed. */
    void reset();

    /** \brief Requests a number of all events from an open log. */
    void requestLogEventsNum();

    /** \brief Requests a number of all traces from an open log. */
    void requestLogTracesNum();

    /** \brief Requests a number of all traces and allocate storage for them. */
    void obtainLogTracesNum();

    /** \brief Requests a number of all activity classes (activities) from an open log. */
    void requestLogActivitiesNum();
   
    /** \brief Extracts attributes of a log. */
    void extractLogAttributes();

    /** \brief Requests an attribute associated with this log. */
    bool requestLogAttr(const char* id, Attribute& a);

    /** \brief Requests all attributes associated with this log. */
    IAttributesEnumerator* requestLogAttrs();

    /** \brief Requests an event attribute #1. */
    bool requestEventAttr(int traceNum, int eventNum, const char* id, Attribute& a);

    /** \brief Requests an event attribute #2. */
    bool requestEventAttrByID(int eventId, const char* id, Attribute& a);

    /** \brief Requests all attributes associated with an event that is determined
     *  by trace and event offset.
     */
    IAttributesEnumerator* requestEventAttrs(int traceNum, int eventNum);

    /** \brief Requests all attributes associated with an event that is determined
     *  by uniq id.
     */
    IAttributesEnumerator* requestEventAttrsByID(int eventId);

    /** \brief Requests a tract attribute. */
    bool requestTraceAttr(int traceNum, const char* id, Attribute& a);
         
    /** \brief Requests all attributes associated with a trace that is determined
     *  by uniq id \a traceNum.
     */
    IAttributesEnumerator* requestTraceAttrs(int traceNum);

    /** \brief Requests and returns a number of events for the given trace \a traceNum. */
    int requestTraceSize(int traceNum);


    /** \brief Allocates a storage for traces according to the number of traces extracted 
     *  prior to invocation of this method.
     */
    void allocTracesStorage();


    /** \brief Deletes all traces objects extracted and stored in the traces storage. */
    void freeTraces();

    /** \brief Binds a trace identifier for a statement \a stmt. In case of direct access, 
     *  an ID is the same as param \a traceNum is. Otherwise, when id-lookup mode is on, 
     *  it binds a corresponding value.
     *
     *  \param paramNum determines of a number of binded paramter.
     */
    void bindTraceID(SQLiteStmt* stmt, int traceNum, int paramNum = 1);

protected:

    /** \brief Check if a given param \a aParam should be used for setting a 
     *  one of the log queries \a qParam to the  value \a val stored by
     *  a STRING \a qry.
     *  \returns true if qParam and aParam are equal to each other.
     */
    bool checkParamLQry(const char* aParam, const char* val, 
        const char* qParam, std::string& qry);


    /** \brief Check if a given param \a aParam should be used for setting a
     *  one of the autoloaded config params \a qParam to the value \a val stored by
     *  a BOOL \a confParam.
     *  \returns true if qParam and aParam are equal to each other.
     */
    bool DEPRECATED checkAutoParam(const char* aParam, const char* val,
        const char* qParam, bool& confParam);

    /** \brief Checks if the param \a par is presented in the collection of
     *  auto-loaded config parameters, and if so, loads its value
     *  into confParam param.
     *
     *  \return true if the paramter is presented, false otherwise.
     *  
     *  To replace obsolete checkAutoParam() method.
     */
    bool checkConfigAutoParam(const char* par, bool& confParam);

protected:
    //-----<SQLite One-Step Query Execution Callbacks>-----

    /** \brief Static callback router function. */
    static int cb_loadConfig(void* sqlog, int colsNum, char** colsData, char** colNames);

    /** \brief Typed callback for LoadConfig query. 
     *
     *  The query for loading a configuration have to provided with at least two columns.
     *  By agreement, colsData[0] is always for param name and colsData[1] is for param value.
     *  All other columns if they are present on the dataset are ignored (e.g. perpective number).    
     */
    void onLoadConfig(int colsNum, char** colsData, char** colNames);

    /** \brief Called by onLoadConfig() when another pair of param-value is loaded. */
    virtual void onLoadConfigRecord(const char* param, const char* value);

protected:
    /** \brief Represents an SQLite DB object. */
    SQLiteDB _db;

    /** \brief Settings (flags) object. */
    SettingsBitset _settings;

    /** \brief SQL query for loading configuration from a log. */
    std::string _qryLoadConfig;

    /** \brief Stores a pointers to extracted traces. */
    VectorOfTraces _traces;

    /** \brief Stores a collection of trace IDs indexed by 0-index. Used in trace-ids-lookup mode. */
    AttributesVector _traceIDs;

    /** \brief A collection of work quieries that are used to obtain individual 
     *  event log characteristic.
     *
     *  It came to replace the obsolete individual query variables.
     */
    SqlQueriesMap _workQueries;

    /** \brief Stores a collection of log attributes in the form of a vector. */
    NmAttributesVectors _logAttributes;


    /** \brief Flag determines if attributes of a log were extracted.  */
    bool _logAttrsExtracted;

    /** \brief Stores a number of events in the log.
     *
     *  Until the opening, stores -1, which determines that the number of events is not
     *  calculated yet.
     */
    int _logEventsNum;

    /** \brief Stores a number of traces in the log.
     *
     *  Until the opening, stores -1, which determines that the number of events is not
     *  calculated yet.
     */
    int _logTracesNum;

    /** \brief Stores a number of traces in the log.
     *
     *  Until the opening, stores -1, which determines that the number of events is not
     *  calculated yet.
     */
    int _logActivitiesNum;

}; // class SQLiteLog



//==============================================================================
// class SQLiteStmtAttrsVertEnumerator
//==============================================================================

/** \brief Special variant of enumerator extracting attributes from the result of the 
 *  given query using "vertical" approach.
 */
class SQLiteStmtAttrsVertEnumerator : public BaseAttributesEnumerator {
public:

    /** \brief Costructor initialize with the given statement \a stmt. 
     *  
     *  \param mngStLifet determines if the lifetime of the statement is managed by
     *  this object. In such case the enumerator will destroy the statement at the last.
     */
    SQLiteStmtAttrsVertEnumerator(SQLiteStmt* stmt, bool mngStLifet = true,
        int idNum = 0, int valNum = 1, int typeNum = 2);

    /** \brief Destructor.  
     *
     *  Destroyes the statement if the instance manages its lifetime.
     */
    virtual ~SQLiteStmtAttrsVertEnumerator();
protected:
    SQLiteStmtAttrsVertEnumerator(const SQLiteStmtAttrsVertEnumerator&);                    // Prevent copy-construction
    SQLiteStmtAttrsVertEnumerator& operator=(const SQLiteStmtAttrsVertEnumerator&);         // Prevent assignment

public:
    //----<IEnumerator<> Implementations>----
    virtual bool hasNext() const override;
    virtual NamedAttribute getNext() override;      // TODO: классы свойств в помощь для корректного возвр. типа!
    virtual void reset() override;
protected:

    
    /** \brief Number of column for the attribute identifier. */
    int _idNum; 
    
    /** \brief Number of column for the attribute value. */
    int _valNum; 
    
    /** \brief Number of column for the attribute type. */
    int _typeNum;

    /** \brief Associated SQLite statement. */
    SQLiteStmt* _stmt;

    /** \brief Flag determines if the lifetime of the statement is managed by
     *  this object.
     */
    bool _mngStLifetime;

    /** \brief Determines if another data row has been successfully fetched from the dataset. */
    bool _isFetched;
}; //  class SQLiteStmtAttrsVertEnumerator
 

//==============================================================================


/** \brief Special variant of enumerator extracting attributes from the result of the
 *  given query using "horizontal" approach.
 */
class SQLiteStmtAttrsHorEnumerator : public BaseAttributesEnumerator {
public:

    /** \brief Constructor initialize with the given statement \a stmt.
     *
     *  \param mngStLifet determines if the lifetime of the statement is managed by
     *  this object. In such case the enumerator will destroy the statement at the last.
     */
    SQLiteStmtAttrsHorEnumerator(SQLiteStmt* stmt, bool mngStLifet = true);

    /** \brief Destructor.
    *
    *  Destroyes the statement if the instance manages its lifetime.
    */
    virtual ~SQLiteStmtAttrsHorEnumerator();
protected:
    SQLiteStmtAttrsHorEnumerator(const SQLiteStmtAttrsHorEnumerator&);                    // Prevent copy-construction
    SQLiteStmtAttrsHorEnumerator& operator=(const SQLiteStmtAttrsHorEnumerator&);         // Prevent assignment

public:
    //----<IEnumerator<> Implementations>----
    virtual bool hasNext() const override;
    virtual NamedAttribute getNext() override;      // TODO: классы свойств в помощь для корректного возвр. типа!
    virtual void reset() override;
protected:

    /** \brief Associated SQLite statement. */
    SQLiteStmt* _stmt;

    /** \brief Flag determines if the lifetime of the statement is managed by
     *  this object.
     */
    bool _mngStLifetime;

    /** \brief Determines a number of attributes in the current query set. */
    int _attrbsNum;

    /** \brief Determines a number of the current attribute. */
    int _curAttr;

}; //  class SQLiteStmtAttrsHorEnumerator

}}} // namespace xi::ldopa::eventlog 


#endif // XI_LDOPA_EVENTLOG_SQLITE_SQLITELOG_H_
