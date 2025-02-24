////////////////////////////////////////////////////////////////////////////////
// Module Name:  sqlitelog.h/cpp
// Authors:      Sergey Shershakov
// Version:      0.1.0
// Date:         23.08.2017
// Copyright (c) xidv.ru 2014–2017.
//
// This source is for internal use only — Restricted Distribution.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////



// xilib
#include "custom_enums.h"

namespace xi {namespace collections {namespace tests {;


//==============================================================================
// class IntVectorEnumerator
//==============================================================================

bool IntVectorEnumerator::hasNext() const
{
    return _cur != _end;
}

//-----------------------------------------------------------------------------

int IntVectorEnumerator::getNext()
{
    if (_cur == _end)
        throw std::range_error("Enumeration has no more elements.");

    int& val = *_cur;
    ++_cur;

    return val;
}

//-----------------------------------------------------------------------------

void IntVectorEnumerator::reset()
{
    _cur = _vect.begin();
    _end = _vect.end();
}


//==============================================================================
// class VectorOfStringsEnumerator
//==============================================================================

bool VectorOfStringsEnumerator::hasNext() const
{
    return _cur != _end;
}

//-----------------------------------------------------------------------------

std::string& VectorOfStringsEnumerator::getNext()
{
    if (_cur == _end)
        throw std::range_error("Enumeration has no more elements.");

    std::string& val = *_cur;
    ++_cur;

    return val;
}

//-----------------------------------------------------------------------------

void VectorOfStringsEnumerator::reset()
{
    _cur = _vect.begin();
    _end = _vect.end();
}

//-----------------------------------------------------------------------------

} //  namespace tests {
} //  namespace collections {
} // namespace xi {


