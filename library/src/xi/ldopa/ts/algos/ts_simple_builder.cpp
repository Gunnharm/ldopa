// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/ts_simple_builder.h"

namespace xi { namespace ldopa { namespace ts { ;   //


//==============================================================================
// class PrefixStateFunc
//==============================================================================


PrefixStateFunc::PrefixStateFunc(IEventLog* log, AttrListStateIDsPool* stIDsPool, 
    int ws /*= UNLIM_WND_SIZE*/)
    : _log(log)
    , _stIDsPool(stIDsPool)
    , _ws(ws)
    , _maxWndSize(UNLIM_WND_SIZE)           // еще не определялся
{
    if (_ws < 0 && _ws != UNLIM_WND_SIZE)
        throw std::invalid_argument("Invalid window size.");
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeState(IEventTrace* tr, int eventNum,
    IEventLog::Attribute& actAttr)
{
    return makeStateInternal(tr, eventNum, actAttr, _ws);
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeState(int traceNum, int eventNum,
    IEventLog::Attribute& actAttr)
{
    return makeStateInternal(traceNum, eventNum, actAttr, _ws);
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeState(IEventTrace* tr, int eventNum,
    IEventLog::Attribute& actAttr, int ws)
{
    if (ws == DEFAULT_WND_SIZE)
        ws = _ws;

    return makeStateInternal(tr, eventNum, actAttr, ws);
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeState(int traceNum, int eventNum,
    IEventLog::Attribute& actAttr, int ws)
{
    if (ws == DEFAULT_WND_SIZE)
        ws = _ws;
    
    return makeStateInternal(traceNum, eventNum, actAttr, ws);
}

//------------------------------------------------------------------------------

const int PrefixStateFunc::getMaxWndSize() const
{
    return _maxWndSize;
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeStateInternal(IEventTrace* tr, int eventNum,
    IEventLog::Attribute& actAttr, int ws)
{
    int beg;                        // первый ивент в цепочке
    if (ws == UNLIM_WND_SIZE)
        beg = 0;
    else
    {
        beg = eventNum - ws;       // не больше размера окна
        if (beg < 0)
            beg = 0;
    }

    // вот здесь корректируем макс. размер окна, если устанавливался
    int realWndSize = eventNum - beg;
    if (realWndSize > _maxWndSize) // актуальный размер!
        _maxWndSize = realWndSize;

    return makeSubtraceID(tr, beg, eventNum, actAttr);
}


//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeStateInternal(int traceNum, int eventNum,
    IEventLog::Attribute& actAttr, int ws)
{
    int beg;                        // первый ивент в цепочке
    if (ws == UNLIM_WND_SIZE)
        beg = 0;
    else
    {
        beg = eventNum - ws;       // не больше размера окна
        if (beg < 0)
            beg = 0;
    }

    // вот здесь корректируем макс. размер окна, если устанавливался
    int realWndSize = eventNum - beg;
    if (realWndSize > _maxWndSize) // актуальный размер!
        _maxWndSize = realWndSize;

    return makeSubtraceID(traceNum, beg, eventNum, actAttr);
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeSubtraceID(IEventTrace* tr, int beg, int end,
    IEventLog::Attribute& actAttr)
{
    // метод защищенный, поэтому подразумеваем, что beg и eventNum подработаны правильно
    AttrListStateId av;

    //===<< новый вариант с ООП-интерфейсом, направленный на умнеьшение обращений к БД >>===
    //IEventTrace2* tr = _log->getTrace(traceNum);
    if (!tr)
        throw LdopaException("Error getting trace.");

    for (int eventNum = beg; eventNum < end; ++eventNum)
    {
        IEvent* ev = tr->getEvent(eventNum);
        if (!ev)
            throw LdopaException("Error getting event.");
        if (!ev->getAttr(_actAttrID.c_str(), actAttr))
            throw LdopaException("Error getting event's attribute.");

        // наконец, атрибут взят
        av.getAttrs().push_back(actAttr);
    }

    // после того, как сформирован очередной ИД состояния, добавляем его в пул
    // идентификаторов, там он может смерджиться с уже существующим таким же
    // и вернется указатель на существующий, а если не было — то на новый

    const AttrListStateId* stID = (*_stIDsPool)[av];
    return stID;
}

//------------------------------------------------------------------------------

const IStateId* PrefixStateFunc::makeSubtraceID(int traceNum, int beg, int end,
    IEventLog::Attribute& actAttr)
{
    // метод защищенный, поэтому подразумеваем, что beg и eventNum подработаны правильно

    AttrListStateId av;


    //===<< предыдущий вариант с процедурным интерфейсом >>===
    for (int eventNum = beg; eventNum < end; ++eventNum)
    {
        if (!_log->getEventAttr(traceNum, eventNum, _actAttrID.c_str(), actAttr))
            throw LdopaException("Another event doesn't have the Activity attribute.");

        // если атрибут есть, все ок: добавляем его в префикс
        av.getAttrs().push_back(actAttr);
    }


    // после того, как сформирован очередной ИД состояния, добавляем его в пул
    // идентифкаторов, там он может смерджиться с уже существующим таким же
    // и вернется указатель на существующий, а если не было — то на новый

    const AttrListStateId* stID = (*_stIDsPool)[av];
    return stID;
}

//------------------------------------------------------------------------------

void PrefixStateFunc::reset()
{
    // TODO: здесь по флагу как раз можно сбрасывать предыдущий пул
    
    _log->open();               // открываем, вдруг не открыт
    
    // если ид атрибута "Активити" не было задано пользователем руками,
    // попробуем взять его из лога; если же и там баранка, все на этом
    if (_actAttrID.empty())
    {
        _actAttrID = _log->getEvActAttrId();
        if (_actAttrID.empty())
            throw LdopaException("Can't prepare PrefixStateFunc: ID for Activity attribute not set.");
    }

    // сбрасываем максимальный разме
    _maxWndSize = UNLIM_WND_SIZE;
}





//==============================================================================
// class TsPrefixBuilder
//==============================================================================



TsBuilder::TsBuilder(IEventLog* log, ITsStateFunc* sf, IStateIDsPool* stIDsPool)
    : _log(log)
    , _sf(sf)
    , _ts(nullptr)
    , _stIDsPool(stIDsPool)
    , _tracesNum(0)
    , _progrCB(nullptr)
    //, _logActivities(nullptr)
{
}

//------------------------------------------------------------------------------

TsBuilder::~TsBuilder()
{
    // если TS не была отдетачена, значит тот билдер управляет временем жизни ее
    cleanTS();
}

//------------------------------------------------------------------------------


TsBuilder::TS* TsBuilder::build(bool trackActivities)
{
    cleanTS();

    // проверяем корректность входных данных
    if (!_log)
        throw LdopaException("Can't build a TS: no event log is set.");
    if (!_sf)
        throw LdopaException("Can't build a TS: no state function is set.");
    if (!_stIDsPool)
        throw LdopaException("Can't build a TS: no state IDs pool is set.");
   

    // подготовим функцию
    _sf->reset();

    XI_LDOPA_ELAPSEDTIME_START(timer)
    _ts = new TS(_stIDsPool);


    _tracesNum = _log->getTracesNum();
    _ts->setTracesNum(_tracesNum);              // сохраняем инф. о числе трасс в TS!

    if (_progrCB)                               // если есть коллбекер прогресса
        _progrCB->progress(0);
    _lastProgress = 0;                          // обнуляем прогресс

    // перебираем множество всех трасс     
    for (int i = 0; i < _tracesNum; ++i)
    {
        IEventTrace* tr = _log->getTrace(i);
        processTrace(tr);
        //processTrace(i);

         
        if (_progrCB)
        {
            // считаем текущий прогресс в процентах и все уведомления шлем только,
            // если он изменился. иначе, как показали эксперименты, из-за очень
            // частых вызовов существенно (порядки) теряем в производительности. Возможно
            // это из-за маршаллинга в C++/CLI
            int curPr = (int)((double)i / (double)_tracesNum * 100.);
            if (curPr != _lastProgress)
            {
                _lastProgress = curPr;
                // уведомляем по числу трасс и смотрим, не надо ли прервать
                IProgressCallback::Res res = _progrCB->progress(curPr);
                if (res == IProgressCallback::ResCancel)
                    throw LdopaException("Interrupted at the request of the caller.");
            }
        }
    }


    if (_progrCB)                               // если есть коллбекер прогресса
        _progrCB->progress(100);

    // сомнительная по хорошести добавочка: если использовалась функция
    // состояния с окном, зададим макс. размер окна, иначе -1 (неизвестно)
    ITsStateFuncVarWs* vwsFun = dynamic_cast<ITsStateFuncVarWs*>(_sf);
    if (vwsFun)
        _ts->setMaxWS(vwsFun->getMaxWndSize());
    else
        _ts->setMaxWS(-1);

    XI_LDOPA_ELAPSEDTIME_STOP(timer)

    return _ts;
}


//------------------------------------------------------------------------------

void TsBuilder::processTrace(IEventTrace* tr)
{
    // пред состояние: начинаем с начального
    TS::State prevState = _ts->getInitState();

    int traceSize = tr->getSize();   // _log->getTraceSize(traceNum);

    // перебираем все события в трассе
    // здесь нумеруем с 1, подразумевая, что это +1-событие будет в невходящей
    // части полуотрезка [eventNum - ws, eventNum), т.е. по сути это некоторая
    // оптимизация вместо прибавления 1-цы в вызове метода makeState()
    for (int evNum = 1; evNum <= traceSize; ++evNum)
    {
        IEventLog::Attribute actAttr;
        const IStateId* stID = _sf->makeState(tr, evNum, actAttr);
        if (!stID)
            throw LdopaException("Error when creating a state ID in processTrace().");

        // получаем соответствующее состояние и между состояниями навешиваем транзицию
        TS::State curState = _ts->getOrAddState(stID);
        TS::Transition tr = _ts->getOrAddTransF(prevState, curState, actAttr, 1);

        prevState = curState;
    }

    // если в трассе было хотя бы одно событие, то появилось состояние, помимо
    // начального, которое надо отметить принимающим
    if (traceSize > 0)
        _ts->setAcceptingState(prevState, true);
}

//------------------------------------------------------------------------------

void TsBuilder::processTrace(int traceNum)
{
    // пред состояние: начинаем с начального
    TS::State prevState = _ts->getInitState();

    int traceSize = _log->getTraceSize(traceNum);

    // перебираем все события в трассе
    // здесь нумеруем с 1, подразумевая, что это +1-событие будет в невходящей
    // части полуотрезка [eventNum - ws, eventNum), т.е. по сути это некоторая
    // оптимизация вместо прибавления 1-цы в вызове метода makeState()
    for (int evNum = 1; evNum <= traceSize; ++evNum)
    {
        IEventLog::Attribute actAttr;
        const IStateId* stID = _sf->makeState(traceNum, evNum, actAttr);
        if (!stID)
            throw LdopaException("Error when creating a state ID in processTrace().");

        // получаем соответствующее состояние
        TS::State curState = _ts->getOrAddState(stID);

        // между состояниями навешиваем транзицию
        TS::Transition tr = _ts->getOrAddTransF(prevState, curState, actAttr, 1);
        prevState = curState;

    }

    // если в трассе было хотя бы одно событие, то появилось состояние, помимо
    // начального, которое надо отметить принимающим
    if (traceSize > 0)
        _ts->setAcceptingState(prevState, true);
}


//------------------------------------------------------------------------------

void TsBuilder::cleanTS()
{
    if (_ts)        
    {
        delete _ts;
        _ts = nullptr;
    }
}

//------------------------------------------------------------------------------

TsBuilder::TS* TsBuilder::detach()
{
    if (!_ts)
        return nullptr;
    
    TS* ts = _ts;
    _ts = nullptr;

    return ts;
}

}}} // namespace xi { namespace ldopa { namespace ts {
