// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/ts_metrics_calc.h"
#include "xi/ldopa/ts/algos/dual_ts_simulator.h"

#include <cmath>    // <math.h> // sqrt


namespace xi { namespace ldopa { namespace ts { ;   //

//==============================================================================
//  class TsMetricsCalc
//==============================================================================


TsMetricsCalc::TsMetricsCalc(IEventLog* log, //const AttributesSet* logActivities, 
    TsMetricsCalc::TS* _fullTs /*= nullptr*/)
    : _log(log)
    //, _logActivities(logActivities)
    , _ts(nullptr)
    , _fullTs(_fullTs)
    , _calculatedSimplicity(-1)     // has not been calculated yet
    , _calculatedPrecision(-1)      // has not been calculated yet
    , _calculatedGeneralization(-1) // has not been calculated yet
{

}

//------------------------------------------------------------------------------

void TsMetricsCalc::setAndCheckTs(TS* ts)
{
    if (ts)
        _ts = ts;
    if (!_ts)
        throw LdopaException("Can't calculate metrics: TS is null.");
}

//------------------------------------------------------------------------------

double TsMetricsCalc::calcSimplicity(TS* ts)
{
    // TODO: возможно, проверку _log надо вынести в setAndCheckTs
    if (!_log)
        throw LdopaException("Can't calc TS simplicity: no event log is set.");
    setAndCheckTs(ts);

    // три составляющие simplicity
    double nE = (double)(_log->getActivitiesNum());     // число (классов) активностей    
    double nS = (double)ts->getStatesNum();             // число состояний в СП
    double nT = (double)ts->getTransitionsNum();        // число переходов в СП
     
    _calculatedSimplicity = (nE + 1) / (nS + nT);

    return _calculatedSimplicity;
}

//------------------------------------------------------------------------------

double TsMetricsCalc::calcPrecision(TS* ts /*= nullptr*/)
{
    if (!_fullTs)
        throw LdopaException("Can't calc TS precision: no full TS (prefix tree) is presented.");

    setAndCheckTs(ts);

    DualTsSimulator sim;
    _calculatedPrecision = sim.calcPrecision(ts, _fullTs);

    return _calculatedPrecision;
}

//------------------------------------------------------------------------------

double TsMetricsCalc::calcGeneralization(TS* ts /*= nullptr*/)
{
    // TODO: возможно, проверку _log надо вынести в setAndCheckTs
    if (!_log)
        throw LdopaException("Can't calc TS precision: no event log is set.");
    setAndCheckTs(ts);

    double sum = 0;                             // сумма гармонического среднего

    // переберем все состояния _ts
    TS::StateIter tCur, tEnd;
    for (boost::tie(tCur, tEnd) = _ts->getStates(); tCur != tEnd; ++tCur)
    {
        int stInFlow;                           // выходной поток
        TS::State& st = *tCur;                  // текущее состояния
        
        // начальное состояние рассматриваем отдельно
        if (_ts->getInitState() == st)
            stInFlow = _log->getTracesNum();    // все трассы
        else                                    // входной поток реально
            stInFlow = getStateInFlow(st);      // считаем!

        // если входной поток для некоторого состояния нулевой, это странно...
        if (stInFlow != 0)
            sum += (1 / sqrt((double)stInFlow));
    }

    // нормализуем по числу состояний
    sum = sum / (double)(_ts->getStatesNum());
    _calculatedGeneralization = 1 - sum;

    return _calculatedGeneralization;
}

//------------------------------------------------------------------------------

int TsMetricsCalc::getStateInFlow(TS::State st) const
{
    int sum = 0;

    TS::ItransIter tCur, tEnd;
    for (boost::tie(tCur, tEnd) = _ts->getInTransitions(st); tCur != tEnd; ++tCur)
    {
        TS::IntRes trFreq = _ts->getTransFreq(*tCur);
        if (trFreq.second)                      // что делать в противном случае, не понятно
            sum += trFreq.first;
    }    
    
    return sum;
}

}}} // namespace xi { namespace ldopa { namespace ts {
