// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/freq_condenser.h"

#include <math.h>       // round

#include "boost/tuple/tuple.hpp"
using boost::tie;


namespace xi { namespace ldopa { namespace ts { ;   //


//==============================================================================
// class CondensedTsBuilder
//==============================================================================

CondensedTsBuilder::CondensedTsBuilder(TS* ts)
    : _srcTs(ts)
    , _ts(nullptr)
    , _threshold(0)
{

}

//------------------------------------------------------------------------------

CondensedTsBuilder::~CondensedTsBuilder()
{
    // если TS не была отдетачена, значит тот билдер управляет временем жизни ее
    cleanTS();
}

//------------------------------------------------------------------------------

void CondensedTsBuilder::cleanTS()
{
    if (_ts)
    {
        delete _ts;
        _ts = nullptr;
    }
}


//------------------------------------------------------------------------------

CondensedTsBuilder::TS* CondensedTsBuilder::detach()
{
    if (!_ts)
        return nullptr;

    TS* ts = _ts;
    _ts = nullptr;

    return ts;
}

//------------------------------------------------------------------------------

CondensedTsBuilder::TS* CondensedTsBuilder::build(double threshold)
{
    if (!_srcTs)
        throw LdopaException("No source (full) TS set.");

    // проверим порог
    if (threshold < 0 || threshold > 1)
        throw std::invalid_argument("Bad threshold.");

    _threshold = threshold;
    _minPresFreq = (int)round(threshold * _srcTs->getTracesNum());      // TODO: проконтроллировать


    // очищаем предыдущую TS, если была
    cleanTS();

    XI_LDOPA_ELAPSEDTIME_START(timer)
    _ts = new TS(*_srcTs);                                  // создаем точную копию пока

    markGauntTransitions();                                 // находим и помечаем все тонкие дуги
    removeMarkedTransitions();                              // удаляем помеченные дуги
    removeWidowStates();                                    // удаляем "вдовьи" вершины
    XI_LDOPA_ELAPSEDTIME_STOP(timer)

    return _ts;
}


//------------------------------------------------------------------------------

void CondensedTsBuilder::markGauntTransitions()
{
    // если что-то осталось с прошлого раза, уберем!
    _trans2excl.clear();                                

    // берем все переходы и проверяем их частотность
    
    for (TS::LogTS::TransIterPair trs = _ts->getTransitions();
        trs.first != trs.second; ++trs.first)
    {
        TS::Transition& t = *(trs.first);               // для удобства
        EvLogTSWithFreqs::IntRes tr_r = _ts->getTransFreq(t);

        // TODO: если частота для заданной дуги не задана, мы ее пока не удаляем, 
        // но в целом не очень понятно, от чего это может быть и как надо на это реагировать
        if (!tr_r.second)
            continue;                                   // возможно, надо что-то конкретнее сделать

        // если частота установлена, проверим, проходит ли эта дуга по порогу и добавим
        // в коллекцию "удаляемых", если не проходит
        // сокрасно Def.7 [Shershakov et al. 2017] это "minimum preserved frequency"
        if (tr_r.first < _minPresFreq)
            _trans2excl.insert(t);
    }
}

//------------------------------------------------------------------------------

void CondensedTsBuilder::removeMarkedTransitions()
{
    for (SetOfTransIter it = _trans2excl.begin(); it != _trans2excl.end(); ++it)
        _ts->removeTrans(*it);
}

//------------------------------------------------------------------------------

void CondensedTsBuilder::removeWidowStates()
{
    // см. про стабильность: http://www.boost.org/doc/libs/1_37_0/libs/graph/doc/adjacency_list.html

    // отдельно возьмем начальное состояние
    TS::State init = _ts->getInitState();

    // переберем все вершины
    TS::LogTS::StateIter vi, viEnd, next;
    tie(vi, viEnd) = _ts->getStates();
    for (next = vi; vi != viEnd; vi = next)
    { 
        ++next;                                             // сразу переходим к след, чтобы не инвалидировать удалением
        TS::State& s = *vi;                                 // текущее состояние
        if (!_ts->hasInTransitions(s) && (s != init))
            _ts->removeState(s);
    }
    
}





}}} // namespace xi { namespace ldopa { namespace ts {
