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


#ifndef XILIB_XI_COLLECTIONS_ENUMERATORS_OBSOLETE1_H_
#define XILIB_XI_COLLECTIONS_ENUMERATORS_OBSOLETE1_H_


//#pragma once
namespace xi { namespace collections {;



/** \brief Declares interface for simple enumerating all elements of some sequence.
 *
 *  -
 */
template<typename TElement>
class IEnumerator_obsolete1 {
public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr */
    virtual ~IEnumerator_obsolete1() {}

public:
    /** \brief Determines if there is current element (true) or not (false). */
    virtual bool hasNext() const = 0;

    /** \brief \returns current active element if exists, otherwise throws an exception. */
    virtual TElement* getNext() = 0;
}; // class IEnumerator


/** \brief Declares interface for simple enumerating all elements of some sequence.
 *
 *  Const version.
 */
template<typename TElement>
class IConstEnumerator_obsolete1 {
public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr */
    virtual ~IConstEnumerator_obsolete1() {}

public:
    /** \brief Determines if there current elemnt (true) or not (false) */
    virtual bool hasNext() const = 0;

    /** \brief \returns current active element if exists, otherwise throws an exception */
    virtual const TElement* getNext()  = 0;
}; // class IConstEnumerator




/** \brief Defines a template for trait classes based on different semantics of
 *  iterators of different collections.
 *
 *  -
*/
template< typename TIterator >
struct Enumerator4Iterator_traits_obsolete1 {

}; // template< typename TIterator > struct Enumerator4Iterator_traits


/** \brief Defines a template for implementation classes of enumerator interface for different
 *  iterator types.
 *
 *  TOTHINKOF: actually, TElement can be deducted from TIterator as ::value_type
 */
template<typename TElement, typename TIterator>
class Enumerator4Iterator_obsolete1 : public IEnumerator_obsolete1 < TElement > {
public:
    Enumerator4Iterator_obsolete1(TIterator beg, TIterator end)
    {
        _cur = beg;
        _end = end;

    }
    virtual ~Enumerator4Iterator_obsolete1() {};
public:
    virtual bool hasNext() const override
    {
        return _cur != _end;
    }

    virtual TElement* getNext() override
    {
        TElement* value = _enum4it_trais.getValue(_cur);
        ++_cur;
        return value;
        //return _storageIt_trais.getNext(_cur);
    }
protected:

    Enumerator4Iterator_traits_obsolete1 <TIterator> _enum4it_trais;
    TIterator _cur;
    TIterator _end;
}; // class Enumerator4Iterator


/** \brief Defines a templae for implementation classes of enumerator interface for different
*  iterator types
*
*  TOTHINKOF: actually, TElement can be deducted from TIterator as ::value_type
*/
template<typename TElement, typename TIterator>
class ConstEnumerator4Iterator_obsolete1 : public IConstEnumerator_obsolete1 < TElement > {
public:
    ConstEnumerator4Iterator_obsolete1(TIterator beg, TIterator end)
    {
        _cur = beg;
        _end = end;
    }
    virtual ~ConstEnumerator4Iterator_obsolete1() {};
public:
    virtual bool hasNext() const override
    {
        return _cur != _end;
    }

    virtual const TElement* getNext() override
    {
        const TElement* value = _enum4it_trais.getValue(_cur);
        ++_cur;
        return value;
    }
protected:

    Enumerator4Iterator_traits_obsolete1 <TIterator> _enum4it_trais;
    TIterator _cur;
    TIterator _end;
}; // class ConstEnumerator4Iterator


}} // namespace xi { namespace collections

#endif // XILIB_XI_COLLECTIONS_ENUMERATORS_OBSOLETE1_H_
