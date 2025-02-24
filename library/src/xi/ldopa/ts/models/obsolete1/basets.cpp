// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/models/obsolete1/basets.h"

namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //

BaseTs::BaseTs()
{
}

BaseTs::~BaseTs()
{
}

//==============================================================================
// class TsPtrStateId
//==============================================================================


TsPtrStateId::TsPtrStateId(const std::initializer_list<VectorOfPtrs::value_type>& v)
{
    for (auto itm : v) 
        _ptrs.push_back(itm);
}

//------------------------------------------------------------------------------

void TsPtrStateId::append(const void* el)
{
    _ptrs.push_back(el);
}

//------------------------------------------------------------------------------

bool TsPtrStateId::operator<(const TsPtrStateId& that) const
{
    // comparing id1 and id2, by our definition, if a number of
    // elements in id1 is lesser that in id2, then id1 < id2
    // if they contains equal number of elements, there is a need
    // to compare all elements (pointers)

    if (_ptrs.size() < that._ptrs.size())
        return true;

    if (_ptrs.size() > that._ptrs.size())
        return false;


    // well, the numbers of elements are equal, so one need to compare all elements
    for (size_t i = 0; i < _ptrs.size(); ++i)
    {
        if (_ptrs[i] < that._ptrs[i])
            return true;
        else if (_ptrs[i] > that._ptrs[i])
            return false;

        // if both elements are equal, one nothing to say whose is lesser
    }

    // finally, if we left the loop, it means both id are equal
    // by definition *this is not lesser than that, so...

    return false;
}

//------------------------------------------------------------------------------

bool TsPtrStateId::operator==(const TsPtrStateId& that) const
{
    if (_ptrs.size() != that._ptrs.size())
        return false;

    for (size_t i = 0; i < _ptrs.size(); ++i)
        if (_ptrs[i] != that._ptrs[i])
            return false;


    return true;   // ok, as it reaches this point
}

//------------------------------------------------------------------------------

bool TsPtrStateId::isEqualTo(const ITsStateId* that) const
{
    // it can be an exception here
    const TsPtrStateId* typedRhv = dynamic_cast<const TsPtrStateId*>(that);
    return *this == *typedRhv;
    
}

//------------------------------------------------------------------------------

bool TsPtrStateId::isLessThan(const ITsStateId* that) const
{
    const TsPtrStateId* typedRhv = dynamic_cast<const TsPtrStateId*>(that);
    return *this < *typedRhv;
}

}}}} // namespace xi::ldopa::ts::obsolete1
