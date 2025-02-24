// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/varws_ts_rebuilder.h"

#include <cmath>

namespace xi { namespace ldopa { namespace ts { ;   //


//==============================================================================
//  class VarWsTsBuilder
//==============================================================================

VarWsTsBuilder::VarWsTsBuilder(IEventLog* log, TS* ts, ITsStateFuncVarWs* sf)
    : _log(log)
    , _sf(sf)
    , _srcTs(ts)
    , _ts(nullptr)
    , _tracesNum(0)
    , _0wsStateSet(false)
{
}

//------------------------------------------------------------------------------

VarWsTsBuilder::~VarWsTsBuilder()
{
    cleanTS();
}

//------------------------------------------------------------------------------

VarWsTsBuilder::TS* VarWsTsBuilder::detach()
{
    if (!_ts)
        return nullptr;

    TS* ts = _ts;
    _ts = nullptr;

    return ts;
}

//------------------------------------------------------------------------------

void VarWsTsBuilder::cleanTS()
{
    if (_ts)
    {
        delete _ts;
        _ts = nullptr;
    }
}

//------------------------------------------------------------------------------

void VarWsTsBuilder::reset()
{
    // если лог не открыт, надо исправить, а также запросить из лога ИД атрибута, определяющего активность
    _log->open();
    _actAttrID = _log->getEvActAttrId();
    if (_actAttrID.empty())
        throw LdopaException("Can't prepare VarWsTsBuilder: ID for Activity attribute is not set.");

    // сохраним число трасс и подготовим массив последних ивентов для трасс
    _tracesNum = _log->getTracesNum();
    _tracesCompl.clear();
    _tracesCompl.resize(_tracesNum, IntStatePair(0, TS::State()));     // 0 — ни одной трассы не готов

    // множество временных транзиций
    _tempTrans.clear();

}

//------------------------------------------------------------------------------

VarWsTsBuilder::TS* VarWsTsBuilder::build(double vwsc, ZeroSizeWndAction zsa)
{
    if (_log == nullptr)
        throw LdopaException("Can't build a TS: no event log is set.");

    if (_srcTs == nullptr)
        throw LdopaException("No source (condensed) TS is set.");

    // сохраним параметры
    _vwsc = vwsc;
    _zsa = zsa;
    
    // очищаем предыдущую TS, если была
    cleanTS();

    XI_LDOPA_ELAPSEDTIME_START(timer)
    _ts = new TS(*_srcTs);                                  // создаем точную копию

    // инициализируем лог, число трасс и связанные структуры
    reset();
    //replayLoop();                                           // главный цикл восстановления
    replayLoop();                                           // главный цикл восстановления
    XI_LDOPA_ELAPSEDTIME_STOP(timer)

    return _ts;                                             // TODO:
}

//------------------------------------------------------------------------------

void VarWsTsBuilder::replayLoop()
{
    // по крайней мере один раз все трассы надо проиграть
    bool uncompleteTraces;
    do {
        uncompleteTraces = false;

        // перебираем все трассы        
        for (int i = 0; i < _tracesNum; ++i)
        {
            if (!replayTrace(i))
                uncompleteTraces = true;
        }

        // если хотя бы одну трассу не удалось проиграть, надо восстановить состояния всей системы
        if (uncompleteTraces)
        {
            for (int i = 0; i < _tracesNum; ++i)
                restateTrace(i);
        }
    } while (uncompleteTraces);
}


//------------------------------------------------------------------------------

bool VarWsTsBuilder::replayTrace(int traceNum)
{
    IEventTrace* tr = _log->getTrace(traceNum);
    if (!tr)
        throw LdopaException("Error getting a trace.");
    int traceSize = tr->getSize();

    // смотрим, была ли тек. трасса полностью проиграна
    int lastEventNum = getLastEventNum(traceNum);
    if (lastEventNum == traceSize)
        return true;                                    // была, все

    TS::State curState;                                 // текущее состояние, с которого начнем бегать
    if (lastEventNum == 0)
        curState = _ts->getInitState();                 // текущее состояние, с которого начнем бегать
    else
        curState = getLastEventState(traceNum);         // иначе тек. состояние с последнего проигранного           

    // если же не была, проходимся по всем ивентам текущей трассы, начиная с 
    // последнего состояния, которым закончили в прошл. раз
    for (int i = lastEventNum; i < traceSize; ++i)
    {
        // возьмем активность текущего ивента
        IEventLog::Attribute actAttr;
        //if (!_log->getEventAttr(traceNum, i, _actAttrID.c_str(), actAttr))
        //    throw LdopaException("Another event doesn't have the Activity attribute.");

        IEvent* ev = tr->getEvent(i);
        if (!ev)
            throw LdopaException("Error getting event.");
        if (!ev->getAttr(_actAttrID.c_str(), actAttr))
            throw LdopaException("Error getting event's attribute.");

        // ищем подходящий переход: т.к. у нас строго детерменированные TS, если он есть, то один
        TS::TransRes trRes = _ts->getFirstOutTrans(curState, actAttr);
        if (trRes.second)                                               // есть такой
        {
            TS::Transition& t = trRes.first;

            // определяем, является ли найденных переход временным
            if (isTransTemp(t))                                         // является временной
            {
                incTransFreq(t);                                        // увеличим частоту временной
                setLastEvNumState(traceNum, i, curState);               // сохраняем предыдущее состояние!!!

                return false;
            }

            // если же переход постоянный, т.е. все ок, двинемся далее затем
            curState = _ts->getTargState(t);                            // новое опорное состояние
        }
        else                                                            // если же искомой транзиции нет
        {
            TS::State sa = _ts->addAnonState();                         // создаем новое анонимное состояние
            TS::Transition ta = _ts->getOrAddTransF(curState, sa,
                actAttr, 1);        // сразу частоту увеличим
            markTransAsTemp(ta, true);                                  // помечаем переход временным
            setLastEvNumState(traceNum, i, curState);                   // сохраняем предыдущее состояние!!!

            return false;                                               // и пока с этой трассой все...
        }   // if ... else        
    }   // for...

    // если вышли из цикла естественным образом, значит трасса полностью проиграна
    setLastEventNum(traceNum, traceSize);

    return true;
}

//------------------------------------------------------------------------------

void VarWsTsBuilder::restateTrace(int traceNum)
{
    IEventTrace* tr = _log->getTrace(traceNum);    // здесь проверку существования не делаем, т.к. раньше грхнулась бы
    int traceSize = tr->getSize();
    //int traceSize = _log->getTraceSize(traceNum);
    
    int curEventNum;
    TS::State regState;                             // последнее валидное регулярное состояние
    boost::tie(curEventNum, regState) = getLastEvNumState(traceNum);


    // APPNOTE: 18/02/2018: чтобы еще чуть-чуть выиграть, можно пометку -1 ставить, означающую, что конец,
    // вместо размера трассы, чтобы избежать запроса объекта трассы и сразу пропускать

    if (curEventNum == traceSize)                   // если трасса вся
        return;                                     // собственно, на этом и все

    // т.к. трасса еще не проигрывается до конца, значит curEventNum является 0-based 
    // номером первого ивента в этой трассе, который не проигрывается на регулярной
    // основе, а в state - предыдущее состояние в системе переходов, от которого
    // ведет временный переход, помеченный активностью этого непроигранного ивента

    // берем активность, которой помечен временный переход, т.е. она уже сто раз фигурировала,
    // не проверяем ее существование; так же не проверяем, что переход есть, и есть
    // же временное состояние, к которому он ведет
    IEventLog::Attribute actAttr;

    IEvent* ev = tr->getEvent(curEventNum);
    ev->getAttr(_actAttrID.c_str(), actAttr);       // не проверяем по указанной выше причине

    //_log->getEventAttr(traceNum, curEventNum, _actAttrID.c_str(), actAttr);

    TS::Transition tempTrans = _ts->getFirstOutTrans(regState, actAttr).first;
    TS::State tempState = _ts->getTargState(tempTrans);

    // вообще, при восстановлении трасс мы для данной подтрассы могли временные
    // переход и состояния уже на какой-то предыдущей трассе восстановить, 
    // и тогда просто не надо ничего делать для этой трассы
    if (!isTransTemp(tempTrans))
        return;

    // ну, а если же переход все-таки временный, тогда делаем из него постоянный:
    // главная идея предложенного подхода

    int freq = _ts->getTransFreq(tempTrans).first;          // пометка частоты для перехода
    int maxWndSize = _ts->getMaxWS();

    // рассчитываем размер окна персонально для данного перехода
    int wndSize = (int)round((double)maxWndSize * (double)freq * _vwsc / (double)_log->getTracesNum());

    TS::State newState;
    bool newStateSet = false;

    // если размер окна получился равным нулю, значит текущая подтрасса по сути
    // редко появляющаяся, и есть варианты: либо ее в мусор, либо ограничить
    // минимальный размер окна единицей
    if (wndSize == 0)
    {
        if (_zsa == ZeroSizeWndAction::zsaSet1)
            wndSize = 1;
        else                                                // мусорное состояние
        {
            newState = get0WsState();
            newStateSet = true;
        }
    }

    if (!newStateSet)                                       // если еще не разобрались с состоянием
    {
        // здесь прибавляем к номеру события 1, т.к. следующий за нужным номер ивента попадет в
        // невходящую часть полуотрезка [eventNum - ws, eventNum)
        // см. тж. TsBuilder::processTrace().
        //const IStateId* stID = _sf->makeState(traceNum, curEventNum + 1, actAttr, wndSize);
        const IStateId* stID = _sf->makeState(tr, curEventNum + 1, actAttr, wndSize);
        if (!stID)
            throw LdopaException("Error creating a state ID in restateTrace().");
        newState = _ts->getOrAddState(stID);
    }

    // теперь удаляем пред. состояние и переход
    markTransAsTemp(tempTrans, false);                      // убираем переход из множества временных
    _ts->removeState(tempState);                            // убирая состояние из графа, автоматом уйдут связанные ребра

    // наконец, наводим новый регулярный переход между последним норм. состоянием и новым состоянием
    _ts->getOrAddTransF(regState, newState, actAttr, freq);  // пусть уже и частота сохранится

    // наконец, если это последний символ трассы, надо пометить состояние принимающим
    if (curEventNum = traceSize - 1)
        _ts->setAcceptingState(newState, true);
}


//------------------------------------------------------------------------------

void VarWsTsBuilder::incTransFreq(TS::Transition t)
{

    int freq;
    bool hasFreq;
    boost::tie(freq, hasFreq) = _ts->getTransFreq(t);
    if (!hasFreq)
        freq = 0;

    ++freq;
    _ts->setTransFreq(t, freq);
}

//------------------------------------------------------------------------------

VarWsTsBuilder::TS::StateRes VarWsTsBuilder::replayUpToNthState(TS::State initSt, 
    int traceNum, int eventNum)
{
    TS::State curState = initSt;
    for (int i = 0; i < eventNum; ++i)              // TODO: проверить, вдруг ли тут д.б. i <= eventNum
    {
        // активность текущего ивента
        IEventLog::Attribute actAttr;
        if (!_log->getEventAttr(traceNum, i, _actAttrID.c_str(), actAttr))
            throw LdopaException("replayUpToNthState(): Another event doesn't have the Activity attribute.");
        // ищем подходящий переход: т.к. у нас строго детерменированные TS, если он есть, то один
        TS::TransRes trRes = _ts->getFirstOutTrans(curState, actAttr);
        
        if (!trRes.second)                          // есть нет такого перехода
            return TS::StateRes(TS::State(), false);

        TS::Transition& t = trRes.first;
        curState = _ts->getTargState(t);            // к след. состоянию по переходу
    }
    // выход из цикла - признак успешного реплея

    return TS::StateRes(curState, true);
}

//------------------------------------------------------------------------------

VarWsTsBuilder::TS::State VarWsTsBuilder::get0WsState()
{
    // если спец. состояние еще не было добавлено, сделаем это
    if (!_0wsStateSet)
    {
        _0wsState = _ts->addAnonState();
        _0wsStateSet = true;
    }

    return _0wsState;
}

//------------------------------------------------------------------------------



}}} // namespace xi { namespace ldopa { namespace ts {
