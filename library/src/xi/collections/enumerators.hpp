////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Homebrew enumerators for common traversing tasks.
/// \author Sergey Shershakov
/// \version 0.3.1
/// \date 29.08.2017
///
/// Using for translating templates to virtual generics.
/// This module is moved here on from l-dopa library as having potentially
/// generic purpose.
///
/// \todo a Library part project for grouping collection files together ius needed.
/// \todo Comprehensive test set for the module is needed
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XILIB_XI_COLLECTIONS_ENUMERATORS_H_
#define XILIB_XI_COLLECTIONS_ENUMERATORS_H_


#include <stdexcept>        // range_error

//#pragma once
namespace xi { namespace collections {;


/** \brief Defines a template for enumerator traits classes. */
template<typename TElement>
class IEnumerator_traits {
public:
    typedef TElement&   TRet;               ///< Default return type: reference.

}; // class IEnumerator_traits


/** \brief Defines a template for enumerator traits classes. */
template<typename TElement>
class IConstEnumerator_traits {
public:
    typedef const TElement&   TRet;         ///< Default return type: const reference.
}; // class IConstEnumerator_traits


/** \brief Partial template specialization for int. */
template <>
class IEnumerator_traits<int> {
public:
    typedef int   TRet;               ///< Return type.
}; // class IEnumerator_traits<int>


/** \brief Partial template specialization for const int. */
template <>
class IConstEnumerator_traits<int> {
public:
    typedef int   TRet;               ///< Return type — simply int.
}; // class IConstEnumerator_traits<int>


/** \brief Declares interface for simple iterating over a generic collection.
 *
 *  Mix of
 *  https://msdn.microsoft.com/ru-ru/library/78dfe2yb(v=vs.110).aspx and
 *  https://docs.oracle.com/javase/7/docs/api/java/util/Enumeration.html.
 */
template<typename TElement, typename TTraits> // = IEnumerator_traits<TElement>>
class IBaseEnumerator {
public:
    //----<Types>----
    typedef typename TTraits::TRet  TRet;   ///< Return type.
public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr. */
    virtual ~IBaseEnumerator() {}
public:
    /** \brief Tests if this enumeration contains more elements.. */
    virtual bool hasNext() const = 0;

    /** \brief \returns the next element of this enumeration if this enumeration object 
     *  has at least one more element to provide. 
     *
     *  Raises std::range_error exception if a next element is request where call of
     *  IEnumerator::hasNext() method returns false.
     */
    virtual TRet getNext() = 0;

    /** \brief Sets the enumerator to its initial position, which is the first element 
     *  in the collection.
     *
     *  Not all collection implements this method. Some of then can throw exception when the
     *  method is invoked.
     */
    virtual void reset() = 0;
}; // class IEnumerator

// https://stackoverflow.com/questions/6907194/how-to-typedef-a-template-class
template<typename TElement>
using IEnumerator = IBaseEnumerator<TElement, IEnumerator_traits<TElement>>;

template<typename TElement>
using IConstEnumerator = IBaseEnumerator<TElement, IConstEnumerator_traits<TElement>>;




/** \brief Defines a template for implementation classes of enumerator interface for different
*  iterator types.
*/
template<   typename TIterator, 
            typename TEnumTraits, 
            typename TElement = typename TIterator::value_type
        >
class BaseEnumerator4Iterator : public IBaseEnumerator < TElement, TEnumTraits > {
public:
    //----<Types>----
    typedef typename TEnumTraits::TRet  TRet;       ///< Return type.
    //typedef typename IEnumerator_traits<TElement>::TRet  TRet;   ///< Return type.

public:
    BaseEnumerator4Iterator(TIterator beg, TIterator end)
    {
        _cur = beg;
        _beg = beg;
        _end = end;

    }
    virtual ~BaseEnumerator4Iterator() {}

public:
    virtual bool hasNext() const override
    {
        return _cur != _end;
    }

    virtual TRet getNext() override
    {
        if (_cur == _end)
            throw std::range_error("Enumeration has no more elements.");
        TRet value = *_cur;
        ++_cur;
        return value;
    }

    virtual void reset() override
    {
        _cur = _beg;
    }
protected:
    TIterator _cur;             ///< Current iterator.
    TIterator _end;             ///< Sentinel end iterator.
    TIterator _beg;             ///< Begin iterator for reset() feature.
}; // class BaseEnumerator4Iterator



template<typename TIterator, typename TElement = typename TIterator::value_type>
using Enumerator4Iterator = BaseEnumerator4Iterator<TIterator, IEnumerator_traits<TElement>, TElement>;


template<typename TIterator, typename TElement = typename TIterator::value_type>
using Enumerator4ConstIterator = 
    BaseEnumerator4Iterator<TIterator, IConstEnumerator_traits<TElement>, TElement>;



}} // namespace xi { namespace collections

#endif // XILIB_XI_COLLECTIONS_ENUMERATORS_H_
