// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/models/eventlog_ts_stateids.h"




namespace xi { namespace ldopa { namespace ts { ;   //

//==============================================================================
// class AttrListStateId
//==============================================================================



bool AttrListStateId::operator==(const AttrListStateId& that) const
{
    if (_attrs.size() != that._attrs.size())
        return false;

    for (size_t i = 0; i < _attrs.size(); ++i)
        if (_attrs[i] != that._attrs[i])
            return false;

    return true;   // ok, as it reaches this point
}

//------------------------------------------------------------------------------

AttrListStateId::AttrListStateId(const std::initializer_list<VectorOfAttrs::value_type>& v)
{
    for (auto itm : v)
        _attrs.push_back(itm);
}

//------------------------------------------------------------------------------

bool AttrListStateId::operator<(const AttrListStateId& that) const
{
    // comparing id1 and id2, by our definition, if a number of
    // elements in id1 is lesser that in id2, then id1 < id2
    // if they contains equal number of elements, there is a need
    // to compare all elements (pointers)

    if (_attrs.size() < that._attrs.size())
        return true;

    if (_attrs.size() > that._attrs.size())
        return false;


    // well, the numbers of elements are equal, so one need to compare all elements
    for (size_t i = 0; i < _attrs.size(); ++i)
    {
        if (_attrs[i] < that._attrs[i])
            return true;
        else if (_attrs[i] > that._attrs[i])
            return false;

        // if both elements are equal, one nothing to say whose is lesser
    }

    // finally, if we left the loop, it means both id are equal
    // by definition *this is not lesser than that, so...

    return false;
}

//------------------------------------------------------------------------------

bool AttrListStateId::isEqualTo(const IStateId* that) const
{
    // прикастовываем указатель к нашему типу, если не получается — не равны
    const AttrListStateId* typedRhv = dynamic_cast<const AttrListStateId*>(that);
    if (!typedRhv)
        return false;

    return *this == *typedRhv;
}

//------------------------------------------------------------------------------

bool AttrListStateId::isLessThan(const IStateId* that) const
{
    const AttrListStateId* typedRhv = dynamic_cast<const AttrListStateId*>(that);
    return *this < *typedRhv;
}

//------------------------------------------------------------------------------

std::string AttrListStateId::toString() const
{

    std::string lbl = "[";
    bool first = true;

    //const TsPtrStateId& stId = getStateId(st);
    for (auto it = _attrs.begin(); it != _attrs.end(); ++it)
    {
        if (!first)
            lbl += ", ";
        else
            first = false;

        //const std::string* st = (const std::string*)*it;
        //lbl += (*st);
        lbl += it->toString();
    }

    lbl += "]";

    return lbl;
}


//==============================================================================
// class AttrListStateIDsPool
//==============================================================================



//------------------------------------------------------------------------------

AttrListStateIDsPool::AttrListStateIDsPool()
{
    _poolSet = new StateIDsSet();
}

//------------------------------------------------------------------------------

AttrListStateIDsPool::~AttrListStateIDsPool()
{
    delete _poolSet;
}

//------------------------------------------------------------------------------

const IStateId* AttrListStateIDsPool::getInitStateId()
{
    return (*this)[AttrListStateId()];
    //return insert(AttrListStateId());
}

//------------------------------------------------------------------------------

size_t AttrListStateIDsPool::getSize() const
{
    return _poolSet->size();
}

const AttrListStateId* AttrListStateIDsPool::operator[](const AttrListStateId& stId)
{
    std::pair<StateIDsSetIter, bool> res = _poolSet->insert(stId);

    // address of an str object that is got through dereference of iter
    return &(*(res.first));     
}





}}} // namespace xi { namespace ldopa { namespace ts {
