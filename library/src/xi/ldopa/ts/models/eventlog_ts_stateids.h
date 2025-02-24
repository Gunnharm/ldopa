////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.2.0
/// \date      11.10.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Declaration of some common types for state identifiers.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_MODELS_EVENTLOG_TS_STATEIDS_H_
#define XI_LDOPA_TRSS_MODELS_EVENTLOG_TS_STATEIDS_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/ts/models/eventlog_ts.h"
//#include "xi/ldopa/eventlog/sqlite/eventlog2.h"


// std
#include <vector>
#include <set>


namespace xi { namespace ldopa { namespace ts {;   //

//==============================================================================
// class AttrListStateId
//==============================================================================

/** \brief Declares a state ID type based on a list of attributes from EventLog(2) library.
 *
 *  #todo: специальное 0WS состояние
 */
class LDOPA_API AttrListStateId : public IStateId
{
public:
#pragma region Type Definitions

    /** \brief Declares Vector of attributes. */
    typedef std::vector<Attribute> VectorOfAttrs;

#pragma endregion // Type Definitions
public:
    //----<Constructors and destructor>----

    /** \brief Default constructor. */
    AttrListStateId() {}


    /** \brief Constructor with initializer list (Cx11). */
    AttrListStateId(const std::initializer_list<VectorOfAttrs::value_type>& v);

public:
    //----<Operators>----
    /** \brief Declares operation as it declares Compare concept.
     *
     *  For compare concept see http://en.cppreference.com/w/cpp/concept/Compare.
     *  lhv (given as this) is compared with rhv given as \a that
     */
    bool operator == (const AttrListStateId& that) const;

    /** \brief Non-equality operator (complimentary to ==). */
    bool operator != (const AttrListStateId& that) const { return !(*this == that); };

    /** \brief Return true if \a *this is equal to \a rhv */
    bool operator < (const AttrListStateId& that) const;

    /** \brief Syntax sugar for operator < */
    bool operator > (const AttrListStateId& that) const { return that < *this; };


public:
    //----<IStateId implementations>----
    virtual bool isEqualTo(const IStateId* that) const override;
    virtual bool isLessThan(const IStateId* that) const override;
    virtual std::string toString() const override;

public:
    /** \brief Return the underlied vector of attributes. */
    VectorOfAttrs& getAttrs() { return _attrs; }

    /** \brief Const overloaded version of getPtrs(). */
    const VectorOfAttrs& getAttrs() const { return _attrs; }

    /** \brief Append a given element \a el to the end of the attributes list. */
    void append(const Attribute& el) { _attrs.push_back(el); }


protected:

    /** \brief Stores list of attributes. */
    VectorOfAttrs _attrs;
}; // class AttrListStateId




/** \brief Pool of stateIDs that are based on a list (vector) of attributes.
 *
 *  The conceptual possibility of such an approach comes, as some of STL containers (set, map, list)
 *  are preserving their elements from being reconstructed when new elments are added to containers:
 *    - http://stackoverflow.com/questions/5182122/pointers-to-elements-of-stl-containers
 *    - http://stackoverflow.com/questions/20938441/implementing-a-string-pool-that-is-guaranteed-not-to-move
 *
 */
class LDOPA_API AttrListStateIDsPool : public IStateIDsPool
{
public:
    /** \brief Type for a set of StateIDs as they defined in the associated type. */
    typedef std::set<AttrListStateId> StateIDsSet;
    
    /** \brief Iterator type for StateIDsSet */
    typedef StateIDsSet::iterator StateIDsSetIter;
public:
    
    /** \brief Default constructor. */
    AttrListStateIDsPool();

    /** \brief Virtual destructor. */
    virtual ~AttrListStateIDsPool();
protected:
    AttrListStateIDsPool(const AttrListStateIDsPool&);                 // Prevent copy-construction
    AttrListStateIDsPool& operator=(const AttrListStateIDsPool&);      // Prevent assignment
public:
    //----<IStateIDsPool Implementation>----
    virtual const IStateId* getInitStateId() override;
    virtual size_t getSize() const override;
    //virtual const IStateId* operator[](const IStateId* stId);
public:
    //----<Main interface>----
    
    /** \brief Inserts a given StateID \a stId into a set and returns a const
     *  pointer to its internal (permanent) copy.
     */
    const AttrListStateId* operator[](const AttrListStateId& stId);
protected:

    /** \brief A set of the pool. */
    StateIDsSet* _poolSet;
}; // class AttrListStateIDsPool
 
}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_MODELS_EVENTLOG_TS_STATEIDS_H_
