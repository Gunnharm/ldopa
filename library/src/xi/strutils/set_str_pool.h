////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Represents a string pool class based on set of strings.
/// \author    Sergey Shershakov
/// \version   0.3.1
/// \date      28.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// ex. setStrPool.h
///
/// \TODO Имеет смысл (?) сделать отдельный шаблонный класс — пул объектов — 
/// и конкретизировать его для строк?
///
////////////////////////////////////////////////////////////////////////////////


#ifndef XILIB_XI_STRUTILS_SET_STR_POOL_H_
#define XILIB_XI_STRUTILS_SET_STR_POOL_H_


#include "xi/xilib_dll.h"

#include <set>
#include <string>

namespace xi {namespace strutils {; //


// экспорт конкретизированных шаблонов в DLL
// это должно быть ДО использования в нижележащих классах
#ifdef XILIB_DLL
XILIB_EXPIMP_TEMPLATE template class XILIB_API std::set<std::string>;
#endif // XILIB_DLL


/** \brief Represents a pool of unique <CODE>std::string</CODE>'s.
 *
 *  There is a need in a such of string pool where a cost of storing strings is too expensive
 *  and it is preferrable to have every string value as a one instance and get access to it
 *  through brif poiner.
 *  The conceptual possibility of such an approach comes, as some of STL containers (set, map, list)
 *  are preserving their elements from being reconstructed when new elments are added to containers:
 *    - http://stackoverflow.com/questions/5182122/pointers-to-elements-of-stl-containers
 *    - http://stackoverflow.com/questions/20938441/implementing-a-string-pool-that-is-guaranteed-not-to-move
 */
class XILIB_API SetStrPool {
public:
    //-----<Types>-----
    typedef std::set<std::string> StrsSet;
    
    /** \brief Iterator type for StrsSet */
    typedef StrsSet::iterator StrsSetIter;
public:

    /** \brief Inserts a given string \a str into a set and returns a const 
     *  pointer to its internal (permanent) copy.
     */
    const std::string* insert(const std::string& str);

    /** \brief Clears the pool. 
     *
     *  This will invalidate all externally keeped pointers as string object 
     *  will no more exists.
     */
    void clear();

    /** \brief Operator acts in the same way as insert() method */
    const std::string* operator[](const std::string& str);

    /** \brief Returns an underlying pool set */
    StrsSet& getPool() { return _pool;  }

    /** \brief Overloaded const-version of getPool() */
    const StrsSet& getPool() const { return _pool; }

protected:

    /** \brief A set object storing a unique strings. */
    StrsSet _pool;
}; // class SetStrPool


//------------------------------------------------------------------------------
}} // namespaces


#endif // XILIB_XI_STRUTILS_SET_STR_POOL_H_
