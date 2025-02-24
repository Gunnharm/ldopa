// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/models/evlog_ts_red.h"



namespace xi { namespace ldopa { namespace ts { ;   //


//==============================================================================
// class EvLogTSWithFreqs
//==============================================================================


EvLogTSWithFreqs::EvLogTSWithFreqs(IStateIDsPool* stIDsPool)
    : Base(stIDsPool)
    , _maxWS(0)
{
}


//------------------------------------------------------------------------------

EvLogTSWithFreqs::EvLogTSWithFreqs(const EvLogTSWithFreqs& that)
    : BaseEventLogTS(that)
{
    validateStates(that);
    validateTransitions(that);

    // доп. атрибуты
    _maxWS = that._maxWS;       // макс. размер окна

}

//------------------------------------------------------------------------------

void EvLogTSWithFreqs::validateStates(const EvLogTSWithFreqs& that)
{
    LogTS::StateIter stOrig, stOrigEnd, stCur, stCurEnd;
    boost::tie(stOrig, stOrigEnd) = that.getStates();
    boost::tie(stCur, stCurEnd) = getStates();

    // итерируем по вершинам исходного графа
    for (; stOrig != stOrigEnd; ++stOrig, ++stCur)
    {        
        auto it = that._stateFlags.find(*stOrig);
        if (it != that._stateFlags.end())                // если ключ для исходного вертекса
            _stateFlags[*stCur] = it->second;

    }
}

//------------------------------------------------------------------------------

void EvLogTSWithFreqs::validateTransitions(const EvLogTSWithFreqs& that)
{
    LogTS::TransIter trOrig, trOrigEnd, trCur, trCurEnd;
    boost::tie(trOrig, trOrigEnd) = that.getTransitions();
    boost::tie(trCur, trCurEnd) = getTransitions();

    // итерируем по дугам исходного графа
    for (; trOrig != trOrigEnd; ++trOrig, ++trCur)
    {
        auto it = that._transFreqsMap.find(*trOrig);
        if (it != that._transFreqsMap.end())                // если ключ для исходного вертекса
            _transFreqsMap[*trCur] = it->second;
    }
}


//------------------------------------------------------------------------------

EvLogTSWithFreqs::~EvLogTSWithFreqs()
{
}

//------------------------------------------------------------------------------

EvLogTSWithFreqs::Transition EvLogTSWithFreqs::getOrAddTransF(State s, State t, 
    const Attribute& lbl, int addFreq)
{
    Transition tr = Base::getOrAddTrans(s, t, lbl);

    IntRes fr = getTransFreq(tr);
    if (!fr.second)                         // частота ранее не устанавливалась
        fr.first = addFreq;
    else
        fr.first += addFreq;
    
    setTransFreq(tr, fr.first);             // задаем новую частоту для этой транзиции

    return tr;
}

//------------------------------------------------------------------------------

EvLogTSWithFreqs::IntRes EvLogTSWithFreqs::getTransFreq(Transition t) const
{
    TransIntMap::const_iterator tf = _transFreqsMap.find(t);
    if (tf == _transFreqsMap.end())
        return std::make_pair(int(), false);   

    return std::make_pair((*tf).second, true);
}

//------------------------------------------------------------------------------

void EvLogTSWithFreqs::setTransFreq(Transition t, int freq)
{
    _transFreqsMap[t] = freq;
}


//------------------------------------------------------------------------------

EvLogTSWithFreqs::StateFlagsRes EvLogTSWithFreqs::getStateFlags(State s) const
{
    StateFlagsMap::const_iterator sfi = _stateFlags.find(s);
    if (sfi == _stateFlags.end())
        return std::make_pair(StateFlags(), false);

    return std::make_pair((*sfi).second, true);
}

//------------------------------------------------------------------------------

void EvLogTSWithFreqs::setStateFlags(State s, StateFlags sf)
{
    _stateFlags[s] = sf;
}

//------------------------------------------------------------------------------

bool EvLogTSWithFreqs::isStateAccepting(State s) const
{
    StateFlagsRes sfr = getStateFlags(s);
    if (!sfr.second)                        // если вообще никакие флаги не установлены
        return false;

    return sfr.first.getAccepting() == StateFlags::triTrue;
}

//------------------------------------------------------------------------------

void EvLogTSWithFreqs::setAcceptingState(State s, bool accepting)
{
    StateFlagsRes sfr = getStateFlags(s);
    sfr.first.setAccepting(accepting);
    setStateFlags(s, sfr.first);
}

//------------------------------------------------------------------------------

}}} // namespace xi { namespace ldopa { namespace ts {
