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
/// Tools for building transition systems from a log by using simple state functions.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_ALGOS_TS_SIMPLE_BUILDER_H_
#define XI_LDOPA_TRSS_ALGOS_TS_SIMPLE_BUILDER_H_

//#pragma once


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/eventlog/eventlog.h"
#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"
#include "xi/ldopa/utils/progress_callback.h"
#include "xi/ldopa/utils/elapsed_time.h"


namespace xi { namespace ldopa { namespace ts {;   //

//==============================================================================
//  interface class ITsStateFunc
//==============================================================================


/** \brief Declares an interface for a state function that is used while building a TS. */
class LDOPA_API ITsStateFunc {
public:
    // types
    typedef eventlog::IEventLog IEventLog;        ///< Alias for IEventLog2.
    typedef eventlog::IEventTrace IEventTrace;    ///< Alias for IEventTrace2.
public:

    /** \brief Defines a virtual destructable feature. */
    virtual ~ITsStateFunc() {}
public:

    /** \brief State function: creates a state.
     *
     *  \param traceNum is the number of a processed trace.
     *  \param eventNum determines the number of an event, which is next to the last to be
     *  added in the subtrace ID. That way we have a semi-interval of events to be included
     *  into id: [eventNum - ws, eventNum).
     *  \param[out] actAttr is used for getting back the Activity attribute
     *  of the current event with number \a end.
     */
    virtual const IStateId* makeState(int traceNum, int eventNum,
        IEventLog::Attribute& actAttr) = 0;

    // альтернативный вариант с трассой-объектом
    virtual const IStateId* makeState(IEventTrace* tr, int eventNum,
        IEventLog::Attribute& actAttr) = 0;


    /** \brief Normally invoked before starting a new mining session for 
     *  proper resetting of the function.
     */
    virtual void reset() = 0;
}; // interface ITsStateFunc

//==============================================================================
//  interface class ITsStateFuncVarWs
//==============================================================================

/** \brief Declares an interface for a state function with variadic window size. */
class LDOPA_API ITsStateFuncVarWs
    : public ITsStateFunc
{
public:
    // types
    typedef eventlog::IEventLog IEventLog;        ///< Alias for IEventLog2.
public:

    /** \brief Defines a virtual destructable feature. */
    virtual ~ITsStateFuncVarWs() {}
public:

    /** \brief State function: creates a state.
     *
     *  \param traceNum determines
     *  \param[out] actAttr is used for getting back the Activity attribute
     *  of the current event with number \a end.
     *  \param ws current window size.
     */
    virtual const IStateId* makeState(int traceNum, int eventNum,
        IEventLog::Attribute& actAttr, int ws) = 0;

    // аотернативнй вариант с трассой-объектом
    virtual const IStateId* makeState(IEventTrace* tr, int eventNum,
        IEventLog::Attribute& actAttr, int ws) = 0;


    /** \brief Returns a maximum window size that is appeared while creating states. */
    virtual const int getMaxWndSize() const = 0;
}; // interface ITsStateFuncVarWs



//==============================================================================
//  class PrefixStateFunc
//==============================================================================

/** \brief State function based on a prefix approach with possibly fixed window size.
 *
 *  Uses AttrListStateId class for instantiate stateIDs.
 *
 *  #thinkof: возможно, связку с логом надо перенести либо в сам интерфейс,
 *  либо в базовый класс, который определяет семейство функций состояний, 
 *  основанных на логе.
 *  
 *  Сейчас получается, что этот фид функции сильно связан с типом пула,
 *  а именно с AttrListStateIDsPool, т.к. развязать через базовый интерфейс
 *  IStateIDsPool невозможно, коль скоро он не дает никакой информации о
 *  способе организации идентификаторов, которые в нем хранятся. В то же самое время
 *  для префиксной (а так же для постфиксной, инфиксной) функций важно, что 
 *  идентификаторы хранятся в виде списка. Если мы захотели бы реализовывать
 *  пул другой упорядоченной структурой, тогда нужно завести промежуточный
 *  интерфейс, у которого был бы методы по типу push_front, push_back и, 
 *  возможно, даже, insert в середину. Это не потребуется для функций,
 *  которые состояния хранят в виде map или multimap. В общем, надо
 *  к этому вопросу будет еще вернуться.
 */
class LDOPA_API PrefixStateFunc : public ITsStateFuncVarWs { //ITsStateFunc {
public:

    /** \brief Defines a special value for the unlimited wnd size. */
    static const int UNLIM_WND_SIZE = -1;

    /** \brief Defines a special value for the default wnd size. */
    static const int DEFAULT_WND_SIZE = -2;

public:
    // types
    typedef eventlog::IEventLog IEventLog;        ///< Alias for IEventLog2.
    typedef eventlog::IEventTrace IEventTrace;    ///< Alias for IEventTrace2.
    typedef eventlog::IEvent IEvent;              ///< Alias for IEvent2.

public:
    //----<Constructors and destructor>----
    /** \brief Constructor initializes the function with a ptr to a state IDs pool. */
    PrefixStateFunc(IEventLog* log, AttrListStateIDsPool* stIDsPool, 
        int ws = UNLIM_WND_SIZE);
protected:
    PrefixStateFunc(const PrefixStateFunc&);                 // Prevent copy-construction
    PrefixStateFunc& operator=(const PrefixStateFunc&);      // Prevent assignment

public:
    //----<ITsStateFunc Implementation>----
    virtual const IStateId* makeState(int traceNum, int eventNum,
        IEventLog::Attribute& actAttr) override;

    virtual const IStateId* makeState(IEventTrace* tr, int eventNum,
        IEventLog::Attribute& actAttr);

    virtual void reset() override;

    //----<ITsStateFunc ITsStateFuncVarWs>----
    virtual const IStateId* makeState(int traceNum, int eventNum,
        IEventLog::Attribute& actAttr, int ws) override;

    virtual const IStateId* makeState(IEventTrace* tr, int eventNum,
        IEventLog::Attribute& actAttr, int ws) override;
    
    virtual const int getMaxWndSize() const override;

public:
    //----<Setters/Getters>----

    /** \brief Gets the Activity Attribute ID. */
    std::string getActAttrID() const { return _actAttrID; }
    
    /** \brief Sets an Activity Attribute ID. */
    void setActAttrID(const std::string& atid) { _actAttrID = atid; }

    /** \brief Sets new ws to \a ws. */
    void setWS(int ws) { _ws = ws; }

    /** \brief Gets the current ws. */
    int getWS() const { return _ws; }
protected:

    /** \brief Makes an ID for a subtrace, which is determined by the trace
     *  num \a traceNum, a number \a beg of a first event in the subtrace,
     *  and a number \a end of the last event.
     *
     */
    const IStateId* makeSubtraceID(int traceNum, int beg, int end,
        IEventLog::Attribute& actAttr);

    // альтернативный вариант с трассой-объектом
    const IStateId* makeSubtraceID(IEventTrace* tr, int beg, int end,
        IEventLog::Attribute& actAttr);

    /** \brief Internal implementation of makeStateFunc. */
    const IStateId* makeStateInternal(int traceNum, int eventNum,
        IEventLog::Attribute& actAttr, int ws);

    // альтернативный вариант с трассой-объектом
    const IStateId* makeStateInternal(IEventTrace* tr, int eventNum,
        IEventLog::Attribute& actAttr, int ws);

protected:
    
    /** \brief Stores a ptr to an event log.  */
    IEventLog* _log;

    /** \brief A specific pool for specific stateIDs. */
    AttrListStateIDsPool* _stIDsPool;


    /** \brief Stores a max wnd size (limit). */
    int _ws;
   
    /** \brief Stores a maximum size of a window during states making process. */
    int _maxWndSize;

    /** \brief Stores an ID for the Activity attribute. */
    std::string _actAttrID;
}; // class PrefixStateFunc



//==============================================================================
//  class TsBuilder
//==============================================================================


/** \brief Implements an algorithm for building a TS with a custom state function.
 *
 *  TS that is created during builder work is stored by the builder until next
 *  building or the TS is detached by using an appropriate method (detach).
 *  Normally, the builder manages the lifetime of a builded TS.
 */
class LDOPA_API TsBuilder 
        : public ElapsedTimeStore       // вспомогательный класс для учета времени алгоритма
{
public:
    //-----<Types>-----

    /** \brief Enumerates setting bit number constants. */
    enum
    {
        SET_AUTO_CLEAR_ST_POOL = 0,     ///< Determines whether the stateIDs pool is cleared before mining.
        //SET_SINGLE_ACPT_STATE = 1,      ///< Determines whether the single accepting state must be added.
        //
        //
        SET__LAST                       ///< Technical internal value used to determine size for underlying bitset
    };

    /** \brief A typename for bitset type for storing settings */
    typedef std::bitset<SET__LAST> SettingsBitset;

    /** \brief Alias for the datatype of a currently created TS. */
    typedef EvLogTSWithFreqs TS;
    typedef TS::State TsState;          ///< TS state.
    typedef TS::StateIter TsStateIter;  ///< TS state iterator.

public:
    // types
    typedef eventlog::IEventLog IEventLog;        ///< Alias for IEventLog2.
    typedef eventlog::IEventTrace IEventTrace;    ///< Alias for IEventTrace2.

    /** \brief Declares a set of log attributes. */
    typedef std::set<IEventLog::Attribute> AttributesSet;

    typedef IEventLog::Attribute Attribute;     // Added as of 18/08/2018: надо везд заменить упрощенкой

public:
    //-----<Const>-----

    // default settings
    static const unsigned int DEF_SETTINGS =
        (0x0 << SET_AUTO_CLEAR_ST_POOL          // Do not clear stateIDs map by default.
        //| 0x0 << SET_SINGLE_ACPT_STATE          // With no single accepting state so far.
        //| 0x1 <<                              //
        );
public:
    //----<Constructors and destructor>----
    
    /** \brief Constructor initializes the builder with a log and a state function. */
    TsBuilder(IEventLog* log, ITsStateFunc* sf, IStateIDsPool* stIDsPool);

    /** \brief Destructor. */
    ~TsBuilder();

protected:
    TsBuilder(const TsBuilder&);                 // Prevent copy-construction
    TsBuilder& operator=(const TsBuilder&);      // Prevent assignment

public:
    //----<Main interface>----
    
    /** \brief Builds a TS. 
     *
     *  \param trackActivities determines whether all log activities should be added to a special
     *  set of activities during inferring TS (true), or not (false).
     *  \returns a TS if built successfully, otherwise nullptr.
     *  A newly built TS is managed by the builder according to the rules,
     *  presented in the class definition.
     */
    TS* build(bool trackActivities = true);

    /** \brief Detaches and returns the TS built at the previous call of build() method. */    
    TS* detach();

    /** \brief Cleans a previously built TS if exists. If no, does nothing. */
    void cleanTS();

public:
    //----<Setters/Getters>----

    /** \brief Returns a ptr to the the TS built at the previous call of build() method. */
    TS* getTS() { return _ts; }

    /** \brief Const overload for getTS(). */
    const TS* getTS() const { return _ts; }

    /** \brief Sets a progress callback. */
    void setProgressCB(IProgressCallback* pcb) { _progrCB = pcb; }

    /** \brief Returns a progress callback. */
    IProgressCallback* getProgressCB() const { return _progrCB; }
public:
    //-----<Helpers for work with settings>-----

    /** \brief Getter for SET_AUTO_CLEAR_ST_POOL option. */
    bool isAutoClearStateIDsPool() const { return _settings[SET_AUTO_CLEAR_ST_POOL]; }

    /** \brief Setter for SET_AUTO_CLEAR_ST_POOL option. */
    void setAutoClearStateIDsPool(bool f) { _settings[SET_AUTO_CLEAR_ST_POOL] = f; }

protected:
    //----<Algorith decomposition>----
    
    /** \brief Proceesses anothoer trace from the log. */
    void processTrace(int i);

    // альтернативный подход с трассой, как объектом
    void processTrace(IEventTrace* tr);

    /** \brief Makes a new (single) accepting state and flows all previous AS to it. */
    void makeSingleAcptState();
protected:
    /** \brief Settings (flags) object. */
    SettingsBitset _settings;

    /** \brief Stores a ptr to an event log.  */
    IEventLog* _log;

    /** \brief Stores a state function. */
    ITsStateFunc* _sf;

    /** \brief Stores a ptr to a pool of state IDs. */
    // #thinkof: здесь у нас идет тот же пул, что и в функции... если они разные?!
    // должна ли функция стать более сложным компонентом — создающим дефолтную TS в т.ч.?
    // например, ITsElementsCreator?
    IStateIDsPool* _stIDsPool;

    /** \brief Stores a resulting TS. */
    TS* _ts;

    /** \brief Stores the number of all traces in the log, processed at the last build. */
    int _tracesNum;

    /** \brief Progress callback. */
    IProgressCallback* _progrCB;

    /** \brief Store last progress value to decrease the frequency of progress updater. */
    int _lastProgress;

}; //  class TsBuilder

}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_ALGOS_TS_SIMPLE_BUILDER_H_
