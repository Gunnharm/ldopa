//
#ifndef XI_LDOPA_TRSS_MODELS_OBSOLETE1_BASETS_H_
#define XI_LDOPA_TRSS_MODELS_OBSOLETE1_BASETS_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"
#include "xi/ldopa/graphs/boost/bgl_graph_adapters.h"


namespace xi { namespace ldopa { namespace ts { namespace obsolete1 {;   //


// экспорт конкретизированных шаблонов в DLL, это должно быть ДО использования 
// в нижележащих классах (https://support.microsoft.com/en-us/kb/168958)
// тж. см. возможную проблему https://s.pais.hse.ru/redmine/issues/428
#ifdef LDOPA_DLL

LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::vector<const void*>;


#endif // LDOPA_DLL



/** \brief Interface declares types that should behavior like Ts State Identifier
 *
 *  \TODO May be, interface should be moved to a separate declaration module
 */
class LDOPA_API ITsStateId {
protected:
    /** \brief Protected Destructor: It is not intended to delete an object through this interface! */
    ~ITsStateId() {};

public:

    /** \brief Declares operator == that returns true if two given states considered as to be equal */
    virtual bool isEqualTo(const ITsStateId* that) const = 0;

    /** \brief Declares operator < that returns true if left stateId is lesser that right stateId */
    virtual bool isLessThan(const ITsStateId* that) const = 0;
}; // class ITsStateId


/** \brief Transition system state ID implementation based on a vector of untyped const pointers
 *
 *  Actually, the only desirable operation for TraceID is to determine if two given IDs are equal.
 *  Hence, if a state id is represented as a vector of pointers, two such IDs are considered
 *  to be equal iff their vectors have the same sizes and corresponding pointers stored
 *  in the vectors are equal
 */
class LDOPA_API TsPtrStateId : public ITsStateId
{
public:
    //-----<Types>-----
    
    /** \brief Declares Vector of (untyped) Pointers type */
    typedef std::vector<const void*> VectorOfPtrs;
public:
    TsPtrStateId() {}

    /** \brief Constructs and reserves the vector capacity by set it to \a cap*/
    TsPtrStateId(size_t cap)
        //: _ptrs(size)
    {
        _ptrs.reserve(cap);
    }

    /** \brief Constructor with initializer list (Cx11) */
    TsPtrStateId(const std::initializer_list<VectorOfPtrs::value_type>& v);

    /** \brief Inititalize with single pointer */
    TsPtrStateId(const void* ptr1)
    {
        _ptrs.push_back(ptr1);
    }

    // copy constructor for debug urposes only
    TsPtrStateId(const TsPtrStateId& that)
    {
        _ptrs = that._ptrs;
    }

    // try to implement move semantics: construction
    TsPtrStateId(TsPtrStateId&& that)
    {
        _ptrs = std::move(that._ptrs);
    }

    // try to implement move semantics: copy operation
    TsPtrStateId& operator=(TsPtrStateId&& that) 
    {
        _ptrs = std::move(that._ptrs);

        return *this;
    }

    // copy operation implementing copy and swap idion (https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-and-swap)
    TsPtrStateId& operator= (const TsPtrStateId& that)
    {
        TsPtrStateId temp(that);    // Copy-constructor -- RAII
        //temp.swap(*this);           // Non-throwing swap
        _ptrs = std::move(temp._ptrs);

        return *this;
    }

public:
    void swap(TsPtrStateId&& that)
    {
        TsPtrStateId t = that;
        that._ptrs = std::move(_ptrs);
        _ptrs = std::move(t._ptrs);
    }
public:
    virtual bool isEqualTo(const ITsStateId* that) const override;
    virtual bool isLessThan(const ITsStateId* that) const override;

public:
    // local version of operators < and ==
    /** \brief Declares operation as it declares Compare concept 
     *
     *  For compare concept see http://en.cppreference.com/w/cpp/concept/Compare .
     *  lhv (given as this) is compared with rhv given as \a that
     */
    bool operator == (const TsPtrStateId& that) const; // { return isEqualTo(that); };
    
    /** \brief Return true if \a *this is equal to \a rhv */
    bool operator < (const TsPtrStateId& that) const; // { return isLessThan(that); };

    /** \brief Syntax sugar for operator < */
    bool operator > (const TsPtrStateId& that) const { return that < *this; };

public:
    /** \brief Return the underlied vector of pointers */
    VectorOfPtrs& getPtrs() { return _ptrs;  }

    /** \brief Const overloaded version of getPtrs() */
    const VectorOfPtrs& getPtrs() const { return _ptrs; }

    /** \brief Append a given element \a el to the end of trace id */
    void append(const void* el);
protected:  

    /** \brief Stores vector of pointers */
    VectorOfPtrs _ptrs;
}; // class TsPtrStateId
 

/** \brief Represents a basic version of transition systems
 *
 *  Internal representation of TS is based on BGL library
 *  
 *  This is an initial iteration to a TS concept as it is still unclear hot to 
 *  deal with underlying graph so that to prevent an excessive object recreation...
 */
class LDOPA_API BaseTs
{
public:
    //-----<Types>-----
    /** \brief State type alias for graph Vertex */
    typedef BaseListBGraph::Vertex State;

    /** \brief Transition type alias for graph Edge */
    typedef BaseListBGraph::Edge Transition;

    /** \brief Type alias for state/vertex return result */
    typedef BaseListBGraph::VertexRetResult StateRetResult;

    /** \brief Type alias for edge/transition return result */
    typedef BaseListBGraph::EdgeRetResult TransRetResult;

public:
    BaseTs();
    ~BaseTs();
public:
    /** \brief Returns a ref to the underlying graph */
    BaseListBGraph& getGraph() { return _gr;  }

    /** \brief Const overloaded version of getGraph() */
    const BaseListBGraph& getGraph() const { return _gr; }

    /** \brief Return a number of states in the TS */
    size_t getStatesNum() const { return _gr.getVerticesNum(); }

    /** \brief Return a number of edges in the TS */
    size_t getEdgesNum() const { return _gr.getEdgesNum(); }

protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    BaseTs(const BaseTs&);                 // Prevent copy-construction
    BaseTs& operator=(const BaseTs&);      // Prevent assignment

protected:
    /** \brief Stores underlying graph */
    BaseListBGraph _gr;
}; // class BaseTs



}}}} // namespace xi::ldopa::ts::obsolete1


#endif // XI_LDOPA_TRSS_MODELS_OBSOLETE1_BASETS_H_
