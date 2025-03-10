////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Defines a type for a pointer to an object that should be destructed
///            through invocation of its virtual destructor.
/// \author    Sergey Shershakov
/// \version   0.3.1
/// \date      28.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Also some helper types are defined here.
///
////////////////////////////////////////////////////////////////////////////////


// #pragma

#ifndef XILIB_XI_ATTRIBUTES_DESTRUCTABLE_OBJECT_H_          
#define XILIB_XI_ATTRIBUTES_DESTRUCTABLE_OBJECT_H_

#include "xi/xilib_dll.h"

#include <utility>


namespace xi {
namespace attributes {


/** \brief Interface declaring only virtual destructor.
 *
 *  Any object implemented this interface has to define a destructor. So,
 *  such an object can be deleted through the pointer of this type.
 *  Unlike the void* the IDestructableObject* can be used where auto-deletion
 *  feature is required.
 *  
 *  The interface may be considered os a subtle example of a classic Object type
 *  introducing a Superclass, the single root of all creatable and deletable 
 *  datatypes.
 */
class XILIB_API IDestructableObject {
public:
    //class IDestructableObject();
    /** \brief Virtual destructor - is a main feature of this interface */
    virtual ~IDestructableObject() {}
}; // class IAutoDeleteAttrPtr


/** \brief Defines destructable byte array. */
class XILIB_API DestrByteArray : public IDestructableObject {
public:
    // types
    typedef unsigned char Byte;
    typedef std::size_t TSize;
    typedef std::pair<Byte*, TSize> APair;            // pair of "array-size"
    typedef std::pair<const Byte*, TSize> CAPair;     // const version
protected:
    Byte* _arr;                             // определяем прямо сейчас, дабы избежать вынесение в cpp
    TSize _size;          
public:
    // constructors
    DestrByteArray() : _arr(nullptr), _size(0) {}
    DestrByteArray(Byte* arr, TSize size) : _arr(arr), _size(size) {}
    DestrByteArray(APair ap) : DestrByteArray(ap.first, ap.second) {}

    // following costructors copy the given array!
    DestrByteArray(const Byte* arr, TSize size);
    DestrByteArray(CAPair ap) : DestrByteArray(ap.first, ap.second) {}

    // virtual destructor!
    virtual ~DestrByteArray() { delete [] _arr; }

protected:
    // TODO: пока что их закрываем, но вообще, надо реализовать
    DestrByteArray(const DestrByteArray&);                    // Prevent copy-construction
    DestrByteArray& operator=(const DestrByteArray&);         // Prevent assignment

public:

    Byte* getArr() { return _arr;  }
    const Byte* getArr() const { return _arr; }
    TSize getSize() const { return _size; }

    /** \brief Clears the array and frees the memory. */
    void clear();

    /** \brief Replaces the content to the given array and deletes old ones. */
    void replace(Byte* newArr, TSize newSize);

    void replace(APair ap) { replace(ap.first, ap.second); }
    

    /** \brief Exchenge the content with the given array and returns old ones. */
    DestrByteArray::APair exchange(Byte* newArr, TSize newSize);
    //std::pair<Byte*, int> exchange(Byte* newArr, int newSize);
    //Byte* exchange(Byte* newArr, int newSize);

    DestrByteArray::APair exchange(APair ap)
    {
        return exchange(ap.first, ap.second);
    }   
}; // class XILIB_EXPIMP DestrByteArray




/** \brief Template class for an object managed through a pointer to a base
 *  interface IDestructableObject (being deleted) and carrying simple local
 *  (in stack) object _obj of type T
 */
template<class T>
//class XILIB_API LocalDestrObjectWrapper :
class LocalDestrObjectWrapper :
    public IDestructableObject {

public:
    // test constructor
    LocalDestrObjectWrapper()
    {
        //std::cout << "\nADAWrapper Created\n";
    }

    // test constructor
    LocalDestrObjectWrapper(const T& t)
        : _obj(t)
    {
        //std::cout << "\nADAWrapper Created and initialized\n";
    }


    /** \brief Virtual destructor  */
    virtual ~LocalDestrObjectWrapper()
    {
        //std::cout << "\nADAWrapper Destroyed\n";
    }

public:
    //------------<Sets/Gets>----------------------------------
    const T&    getObj() const          { return _obj; }
    T&          getObj()                { return _obj; }
    void        setObj(const T& par)    { _obj = par; }

protected:
    T _obj;
}; // class LocalDestrObjectWrapper




}; //  namespace attributes
}; // namespace xi


#endif // XILIB_XI_ATTRIBUTES_BOOST_BOOSTATTR_H_
