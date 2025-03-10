////////////////////////////////////////////////////////////////////////////////
// Module Name:  substr.h/cpp
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
#include "xi/strutils/substr.h"


namespace xi {namespace strutils {; //

//------------------------------------------------------------------------------

size_t splitStr(const std::string& str, std::string::value_type sep, 
    VectorOfPairOfIndices& pairs, bool ignoreEmpty)
{   
    size_t strSize = str.size();

    if (strSize == 0)
        return 0;

    size_t bp = 0;                  // begin pos
    size_t ep = 0;                  // end pos
    size_t num = 0;                 // number of occurences

    do 
    {
        ep = str.find(sep, bp);

        // correction for a last substring
        if (ep == std::string::npos)
            ep = strSize;

        if (!(ignoreEmpty && bp == ep))
        {
            pairs.push_back(PairOfIndices(bp, ep));
            ++num;
        }
                
        bp = ep + 1;
    } while (bp <= strSize);    // so delicate point here with <=
    
    return num;
}

//------------------------------------------------------------------------------

size_t splitStr(const std::string& str, std::string::value_type sep,
    VectorOfStrings& strs, bool ignoreEmpty)
{
    // due to performance reasons, we copy the algorithm above
    size_t strSize = str.size();

    if (strSize == 0)
        return 0;

    size_t bp = 0;                  // begin pos
    size_t ep = 0;                  // end pos
    size_t num = 0;                 // number of occurences

    do
    {
        ep = str.find(sep, bp);

        // correction for a last substring
        if (ep == std::string::npos)
            ep = strSize;

        if (!(ignoreEmpty && bp == ep))
        {
            //pairs.push_back(PairOfIndices(bp, ep));
            strs.push_back(extractSubstr(str, bp, ep));
            ++num;
        }

        bp = ep + 1;
    } while (bp <= strSize);    // so delicate point here with <=

    return num;
}


//------------------------------------------------------------------------------
}} // namespaces


