////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Custom enumerators for testing enumerators.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      29.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XILIB_TEST_CORE_COLLECTIONS_CUSTOM_ENUMS_H_
#define XILIB_TEST_CORE_COLLECTIONS_CUSTOM_ENUMS_H_

#pragma once

// std
#include <string>
#include <vector>



#include "xi/collections/enumerators.hpp"




namespace xi { namespace collections { namespace tests {;

using xi::collections::IEnumerator;





/** \brief Represent a enumerator for vector of ints. Return value type is "by value", which 
 *  is determined by special partial redeclaration of a traits template in IEnumerator_traits<int>. 
 */
class IntVectorEnumerator : public IEnumerator<int> { //IEnumerator<int> {
public:
    typedef std::vector<int> IntVector;
    typedef IntVector::iterator IntVectorIterator;

public:
    IntVectorEnumerator(IntVector& vect)
        : _vect(vect)                       // bind reference
    {
        //reset();
        IntVectorEnumerator::reset();       // чтобы оптимизировать обращение к ТВФ
    }
public:
    //----<IEnumerator<> Implementations>----
    virtual bool hasNext() const override;
    //virtual int& getNext() override;
    virtual int getNext() override;        // чтобы избежать такой ^^^ фигни и есть переопр. классы свойств
    virtual void reset() override;


protected:
    
    /** \brief Vector, whose elements are enumerated. */
    IntVector& _vect;

    IntVectorIterator _cur;         ///< Current iterator.
    IntVectorIterator _end;         ///< Sentinel end iterator.

}; // // class IntVectorEnumerator



//==============================================================================

class VectorOfStringsEnumerator : public IEnumerator<std::string> {
public:
    typedef std::vector<std::string> StringVector;
    typedef StringVector::iterator StringVectorIterator;

public:
    VectorOfStringsEnumerator(StringVector& vect)
        : _vect(vect)                       // bind reference
    {
        VectorOfStringsEnumerator::reset();     // чтобы оптимизировать обращение к ТВФ
    }
public:
    //----<IEnumerator<> Implementations>----
    virtual bool hasNext() const override;
    virtual std::string& getNext() override;    // тут в отличие от int точно ссылка надо!
    virtual void reset() override;


protected:

    /** \brief Vector, whose elements are enumerated. */
    StringVector& _vect;

    StringVectorIterator _cur;         ///< Current iterator.
    StringVectorIterator _end;         ///< Sentinel end iterator.

}; // // class IntVectorEnumerator


} //  namespace tests {
} //  namespace collections {
} // namespace xi {



#endif // XILIB_TEST_CORE_COLLECTIONS_CUSTOM_ENUMS_H_
