////////////////////////////////////////////////////////////////////////////////
// Module Name:  destructable_object.h/cpp
// Authors:      Sergey Shershakov
// Version:      0.1.0
// Date:         06.09.2017
// Copyright (c) xidv.ru 2014–2017.
//
// This source is for internal use only — Restricted Distribution.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////


// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "destructable_object.h"
#include <cstring> // memcpy


namespace xi {
namespace attributes {


//==============================================================================
// class IDestructableObject
//==============================================================================


DestrByteArray::DestrByteArray(const Byte* arr, TSize size)
    : _size(size)
{
    _arr = new Byte[size];
    memcpy(_arr, arr, _size);
}

//-----------------------------------------------------------------------------

void DestrByteArray::clear()
{
    if (_arr)
    {
        delete[] _arr;
        _arr = nullptr;
        _size = 0;
    }
}

//-----------------------------------------------------------------------------

void DestrByteArray::replace(Byte* newArr, TSize newSize)
{
    clear();
    _arr = newArr;
    _size = newSize;
}

//-----------------------------------------------------------------------------

//DestrByteArray::Byte* DestrByteArray::exchange(Byte* newArr, int newSize)
DestrByteArray::APair DestrByteArray::exchange(DestrByteArray::Byte* newArr, 
    TSize newSize)
{
    APair old = std::make_pair(_arr, _size);
    _arr = newArr;
    _size = newSize;

    return old;
}

//-----------------------------------------------------------------------------



}; //  namespace attributes
}; // namespace xi


