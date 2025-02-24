// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/models/eventlog_ts.h"

namespace xi { namespace ldopa { namespace ts { ;   //



//------------------------------------------------------------------------------

BaseEventLogTS::BaseEventLogTS(IStateIDsPool* stIDsPool) //, bool copyPool)
    : _stIDsPool(stIDsPool)
{
    _ts = new LogTS();
    
    // начальное состояние берем из пула — обязан предоставить!
    _initStId = _stIDsPool->getInitStateId();    
    _ts->getOrAddState(_initStId);

    _tracesNum = 0;         // нет пока трасс
}

//------------------------------------------------------------------------------

BaseEventLogTS::BaseEventLogTS(const BaseEventLogTS& that)
{
    _ts = new LogTS(*that._ts);         // здесь работает КК для подлежащей TS

    _stIDsPool = that._stIDsPool;       // просто копируем указатели для этих двух
    _initStId = that._initStId;

    // доп. атрибуты
    _tracesNum = that._tracesNum;       // число образующих трасс
}

//------------------------------------------------------------------------------

BaseEventLogTS::~BaseEventLogTS()
{
    delete _ts;
}


//------------------------------------------------------------------------------

void BaseEventLogTS::swap(BaseEventLogTS& lhv, BaseEventLogTS& rhv)
{
    std::swap(lhv._initStId, rhv._initStId);
    std::swap(lhv._ts, rhv._ts);
    std::swap(lhv._stIDsPool, rhv._stIDsPool);

}

//------------------------------------------------------------------------------

BaseEventLogTS& BaseEventLogTS::operator=(const BaseEventLogTS& that)
{
    // implements copy-and-swap idiom
    BaseEventLogTS tmp(that);
    swap(tmp, *this);

    return *this;
}


}}} // namespace xi { namespace ldopa { namespace ts {
