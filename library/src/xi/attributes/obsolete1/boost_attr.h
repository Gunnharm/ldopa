////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Boost lib based attributes system.
/// \author Sergey Shershakov
/// \version 0.1.0
/// \date 04.02.2015
///
/// Types for attributing components with use boost library for maintaining 
/// variant types.
/// Main Boost exported type for maintaining variant is boost::variant
/// 
/// Устаревшие определения, которые со временем отвалятся
///
////////////////////////////////////////////////////////////////////////////////

#pragma     

#ifndef XILIB_XI_ATTRIBUTES_OBSOLETE1_BOOST_ATTR_H_          // boostAttrH
#define XILIB_XI_ATTRIBUTES_OBSOLETE1_BOOST_ATTR_H_


#include "xi/xilib_dll.h"

//STD
#include <bitset>
#include <list>
#include <map>
#include <memory>   // shared_ptr

// boost library
#include <boost/variant.hpp>            // boost::variant
#include <boost/shared_array.hpp>       // boost::shared_array
//#include <boost/variant.hpp>

#include "xi/xilib_dll.h"               // все-таки в каком пакете он должен лежать?
#include "xi/attributes/boost_attr.h"   // обновленное
#include "xi/attributes/destructable_object.h"
#include "xi/types/aliases.h"


namespace xi {

/** \brief Namespace contains types for boost-based attributing. */
namespace attributes {



/** \brief Type definintion for 32-bitset. Used as a variant for BoostAttribute */
typedef std::bitset<32> Bitset32;

typedef std::bitset<64> Bitset64;

typedef std::bitset<96> Bitset96;

typedef std::bitset<128> Bitset128;



/** \brief Type definition for a shared pointer to a c-string. */
// UPD: это некорректно, т.к. память из-под него освобождается delete ptr,
// а не delete [] ptr!
typedef std::shared_ptr<char> CStrSharedPtr;  // тестируем
// TODO: убрать это ^^^^^ вообще




/** \brief Type definintion for boost:variant instantiation declaring
 *  datatypes that can be used as variant values.
 *
 *  >
 */
typedef boost::variant< // numbers
    char, 
    unsigned char,
    int,
    unsigned int,
    double,

    // bitsets
    Bitset32,
    Bitset64,
    Bitset128,

    // pointers
    void*,                                      // memory-unmanaged object pointer
    DestrObjSharedPtr,                          // memory-managed object pointer
    //std::shared_ptr<IAutoDeleteAttrPtr>,

    // shared pointers to some std objects
    StringSharedPtr,

    // std::
    std::string                                 // sizeof(std::string) = 32, adding it here makes sizeof(VarAttribute) = 40 instead od 16

                      > BaseVarAttribute;


/** \brief Main class for out VarAttribute containing some helper methods
 *  to access to an object given in form of...
 *
 *  TODO: create an enum for all var types...
 *  TODO: creates constructors, operators= and so on...
 */
class VarAttribute : public BaseVarAttribute {
    // TODO:
public:
    enum AttributeType { 
        atChar,
        atUchar,
        atInt,
        atUint,
        atDouble,
        atBitset32,
        atBitset64,
        atBitset128,
        atVoidp,
        atDestrObjSharedPtr,
        atStringSharedPtr,
        atStdString,              
    }; //
public:
    //---------<Constructors>-----------
    
    /** \brief Default constructor */
    VarAttribute() {} //               : BaseVarAttribute(val) {  } ;

    VarAttribute(const char val)                : BaseVarAttribute(val) {  } ;
    VarAttribute(const unsigned char val)       : BaseVarAttribute(val) {  } ;
    
    /** \brief Initializing with Integer */
    VarAttribute(const int val)                 : BaseVarAttribute(val) {  } ;
    VarAttribute(const unsigned int val)        : BaseVarAttribute(val) {  } ;
    VarAttribute(const double val)              : BaseVarAttribute(val) {  } ;
    

    // TODO: other bitsets
    VarAttribute(const Bitset128& val)          : BaseVarAttribute(val) {  } ;

    VarAttribute(void*  val)               : BaseVarAttribute(val) {  } ;  // тут с const странности..
    VarAttribute(const DestrObjSharedPtr& val)  : BaseVarAttribute(val) {  } ;
    VarAttribute(const StringSharedPtr& val)    : BaseVarAttribute(val) {  } ;
    VarAttribute(const std::string& val)        : BaseVarAttribute(val) {  } ;

    /** \brief copy constructor */
    VarAttribute(const VarAttribute& rhv) : BaseVarAttribute(rhv.getAsBaseRef()) {  }     
public:
    //--------<Operators>------------

    /** \brief copy (assigning) operator  */
    VarAttribute operator=(const VarAttribute& rhv) { getAsBaseRef() = rhv.getAsBaseRef(); return *this; }

    inline bool operator==(const VarAttribute& rhv) const { return (getAsBaseRef() == rhv.getAsBaseRef()); }



public:
    //-----------<VarAttribute some info>----------
    
    /** \brief Returns a type of attribute data */
    AttributeType getType() const { return (AttributeType)(which()); } // {return atUnknown; } ;


public:
    //----------<sets/gets>-------------

    /** \brief Temporary helper method giving access to the object throught 
     *  a reference to a base class
     */
    BaseVarAttribute& getAsBaseRef() { return *this;  }
    const BaseVarAttribute& getAsBaseRef() const { return *this;  }

    BaseVarAttribute* getAsBasePtr() { return this;  }
    const BaseVarAttribute* getAsBasePtr() const { return this;  }

}; // VarAttribute




/** \brief Typedef for std::list of VarAttribute
 *
 *  
 */
typedef std::list<VarAttribute> BoostAttributeList;

/** \brief Special sort of list that can be deleted throug polymorphic pointer */
//class ListDestrObj : public std::list<VarAttribute>, public IDestructableObject {
class BoostAttributeListDO : public std::list<VarAttribute>, public IDestructableObject {
}; // class ListDestrObj


/** \brief Typedef for std::map of string → VarAttribute
 *
 *  It is used in conjunction with IBoostAttributable 
 *  ! The question is if instead of simple std::map there should be used MapDestrObj here.
 */
typedef std::map<std::string, VarAttribute> BoostAttributeMap;


/** \brief Special sort of map that can be deleted throug polymorphic pointer */
//class MapDestrObj : public std::map<std::string, VarAttribute> , public IDestructableObject {
class BoostAttributeMapDO : public BoostAttributeMap , public IDestructableObject {
}; // class MapDestrObj


/** \brief Interface declares methods for any types that can contain boost attributes.
 */
class IBoostAttributable {
public:

    virtual const BoostAttributeMap& getAttributesMap() const = 0;
    virtual BoostAttributeMap& getAttributesMap()  = 0;

protected:
    /** \brief protected Destructor: It is not intended to delete an object through this interface!! */
    ~IBoostAttributable() {} ;
}; // class IBoostAttributable



}; //  namespace attributes
}; // namespace xi



#endif // XILIB_XI_ATTRIBUTES_OBSOLETE1_BOOST_ATTR_H_
