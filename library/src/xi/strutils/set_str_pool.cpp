////////////////////////////////////////////////////////////////////////////////
// Module Name:  set_str_pool.h/cpp
// Authors:      Sergey Shershakov
// Version:      0.3.1
// Date:         28.08.2017
// Copyright (c) xidv.ru 2014–2017.
//
// This source is for internal use only — Restricted Distribution.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////

// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"
#include "xi/strutils/set_str_pool.h"

namespace xi { namespace strutils {; //

//------------------------------------------------------------------------------

//
//const SetStrPool::String* SetStrPool::insert(const std::string& str)
const std::string* SetStrPool::insert(const std::string& str)
{
    std::pair<StrsSetIter, bool> res = _pool.insert(str);
    
    return &(*(res.first));     // address of an str object that is got through dereference of iter
}


//------------------------------------------------------------------------------

void SetStrPool::clear()
{
    _pool.clear();
}


//------------------------------------------------------------------------------

const std::string* SetStrPool::operator[](const std::string& str)
{
    std::pair<StrsSetIter, bool> res = _pool.insert(str);

    return &(*(res.first));     // address of an str object that is got through dereference of iter

}


}
} // namespaces

