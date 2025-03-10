////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Utilities for dealing with substrings.
/// \author    Sergey Shershakov
/// \version   0.3.1
/// \date      28.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#ifndef XILIB_XI_STRUTILS_SUBSTR_H_
#define XILIB_XI_STRUTILS_SUBSTR_H_

#pragma once


#include "xi/xilib_dll.h"

#include <vector>
#include <string>

// see http://stackoverflow.com/questions/25347174/doxygen-no-detailed-description-for-functions-in-namespaces


namespace xi {namespace strutils {; //

 
/** \brief Type for a pair of integral numerical indecies. Used for determining substring */
typedef std::pair<std::size_t, std::size_t> PairOfIndices;

/** \brief Type moniker for a vector of paris of indices. */
typedef std::vector<PairOfIndices> VectorOfPairOfIndices;

/** \brief Type moniker for vector of strings */
typedef std::vector<std::string> VectorOfStrings;


/** \brief Virtually splits a given str into sustring using a  sep as a one-char separator.
 *  \param str determine a string to be split
 *  \param sep represents a seprator character
 *  \param pairs passes a vector for placing pair of positions for substrings
 *  \param ignoreEmpty determines if only non empty substrings are added to a result vector (if true)
 *  \return total number of decided indices.
 *
 *  Function iteratively searches for all occurrence of given separator \a sep and logically
 *  split a given string to a substrings containing symbols from one separator to next one.
 *  Function puts each pair of decided indices to a given vector \a pairs.
 *  A pair of indices contains a position (bp) of the first character of a found substring
 *  and a position (ep) of the (possible non-exist) character next to the last character of the sustr.
 *  In this way, iterating all the characters of the substring can be made in a loop
 *  with condition <CODE>while(bp != ep) {... ++bp; }</CODE>. The similar approach is used with iterators concept.
 *  If \a ignoreEmpty is true, only non empty substrings are added to a result vector. Otherwise,
 *  a number of substring is equal to a number of delimiters plus 1.
 */
XILIB_API size_t splitStr(const std::string& str, std::string::value_type sep,
                VectorOfPairOfIndices& pairs, bool ignoreEmpty = false);


/** \brief Overloaded version of splitStr() splitting string \a str into vector of strings \a strs 
 *
 *  Functions appends extracted substrings to a given vector of strings \a strs
 */
XILIB_API size_t splitStr(const std::string& str, std::string::value_type sep,
    VectorOfStrings& strs, bool ignoreEmpty = false);


/** \brief Extracts a substring from a string str
 *  \param str a string
 *  \param ind
 *  \param dest
 *  \return dest.
 *
 *  Substring is determined by pair of iterator-like indices, ind, 
 *  and is appended to string dest.
 *  If the string doesn't contain a desirable substring, nothing is added to \a dest.
 */
XILIB_API inline std::string& appendSubstr(const std::string& str, PairOfIndices ind,
    std::string& dest)
{
    dest += str.substr(ind.first, ind.second - ind.first);
    return dest;
}


/** \brief Overloaded version of appendSubstr() with begin and end position */
XILIB_API inline std::string& appendSubstr(const std::string& str, size_t bp, size_t ep,
    std::string& dest)
{
    dest += str.substr(bp, ep - bp);
    return dest;
}


/** \brief Extracts a substring from a string str
 *  \param str
 *  \param ind
 *  \return dest
 *
 *  Substring is determined by pair of iterator-like indices, ind,
 *  and is returned .
 *  
 *  If the string doesn't contain a desirable substring, nothing is added to dest.
 */
XILIB_API inline std::string extractSubstr(const std::string& str, PairOfIndices ind)
{
    return str.substr(ind.first, ind.second - ind.first);
}


/** \brief Overloaded version of extractSubstr() with begin and end position */
XILIB_API inline std::string extractSubstr(const std::string& str, size_t bp, size_t ep)
{
    return str.substr(bp, ep - bp);
}


//------------------------------------------------------------------------------
}} // namespaces


#endif // XILIB_XI_STRUTILS_SUBSTR_H_
