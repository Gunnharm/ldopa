////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri Nets Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      04.04.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Generic class template for various Petri nets.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_MODELS_GEN_PETRINET_H_
#define XI_LDOPA_PN_MODELS_GEN_PETRINET_H_

#pragma once

// std
#include <iterator>

// boost
#include <boost/blank.hpp>


// ldopa
#include "xi/ldopa/utils.h"
#include "xi/ldopa/graphs/boost/bgl_graph_wrappers.h"



namespace xi { namespace ldopa { namespace pn {;   //


/** \brief Defines a traits template for PN Data, which is embedded in Pos, Trans and Arcs. 
 * 
 *  This template class can be partially defined for specific data type to redefine 
 *  the addressing mechanism into the most appropriate way.
 *  
 *  // TODO: нужно ли делать отдельные типы для Pos, Trans, Arc (как в TS)? Вдруг зачем-то
 *  // понадобится по-своему для каждого сорта сделать?..
 */
template<typename TData>
class PnData_traits {
public:
    typedef         TData      Val;       ///< Value type.
    typedef const   TData&     СArg;      ///< Argument type.
    typedef         TData&     Ref;       ///< Ref-argument type.
    typedef const   TData&     CRes;      ///< Constant res.
};


#pragma region PnVertex Traits

/** \brief Traits class for a bundle to embedd in PN vertice, which is either Pos or Trans. */
template<typename TVertData>
struct PnVertBundle_traits {

    #pragma pack(push, 1)    
    /** \brief Vertex property bundle. */
    struct _PnVertBundle {
    //protected:
        TVertData data;                     ///< Custom data embedded in vertices.
        unsigned char type : 1;             ///< Vertex type: 0 — pos, 1 — trans.
        unsigned char misc : 7;             ///< Additional 7 bits (padding to a whole byte), 
                                            ///  which can be used arbitrarily.
    };
    #pragma pack(pop)

    //---<Types of the traits class's concept>---
           
    // vertex bundle
    typedef       _PnVertBundle     VertBundle;             ///< Propert bundle type.
    typedef const _PnVertBundle&    VertBundleCArg;         ///< Bundle as const argument (parameter of a method).
    typedef const _PnVertBundle&    VertBundleCRes;         ///< Bundle as const result (for returning values).
    typedef       _PnVertBundle&    VertBundleArg;          ///< Bundle as non-const  argument (parameter of a method).

    // vertex data
    typedef typename PnData_traits<TVertData>::СArg VertDataCArg;   ///< Vertex data as a const argument.
    typedef typename PnData_traits<TVertData>::CRes VertDataCRes;   ///< Vertex data as a const result.
    

    //typedef TVertData& VertDataRes;                         ///< Type for a reference to a custom data object.

    //---<Static method of the traits class's concept>---

    /** \brief Returns true if the given vertex data \a bundle determines a transition, false for position. */
    static bool isTrans(VertBundleCArg bundle) { return (bundle.type != 0); }

    /** \brief Sets the type of a vertex for the given bundle \a bundle, where \a isTrans is true 
     *  for the transition and false for the position type.
     */
    static void setType(VertBundleArg bundle, bool isTrans) { bundle.type = isTrans; }

    /** \brief Returns 7 misc bits. */
    static unsigned char getMisc(VertBundleCArg bundle) { return bundle.misc; }

    /** \brief Sets 7 misc bits. */
    static void setMisc(VertBundleArg bundle, unsigned char misc) { bundle.misc = misc; }

    /** \brief Returns a valid const ref (or const value) for the embedded data. */
    static VertDataCRes getData(VertBundleCArg bundle) { return bundle.data; }

    /** \brief Sets the embedding data. */
    static void setData(VertBundleArg bundle, VertDataCArg data) { bundle.data = data; }
}; // struct PnVertBundle_traits


/** \brief Partially specified variant for PnVertBundle_traits with no additional data in vertices at all. */
template<>
struct PnVertBundle_traits<boost::no_property> {
    
    /** \brief Vertex property bundle. */
    struct _PnVertBundle {
    //protected:
        // There are no custom data embedded in vertices at all.
        unsigned char type : 1;             ///< Vertex type: 0 — pos, 1 — trans.
        unsigned char misc : 7;             ///< Additional 7 bits (padding to a whole byte), 
                                            ///< which can be used arbitrarily.
    };

    //---<Types of the traits class's concept>---
           
    // vertex bundle
    typedef       _PnVertBundle     VertBundle;             ///< Propert bundle type.
    typedef const _PnVertBundle&    VertBundleCArg;         ///< Bundle as const argument (parameter of a method).
    typedef const _PnVertBundle&    VertBundleCRes;         ///< Bundle as const result (for returning values).
    typedef       _PnVertBundle&    VertBundleArg;          ///< Bundle as non-const  argument (parameter of a method).

    // vertex data
    typedef boost::no_property VertDataCArg;       ///< Vertex data (no data) as a const argument.
    typedef boost::no_property VertDataCRes;       ///< Vertex data (no data) as a const result.
    

    //typedef TVertData& VertDataRes;                         ///< Type for a reference to a custom data object.

    //---<Static method of the traits class's concept>---

    /** \brief Returns true if the given vertex data \a bundle determines a transition, false for position. */
    static bool isTrans(VertBundleCArg bundle) { return (bundle.type != 0); }

    /** \brief Sets the type of a vertex for the given bundle \a bundle, where \a isTrans is true 
     *  for the transition and false for the position type.
     */
    static void setType(VertBundleArg bundle, bool isTrans) { bundle.type = isTrans; }

    /** \brief Returns 7 misc bits. */
    static unsigned char getMisc(VertBundleCArg bundle) { return bundle.misc; }

    /** \brief Sets 7 misc bits. */
    static void setMisc(VertBundleArg bundle, unsigned char misc) { bundle.misc = misc; }

    /** \brief Returns a valid const ref (or const value) for the embedded data. */
    static VertDataCRes getData(VertBundleCArg bundle) { return boost::no_property(); }

    /** \brief Sets the embedding data. */
    static void setData(VertBundleArg bundle, VertDataCArg data) { }
}; // struct PnVertBundle_traits

#pragma endregion // PnVertex Traits

#pragma region PnArc Traits

/** \brief Defines some base types for all PnArcBundle_traits sorts. */
struct PnArcBundle_traits_base {
    enum ArcType
    {
        atReg,              ///< Regular arc.
        atInhib             ///< Inhibitor arc.
    };
}; 


// пока что это вообще 1:1 то же, что и для вершин.. но потом все может измениться!
/** \brief Traits class for a bundle to embedd in PN arc. */
template<typename TArcData>
struct PnArcBundle_traits : public PnArcBundle_traits_base {

    #pragma pack(push, 1)    
    /** \brief Arc property bundle. */
    struct _PnArcBundle {
    //protected:
        TArcData data;                      ///< Custom data embedded in arcs.
        unsigned char type : 1;             ///< Arc type: 0 — normal, 1 — inhibitor.
        unsigned char misc : 7;             ///< Additional 7 bits (padding to a whole byte), 
                                            ///  which can be used arbitrarily.
    };
    #pragma pack(pop)

    //---<Types of the traits class's concept>---
           
    // vertex bundle
    typedef       _PnArcBundle     ArcBundle;           ///< Propert bundle type.
    typedef const _PnArcBundle&    ArcBundleCArg;       ///< Bundle as const argument (parameter of a method).
    typedef const _PnArcBundle&    ArcBundleCRes;       ///< Bundle as const result (for returning values).
    typedef       _PnArcBundle&    ArcBundleArg;        ///< Bundle as non-const  argument (parameter of a method).

    // vertex data
    typedef typename PnData_traits<TArcData>::СArg ArcDataCArg;   ///< Arc data as a const argument.
    typedef typename PnData_traits<TArcData>::CRes ArcDataCRes;   ///< Arc data as a const result.
    

    //typedef TVertData& VertDataRes;                         ///< Type for a reference to a custom data object.

    //---<Static method of the traits class's concept>---

    /** \brief Returns type of the arc. */
    static ArcType getArcType(ArcBundleCArg bundle) { return (bundle.type == 0) ? atReg : atInhib; }

    /** \brief Sets the type of an arc for the given bundle \a bundle to the given type \a at. */
    static void setArcType(ArcBundleArg bundle, ArcType at) { bundle.type = (at == atReg) ? 0 : 1; }


    /** \brief Returns 7 misc bits. */
    static unsigned char getMisc(ArcBundleCArg bundle) { return bundle.misc; }

    /** \brief Sets 7 misc bits. */
    static void setMisc(ArcBundleArg bundle, unsigned char misc) { bundle.misc = misc; }

    /** \brief Returns a valid const ref (or const value) for the embedded data. */
    static ArcDataCRes getData(ArcBundleCArg bundle) { return bundle.data; }

    /** \brief Sets the embedding data. */
    static void setData(ArcBundleArg bundle, ArcDataCArg data) { bundle.data = data; }
}; // struct PnVertBundle_traits


/** \brief Partially specified variant for PnVertBundle_traits with no additional data in vertices at all. */
template<>
struct PnArcBundle_traits<boost::no_property> : public PnArcBundle_traits_base {
    
    /** \brief Vertex property bundle. */
    struct _PnArcBundle {
    //protected:
        // There are no custom data embedded in vertices at all.
        unsigned char type : 1;             ///< Arc type: 0 — normal, 1 — inhibitor.
        unsigned char misc : 7;             ///< Additional 7 bits (padding to a whole byte), 
                                            ///  which can be used arbitrarily.
    };

    //---<Types of the traits class's concept>---
           
    // vertex bundle
    typedef       _PnArcBundle     ArcBundle;             ///< Propert bundle type.
    typedef const _PnArcBundle&    ArcBundleCArg;         ///< Bundle as const argument (parameter of a method).
    typedef const _PnArcBundle&    ArcBundleCRes;         ///< Bundle as const result (for returning values).
    typedef       _PnArcBundle&    ArcBundleArg;          ///< Bundle as non-const  argument (parameter of a method).

    // vertex data
    typedef boost::no_property ArcDataCArg;       ///< Vertix data (no data) as a const argument.
    typedef boost::no_property ArcDataCRes;       ///< Vertix data (no data) as a const result.


    //---<Static method of the traits class's concept>---

    /** \brief Returns type of the arc. */
    static ArcType getArcType(ArcBundleCArg bundle) { return (bundle.type == 0) ? atReg : atInhib; }

    /** \brief Sets the type of an arc for the given bundle \a bundle to the given type \a at. */
    static void setArcType(ArcBundleArg bundle, ArcType at) { bundle.type = (at == atReg) ? 0 : 1; }


    /** \brief Returns 7 misc bits. */
    static unsigned char getMisc(ArcBundleCArg bundle) { return bundle.misc; }

    /** \brief Sets 7 misc bits. */
    static void setMisc(ArcBundleArg bundle, unsigned char misc) { bundle.misc = misc; }

    /** \brief Returns a valid const ref (or const value) for the embedded data. */
    static ArcDataCRes getData(ArcBundleCArg bundle) { return boost::no_property(); }

    /** \brief Sets the embedding data. */
    static void setData(ArcBundleArg bundle, ArcDataCArg data) { }
}; // struct PnVertBundle_traits

#pragma endregion // PnArc Traits

//==============================================================================
// class BaseGenPetriNet
//==============================================================================

// Base Abstract Petri Net Generic...

template<
    typename TVertData  = boost::no_property,       // Данные на позициях/переходах (можно только один общий тип!).
    typename TArcData   = boost::no_property        // Данные на дугах.
>
class BaseGenPetriNet
{
public:
    // types
    /** \brief Packing type for Vertices (poss and transs) for embedding as BGL property. */
    typedef typename PnVertBundle_traits<TVertData>::VertBundle VertBundle;

    /** \brief Packing type for Arcs for embedding as BGL property. */
    typedef typename PnArcBundle_traits<TArcData>::ArcBundle ArcBundle;

    /** \brief Defining a type for the underlying graph.
     *
     *  The most newest approach is to use BGD directly for making a most appropriate type.
     */
    typedef boost::adjacency_list<
        boost::listS,               // out edge storage
        boost::listS,               // vertices storage
        boost::bidirectionalS,      // bi-directional      
        // properties
        VertBundle,                 // vertices properties
        ArcBundle                   // arcs properties
    > Graph;
}; // class BaseGenPetriNet


//==============================================================================
// class GenPetriNet
//==============================================================================


/** \brief Template class for various Petri nets.
 *
 *  Since Petri net is totally based on the underlying graph, one need to distinguish its components.
 *  Here Vertex(ices) and Edge(s) are the components of the graph and they do not typed
 *  according to specific elements of Petri net.
 *  Typed version are: PnVertex, Position and Transition are equivalents of graph's vertices;
 *  BaseArc, PTArc and TPArc are equivalents of graph's edges.
 */
template<
    typename TVertData = boost::no_property,       // Данные на позициях/переходах (можно только один общий тип!).
    typename TArcData = boost::no_property        // Данные на дугах.
>
class GenPetriNet :
    public BaseGenPetriNet<TVertData, TArcData>,
    protected gr::BoostBidiGraphP < typename BaseGenPetriNet<TVertData, TArcData>::Graph >
{
public:

    //-----<Traits types>-----
#pragma region Vertice and Arc Traits Types

    /** \brief Type for vertex property ref-argument. */
    typedef typename PnVertBundle_traits<TVertData>::VertBundleArg VertBundleArg;

    /** \brief Bundle as const argument (parameter of a method). */
    typedef typename PnVertBundle_traits<TVertData>::VertBundleCArg VertBundleCArg;
  
    /** \brief Arc type. */
    typedef typename PnArcBundle_traits_base::ArcType ArcType;

#pragma endregion               // Vertice and Arc Traits Types

#pragma region Positions/Transitions Wrappers

    //=========================================================================



    /** \brief Base class for both positions and transition. */
    class PnVertex {
        friend class GenPetriNet;   ///< Outer (owner) class have grants.

    public:

        // UPD: добавил дефолтный конструктор
        PnVertex() {}

    protected:
        /** \brief Protected constructor, which is only for internal creating of PN vertices. */
        PnVertex(Vertex v) //, VertBundleArg vBundle, bool isTrans)
            : _vert(v)
        {            
        }
    protected:
        /** \brief Factory method for adding a new vertex (P or T) to a PN with valid properties
         *  w/o a vertex wrapper. */
        static Vertex addVertex(GenPetriNet& pn, bool isTrans)
        {
            // создаем новую вершину в подлежащем графе и маркируем
            Vertex v = pn.addVertex();
            PnVertBundle_traits<TVertData>::setType(pn.getGraph()[v], isTrans);

            // здесь же инкрементим число позиций/транзиций
            if (isTrans)
                ++pn._numOfTrans;
            else
                ++pn._numOfPos;

            return v;
        }
              
    public:
        /** \brief Returns the underlying vertex. */
        inline Vertex getVertex() const { return _vert; }

        /** \brief Returns true if this vertex is transition or false if it is position. 
         *
         *  Paramter \a pn is needed to get pos/trans property from a PN this vertex is a part of.
         */
        inline bool isTrans(const GenPetriNet& pn) const 
        {
            return GenPetriNet::isTransition(pn.getGraph()[_vert]);
            //{
            //    return PnVertBundle_traits<TVertData>::isTrans(vertBundle);
        }

        /** \brief Ordering for maps. */
        inline bool operator<(const PnVertex& v) const
        {
            return (_vert < v._vert);
        }

        /** \brief Equality for hashses and so on. */
        inline bool operator==(const PnVertex& v) const
        {
            return (_vert == v._vert);
        }
    protected:
        Vertex _vert;                               ///< BGL Vertex that represents either position or transition.
    }; // class PnVertex

    //=========================================================================

    /** \brief Mostly tag class for representing a PN position as an individual type. */
    class Position : public PnVertex {
        friend class PosIter;
        friend class GenPetriNet;
    
    public:
        
        // UPD: добавил дефолтный конструктор (17/08/2018)
        Position()/* : PnVertex(Vertex()) */{}
    
    protected:
        /** \brief Protected constructor, which is only for internal creating of PN position wrappers over a vertex. */
        Position(Vertex v)
            : PnVertex(v)
        {
        }

    public:
        /** \brief Factory method for adding a new Position to the given graph. */
        static Position addPositionTo(GenPetriNet& pn)
        {
            return Position(addVertex(pn, false));
        }
    }; // class Position 

    //=========================================================================

    /** \brief Mostly tag class for representing a PN transition as an individual type. */
    class Transition : public PnVertex {
        friend class TransIter;
        friend class GenPetriNet;
    public:
        // UPD: добавил дефолтный конструктор (17/08/2018)
        Transition() /*: PnVertex(Vertex())*/ {}

    protected:
        /** \brief Protected constructor, which is only for internal creating of PN transition wrappers over a vertex. */
        Transition(Vertex v)
            : PnVertex(v)
        {
        }

    public:
        /** \brief Factory method for adding a new Position to the given graph. */
        static Transition addTransitionTo(GenPetriNet& pn)
        {
            return Transition(addVertex(pn, true));
        }
    }; // class Transition

    //=========================================================================

    //typedef typename Edge Arc;                  ///< Type alias for Arc.

    /** \brief Base class for typed arcs. */
    struct BaseArc
    {
        BaseArc() {}
        BaseArc(Edge e) : edge(e) {}
        Edge edge;

        /** \brief Ordering for maps. */
        inline bool operator<(const BaseArc& rhv) const
        {
            return (edge < rhv.edge);
        }

        /** \brief Equality for comparison. */
        inline bool operator==(const BaseArc& rhv)
        {
            return (edge == rhv.edge);
        }
    };

    //=========================================================================

    /** \brief Class for typed PT-Arc arcs. */
    struct PTArc : public BaseArc
    {
        PTArc() {}
        PTArc(Edge e) : BaseArc(e) {}
        //PTArc(Arc a) : BaseArc(a) {}
    };

    //=========================================================================

    /** \brief Class for typed TP-Arc arcs. */
    struct TPArc : public BaseArc
    {
        TPArc() {}
        TPArc(Edge e) : BaseArc(e) {}
        //TPArc(Arc a) : BaseArc(a) {}
    };

    /** \brief Type alias for PTArc returning result. 
     * 
     *  Normally, if the second part of this pair is true, then the result
     *  is presented (found, exists etc), otherwise it is false.
     */
    typedef std::pair<PTArc, bool> PTArcRes;

    /// Type alias for TPArc returning result. 
    typedef std::pair<TPArc, bool> TPArcRes;

#pragma endregion      // Positions/Transitions Wrappers

#pragma region                  // Custom iterators

    /** \brief Class for iterating vertices of specific type only from the underlying BG. 
     *   
     *  \tparam TVertex is a vertex type, either Position or Transitions
     *  \tparam TFitFunctor is a struct with method bool operator()(VertexIter), which returns            -----bool isSuitable(VertexIter), which returns------
     *  true if the given vertex suit the filter.
     */
    template<typename TVertex, typename TFitFunctor>
    class VertIter {
    public:
        // types according to the iterator concept
        typedef VertIter self;               // это ли нужно ли?

        // from iterator_traits<>
        typedef TVertex value_type;            ///< The type of the values that can be obtained by dereferencing the iterator.
        typedef const TVertex& reference;      ///< Defines a reference to the type iterated over(value_type).
        typedef const TVertex* pointer;        ///< Defines a pointer to the type iterated over(value_type)
        typedef std::ptrdiff_t difference_type;
        typedef std::forward_iterator_tag iterator_category;
    
    public:

        /** \brief Standards require to have a default constructor for forw. iterators. */
        VertIter()
            : _filter(nullptr)
            , _curIt(VertexIter())
            , _endIt(VertexIter())
            , _vert(Vertex())
        {
        }

        /** \brief Constructs iterator with the original vertex iterator. */
        VertIter(const GenPetriNet* pn, VertexIter first, VertexIter end)
            : _filter(pn) //_pn(pn)
            , _curIt(first)
            , _endIt(end)
            , _vert(Vertex())
        {
            gotoNextSuitableVertex();
        }

        /** \brief This variant is for end() iterator only. */
        VertIter(VertexIter end)
            //: _pn(nullptr)
            : _filter(nullptr)
            , _curIt(end)
            , _endIt(end)
            , _vert(Vertex())
        {
        }


    public:
        // implementing iterator concept
        reference operator*()
        {
            // проверка на конец пусть осуществляется подлежащим оператором
            // кроме того, гарантируем, что если не конец, то это точно позиция
            Vertex v = *_curIt;
            _vert = TVertex(v);                 // TODO: так нехорошо: д.б. const!

            return _vert;
        }

        bool operator==(const self& other) const
        {
            return _curIt == other._curIt;
        }

        bool operator!=(const self& other) const
        {
            return _curIt != other._curIt;
        }

        self& operator++()
        {
            ++_curIt;               // сдвинем внутренний итератор
            gotoNextSuitableVertex();          // дальше пробежимся до след. позиции
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++(*this);
        }

    protected:

        /** \brief If the current dereferenced vertex is not a position, iterates over the range 
         *  until a first position is found or the end is approached.
         */
        void gotoNextSuitableVertex()
        {
            // когда уже в конце, то все
            while (_curIt != _endIt)
            {                
                //if (isSuitableVertex())                // проверяем, является ли текущий элемент позицией
                //if(TFitFunctor::isSuitable(_pn, *_curIt))
                if(_filter(*_curIt))
                    return;
                ++_curIt;                   // иначе движем дальше и по новой все проверяем
            }            
        }

    protected:
        //const GenPetriNet* _pn;     ///< Ptr to a PN for checking vertex types.
        TFitFunctor _filter;        ///< Filter functor.
        VertexIter _curIt;          ///< Current original iterator over a set of vertices.
        VertexIter _endIt;          ///< Ending limiter not to pass over the end of a range.

        TVertex _vert;              ///< Stores a temporary wrapper over a vertex object.
    }; // class PosIterator
  
    struct BaseIterFilter
    {
        BaseIterFilter(const GenPetriNet* petrinet) : pn(petrinet) {}
        const GenPetriNet* pn;
    };

    //template<>
    struct PosIterFilter : public BaseIterFilter 
    {
        PosIterFilter(const GenPetriNet* pn) : BaseIterFilter(pn) {}
        bool operator()(Vertex v) 
        {
            return !(GenPetriNet::isTransition(pn->getGraph()[v]));
        }
    };

    struct TransIterFilter : public BaseIterFilter 
    {
        TransIterFilter(const GenPetriNet* pn) : BaseIterFilter(pn) {}
        bool operator()(Vertex v)
        {
            return (GenPetriNet::isTransition(pn->getGraph()[v]));
        }
    };

    // создаем псевдофильтр, пропускающий все вертексы, чтобы новый тип 
    // для VertexIter -> PnVertexIter не сочинять
    // #delayedtodo: возможно, надо переработать!
    struct PnVertIterFilter : public BaseIterFilter
    {
        PnVertIterFilter(const GenPetriNet* pn) : BaseIterFilter(pn) {}
        bool operator()(Vertex v)
        {
            return true;        // все подряд!
        }
    };

    typedef VertIter<PnVertex, PnVertIterFilter> PnVertIter;    // #delayedtodo: возможно, надо переработать (см. замечание выше)!
    typedef VertIter<Position, PosIterFilter> PosIter;
    typedef VertIter<Transition, TransIterFilter> TransIter;

    //===============================================================================



    //===============================================================================

    /** \brief Generic iterator type for distinguishable PT- and TP-arcs. 
     * 
     *  \tparam TArc determines a type for arcs: PTArc or TPArc.
     *  \tparam TEdgeIter determines a type for edge iterator: IedgeIter or OedgeIter.
     *  So far, we consider 4 combinations of iterators defined by this template:
     *  <PTArc, IedgeIter>, <PTArc, OedgeIter>, <TPArc, IedgeIter>, <TPArc, OedgeIter>.
     */
    template<typename TArc, typename TArcIter>
    class GenArcIter {
    public:
        // types according to the iterator concept
        typedef GenArcIter self;               // это ли нужно ли?

        // from iterator_traits<>
        typedef TArc value_type;                    ///< The type of the values that can be obtained by dereferencing the iterator.
        typedef const TArc& reference;              ///< Defines a reference to the type iterated over(value_type).
        typedef const TArc* pointer;                ///< Defines a pointer to the type iterated over(value_type)
        typedef std::ptrdiff_t difference_type;
        typedef std::forward_iterator_tag iterator_category;

    public:
        /** \brief Standards require to have a default constructor for forw. iterators. */
        GenArcIter() : _pn(nullptr)
        {
        }

        /** \brief Constructs iterator with the original vertex iterator. */
        GenArcIter(const GenPetriNet* pn, TArcIter it)
            : _pn(pn)
            , _curIt(it)
        {
        }

        /** \brief This variant is for end() iterator only. */
        GenArcIter(TArcIter end)
            : _pn(nullptr)
            , _curIt(end)
        {
        }

    public:
        // implementing iterator concept
        reference operator*()
        {
            _arc = TArc(*_curIt);
            return _arc;
        }

        bool operator==(const self& other) const
        {
            return _curIt == other._curIt;
        }

        bool operator!=(const self& other) const
        {
            return _curIt != other._curIt;
        }

        self& operator++()
        {
            ++_curIt;               // сдвинем внутренний итератор
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++(*this);
        }

    protected:
        const GenPetriNet* _pn;
        TArcIter _curIt;           ///< Current original iterator over a set of input arcs.
        TArc _arc;
    }; // class GenArcIter


    // possible specifications of GenArcIter

    typedef GenArcIter<PTArc, OedgeIter> PoTArcIter;            ///< PoT-iterator type for getOutArcs(Pos) method.
    typedef std::pair<PoTArcIter, PoTArcIter> PoTArcIterPair;   ///< Pair of PoT-iterators.

    typedef GenArcIter<PTArc, IedgeIter> PTiArcIter;            ///< PTi-iterator type for getInArcs(Trans) method.
    typedef std::pair<PTiArcIter, PTiArcIter> PTiArcIterPair;   ///< Pair of PTi-iterators.

    typedef GenArcIter<TPArc, OedgeIter> ToPArcIter;            ///< ToP-iterator type for getOutArcs(Trans) method.
    typedef std::pair<ToPArcIter, ToPArcIter> ToPArcIterPair;   ///< Pair of ToP-iterators.

    typedef GenArcIter<TPArc, IedgeIter> TPiArcIter;            ///< TPi-iterator type for getInArcs(Pos) method.
    typedef std::pair<TPiArcIter, TPiArcIter> TPiArcIterPair;   ///< Pair of TPi-iterators.


    // добавочка от 21.07.2018: нужны итераторы для общих дуг, может быть
    // можно этот адаптер использовать?
    typedef GenArcIter<BaseArc, EdgeIter> BaseArcIter;          ///< Base-arc iterator type for getArcs() method.
    typedef std::pair<BaseArcIter, BaseArcIter> BaseArcIterPair;///< Pair of Base-arc iterators.

    //===============================================================================

    // traits base type for PPSetIter class
    template<typename TArcIter>
    struct PPSetIter_traits {        
    };


    // PTiArcIter - Входная PT-дуга для транзиции (PTi). Итератор возвращает для такой дуги ее source — пред. позицию для данной транзиции.
    template<>
    struct PPSetIter_traits < PTiArcIter > {
        typedef PTArc       TArc;               // тип дуги — PT
        typedef Position    TVertex;            // тип вершин (возвращаемый таким итератором) — позиция
        //static TVertex getVertex(const GenPetriNet* pn, TArc arc)  // можно так, но хуже читается
        static Position getVertex(const GenPetriNet* pn, PTArc arc)
        {
            return pn->getSrcPos(arc);
        }
    };

    // ToPArcIter - Выходная TP-дуга для транзиции (ToP). Итератор возвращает для такой дуги ее target след. позицию для данной транзиции.
    template<>
    struct PPSetIter_traits < ToPArcIter > {
        typedef TPArc       TArc;               // тип дуги — TP
        typedef Position    TVertex;            // тип вершин (возвращаемый таким итератором) — позиция
        static Position getVertex(const GenPetriNet* pn, TPArc arc)
        {
            return pn->getTargPos(arc);
        }
    };
    
    // TPiArcIter - Входная TP-дуга для позиции (TPi). Итератор возвращает для такой дуги ее source — пред. транзицию для данной позиции.
    template<>
    struct PPSetIter_traits < TPiArcIter > {
        typedef TPArc       TArc;               // тип дуги — PT
        typedef Transition  TVertex;            // тип вершин (возвращаемый таким итератором) — транзиция
        static Transition getVertex(const GenPetriNet* pn, TPArc arc)
        {
            return pn->getSrcTrans(arc);
        }
    };


    // PoTArcIter - Выходная PT-дуга для позиции (PoT). Итератор возвращает для такой дуги ее target след. транзицию для данной позиции.
    template<>
    struct PPSetIter_traits < PoTArcIter > {
        typedef PTArc       TArc;               // тип дуги — PT
        typedef Transition    TVertex;            // тип вершин (возвращаемый таким итератором) — позиция
        static Transition getVertex(const GenPetriNet* pn, PTArc arc)
        {
            return pn->getTargTrans(arc);
        }
    };
    

    /** \brief Class for iterating input/output positions/transition through an 
     *  arc iterator. 
     *
     *  Dereferenced iterator returns a pair of an arc (input or output) and the vertex 
     *  correpsonding to the arc.
     *  Aimed for use with preset and postset methods.
     */
    //template<typename TVertex>
    template<typename TArcIter>
    class PPSetIter {
    public:
        // additional definitions through traits class
        typedef typename PPSetIter_traits<TArcIter>::TArc       TArc;
        typedef typename PPSetIter_traits<TArcIter>::TVertex    TVertex;

        /// Composite structure of a vertex and an arc.
        typedef std::pair<TVertex, TArc> Value;

        // types according to the iterator concept
        typedef PPSetIter self;               // это ли нужно ли?

        // from iterator_traits<>
        typedef Value value_type;                     ///< The type of the values that can be obtained by dereferencing the iterator.
        typedef const Value& reference;               ///< Defines a reference to the type iterated over(value_type).
        typedef const Value* pointer;                 ///< Defines a pointer to the type iterated over(value_type)
        typedef std::ptrdiff_t difference_type;
        typedef std::forward_iterator_tag iterator_category;

    public:
        /** \brief Standards require to have a default constructor for forw. iterators. */
        PPSetIter() 
            : _pn(nullptr)
            , _val(Vertex(), TArc())
        {
        }

        /** \brief Constructs iterator with the original vertex iterator. */
        PPSetIter(const GenPetriNet* pn, TArcIter it)
            : _pn(pn)
            , _curIt(it)
            , _val(Vertex(), TArc())
        {
        }

        /** \brief This variant is for end() iterator only. */
        PPSetIter(TArcIter end)
            : _pn(nullptr)
            , _curIt(end)
            , _val(Vertex(), TArc())
        {
        }


    public:
        // implementing iterator concept through traits class
        reference operator*()
        {
            // проверка на конец пусть осуществляется подлежащим оператором            
            TArc arc = *_curIt;     // сперва вытаскиваем из итератора типизированную дугу
            // далее для дуги определяем входную позицию (искомую)
            TVertex vert = PPSetIter_traits<TArcIter>::getVertex(_pn, arc);
            _val = Value(vert, arc);
            
            return _val;

        }


        bool operator==(const self& other) const
        {
            return _curIt == other._curIt;
        }

        bool operator!=(const self& other) const
        {
            return _curIt != other._curIt;
        }

        self& operator++()
        {
            ++_curIt;               // сдвинем внутренний итератор
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++(*this);
        }

    protected:
        const GenPetriNet* _pn;     ///< Stores a corresponding PN.
        TArcIter _curIt;            ///< Current original iterator over a set of input arcs.
        //TVertex _vert;              ///< Stores corresponding position or trans inside the iterator.
        Value _val;                 ///< Stores a "vertex-arc" pair.

    }; // class PPSetIter


    // итератор входных позиций для данной транзиции
    typedef typename PPSetIter<PTiArcIter> InPosIter;
    typedef std::pair<InPosIter, InPosIter> InPosIterPair;

    // итератор выходных позиций для данной транзиции
    typedef typename PPSetIter<ToPArcIter> OutPosIter;
    typedef std::pair<OutPosIter, OutPosIter> OutPosIterPair;

    // итератор входных транзиций для данной позиции
    typedef typename PPSetIter<TPiArcIter> InTransIter;
    typedef std::pair<InTransIter, InTransIter> InTransIterPair;

    // итератор выходных транзиций для данной позиции
    typedef typename PPSetIter<PoTArcIter> OutTransIter;
    typedef std::pair<OutTransIter, OutTransIter> OutTransIterPair;

#pragma endregion                 // Custom Iterators

    /** \brief Alias for a base graph wrapper type. */
    typedef gr::BoostBidiGraphP < typename BaseGenPetriNet<TVertData, TArcData>::Graph >
        BaseGraph;               

    typedef typename OedgeIter OArcIter;        ///< Type alias for Output Arc Iterator.
    typedef typename IedgeIter IArcIter;        ///< Type alias for Input Arc Iterator.
    typedef typename EdgeIter ArcIter;          ///< Type alias for Iterator of Arcs of both directions.

    /** \brief ADDED 21/07/2018: custom iter pair for specific PnVertex. */
    typedef std::pair<PnVertIter, PnVertIter> PnVertIterPair;

    /** \brief A pair of positions iterators, which represents a collection of Positions. */
    typedef std::pair<PosIter, PosIter> PosIterPair;

    /** \brief A pair of transition iterators, which represents a collection of Transitions. */
    typedef std::pair<TransIter, TransIter> TransIterPair;
    

    /** \brief A pair of arc iterators, which represents a collection of output arcs. */
    typedef OedgeIterPair OArcIterPair;

    /** \brief A pair of arc iterators, which represents a collection of input arcs. */
    typedef IedgeIterPair IArcIterPair;

    /** \brief A pair of arc iterators, which represents a collection of arcs. */
    typedef EdgeIterPair ArcIterPair;


public:
    /** \brief Constructor. */
    GenPetriNet()
        : _numOfPos(0)
        , _numOfTrans(0)
    {

    }

    /** \brief Copy constructor */
    GenPetriNet(const GenPetriNet& other) : BaseGraph(other)
    {
    }

    /** \brief Virtual destructor. */
    virtual ~GenPetriNet()
    {
        BaseGraph::deleteGraph();
    }

    /** \brief Copy operator. */
    GenPetriNet& operator=(const GenPetriNet& other)
    {
        // implements copy-and-swap idiom
        GenPetriNet tmp(other);
        swap(tmp, *this);

        return *this;
    }

public:
    /** \brief Returns true if the given vertex data \a bundle determines a transition, false for position. */
    static bool isTransition(VertBundleCArg vertBundle)
    {
        return PnVertBundle_traits<TVertData>::isTrans(vertBundle);
    }

public:
    /** \brief Returns a ref to the underlying graph. */
    Graph& getGraph() { return BaseGraph::getGraph(); }

    /** \brief Const overloaded version of GenPetriNet::getGraph(). */
    const Graph& getGraph() const { return BaseGraph::getGraph(); }
public:
    //----<PN Interface through Graph interface>----

    /** \brief Return a number of all vertices (poss and transs) in the PN. */
    size_t getVerticesNum() const { return BaseGraph::getVerticesNum(); } 

    /** \brief Returns the number of positions. */
    size_t getPositionsNum() const { return _numOfPos;  }

    /** \brief Returns the number of transitions. */
    size_t getTransitionsNum() const { return _numOfTrans; }

    /** \brief Return a number of arcs in the PN. */
    size_t getArcsNum() const  { return BaseGraph::getEdgesNum(); }


    /** \brief Returns a collection of PN-casted vertices (by a pair of PN-vertex iterators). */
    inline PnVertIterPair getVertices() const
    {
        VertexIterPair vp = BaseGraph::getVertices();
        return std::make_pair(
            PnVertIter(this, vp.first, vp.second),
            PnVertIter(vp.second));
    }
    

    /** \brief Returns a pair of filter-iterators with a PN positions range. */
    inline PosIterPair getPositions() const
    {
        VertexIterPair vp = BaseGraph::getVertices();
        return std::make_pair(
            PosIter(this, vp.first, vp.second), 
            PosIter(vp.second));
    }


    /** \brief Returns a pair of filter-iterators with a PN transitions range. */
    inline TransIterPair getTransitions() const
    {
        VertexIterPair vp = BaseGraph::getVertices();
        return std::make_pair(
            TransIter(this, vp.first, vp.second),
            TransIter(vp.second));
    }
    


    // добавочка от 21.07.2018: пробуем обобщенные дуги вернуть
    inline BaseArcIterPair getArcs() const
    {
        EdgeIterPair pr = BaseGraph::getEdges();
        return std::make_pair(BaseArcIter(this, pr.first), BaseArcIter(pr.second));
    }

   
    inline PoTArcIterPair getOutArcs(Position p) const
    {
        OedgeIterPair pr = BaseGraph::getOutEdges(p.getVertex());
        return std::make_pair(PoTArcIter(this, pr.first), PoTArcIter(/*this, */pr.second));
    }
    
    inline ToPArcIterPair getOutArcs(Transition t) const
    {
        OedgeIterPair pr = BaseGraph::getOutEdges(t.getVertex());
        return std::make_pair(ToPArcIter(this, pr.first), ToPArcIter(/*this, */pr.second));
    }

    inline PTiArcIterPair getInArcs(Transition t) const 
    {
        IedgeIterPair pr = BaseGraph::getInEdges(t.getVertex());
        return std::make_pair(PTiArcIter(this, pr.first), PTiArcIter(/*this, */pr.second));
    }

    inline TPiArcIterPair getInArcs(Position p) const
    {
        IedgeIterPair pr = BaseGraph::getInEdges(p.getVertex());
        return std::make_pair(TPiArcIter(this, pr.first), TPiArcIter(/*this, */pr.second));
    }

    /** \brief Returns the numbers of input arcs of a vertex \a v. */
    inline size_t getInArcsNum(PnVertex v) const { return BaseGraph::getInEdgesNum(v._vert); }

    /** \brief Returns the numbers of output arcs of a vertex \a v. */
    inline size_t getOutArcsNum(PnVertex v) const { return BaseGraph::getOutEdgesNum(v._vert); }

    // для нетипизированных дуг — входной и выходной вертиксы

    inline PnVertex getSrcVert(const BaseArc& a) const
    {
        Vertex v = BaseGraph::getSrcVertex(a.edge);
        return PnVertex(v);
    }

    inline PnVertex getTargVert(const BaseArc& a) const
    {
        Vertex v = BaseGraph::getTargVertex(a.edge);
        return PnVertex(v);
    }

    // для PT-дуг — входная позиция и выходная транзиция
    inline Position getSrcPos(const PTArc& a) const
    {
        Vertex v = BaseGraph::getSrcVertex(a.edge);
        return Position(v);
    }

    // для PT-дуг — входная позиция и выходная транзиция
    inline Transition getTargTrans(const PTArc& a) const
    {
        Vertex v = BaseGraph::getTargVertex(a.edge);
        return Transition(v);
    }

    // для TP-дуг — входная транзиция и выходная позиция
    inline Transition getSrcTrans(const TPArc& a) const
    {
        Vertex v = BaseGraph::getSrcVertex(a.edge);
        return Transition(v);
    }

    // для TP-дуг — входная транзиция и выходная позиция
    inline Position getTargPos(const TPArc& a) const
    {
        Vertex v = BaseGraph::getTargVertex(a.edge);
        return Position(v);
    }


    InPosIterPair getPreset(Transition t) const
    {
        // входные дуги от входных (искомых) позиций к данной транзиции
        PTiArcIterPair inArcs = getInArcs(t);           
        return std::make_pair(InPosIter(this, inArcs.first), InPosIter(inArcs.second));
    }


    OutPosIterPair getPostset(Transition t) const
    {
        // выходные дуги от данной транзиции к выходным (искомым) позициям 
        ToPArcIterPair outArcs = getOutArcs(t);
        return std::make_pair(OutPosIter(this, outArcs.first), OutPosIter(outArcs.second));
    }



    InTransIterPair getPreset(Position p) const
    {
        // входные дуги от входных (искомых) транзиций к данной позиции
        TPiArcIterPair inArcs = getInArcs(p);
        return std::make_pair(InTransIter(this, inArcs.first), InTransIter(inArcs.second));
    }


    OutTransIterPair getPostset(Position p) const
    {
        // выходные дуги от данной позиции к выходным (искомым) транзициям
        PoTArcIterPair outArcs = getOutArcs(p);
        return std::make_pair(OutTransIter(this, outArcs.first), OutTransIter(outArcs.second));
    }



#pragma region PN Objects Manipulation


    /** \brief Adds a new PN position and returns a wrapper over a newly added BGL vertex. */
    Position addPosition()
    {
        return Position::addPositionTo(*this);
    }

    /** \brief Adds a new PN transition and returns a wrapper over a newly added BGL vertex. */
    Transition addTransition()
    {
        return Transition::addTransitionTo(*this);
    }

    /** \brief Adds a new arc from a source position \a srcP to a destination 
     * transition \a targT. If \a inhib is true, sets inhibitor flag for the arc.
     */
    PTArc addArc(Position srcP, Transition targT, ArcType at = ArcType::atReg)
    {
        // т.к. мы честно упрятали всю логику пометок позиций/переходов в конструкторы
        // и сделали статическую типизацию тем самым, тут можно ничего уже и не проверять!
        Edge edge = BaseGraph::addEdge(srcP._vert, targT._vert);
        PnArcBundle_traits<TArcData>::setArcType(getGraph()[edge], at);
        return PTArc(edge);
    }

    /** \brief Adds a new arc from a source transition \a srcT to a destination 
     * position \a targP. Cannot be inhibitor, only regular one!.
     */
    TPArc addArc(Transition srcT, Position targP) //, ArcType at = ArcType::atReg)
    {
        // см. замечание к аналогичному методу
        Edge edge = BaseGraph::addEdge(srcT._vert, targP._vert);
        PnArcBundle_traits<TArcData>::setArcType(getGraph()[edge], ArcType::atReg);
        return TPArc(edge);
    }


    /** \brief Looks over all parallel arcs between position \a srcP and 
     *  transition \a targT. If such an arc exists,
     *  returns it, otherwise creates a new one and returns it.
     */
    PTArc getOrAddArc(Position srcP, Transition targT, ArcType at = ArcType::atReg)
    {
        // сперва ищем существующую
        PTArcRes arc = getArc(srcP, targT, at);
        if (arc.second)                      // есть такая
            return arc.first;

        // иначе — добавляем новую
        PTArc newArc = addArc(srcP, targT, at);

        return newArc;
    }


    /** \brief Looks over all parallel arcs between position \a srcP and 
     *  transition \a targT. If such an arc exists,
     *  returns it, otherwise creates a new one and returns it.
     */
    TPArc getOrAddArc(Transition srcT, Position targP)
    {
        // сперва ищем существующую
        TPArcRes arc = getArc(srcT, targP);
        if (arc.second)                      // есть такая
            return arc.first;

        // иначе — добавляем новую
        TPArc newArc = addArc(srcT, targP);

        return newArc;
    }


    /** \brief Clears all output arcs of the given vertex \a v. */
    inline void clearOutArcs(PnVertex v)
    {
        BaseGraph::clearOutEdges(v._vert);
    }

    /** \brief Clears all input arcs of the given vertex \a v. */
    inline void clearInArcs(PnVertex v)
    {
        BaseGraph::clearInEdges(v._vert);
    }

    /** \brief Clears all (input and output) arcs of the given vertex \a v. */
    inline void clearArcs(PnVertex v)
    {
        BaseGraph::clearEdges(v._vert);
    }

    /** \brief Removes the given arc \a a. 
     *  
     *  Does not check whether the given arc exists or not.
     */
    inline void removeArc(const BaseArc& a)
    {
        BaseGraph::removeEdge(a.edge);
    }


    /** \brief Removes the given vertex \a v and all its (output and input) arcs. */
    void removeVertex(PnVertex v)
    {
        // #appnote: в более интересных версиях этого метода в производных классах
        // нужно также не забыть поухаживать за данными, ассоциированными с этим
        // вертексом

        clearArcs(v);                       // очищаем все дуги (требование!)
        BaseGraph::removeVertex(v._vert);         // и саму вершину напоследок
    }

#pragma endregion // PN Objects Manipulation
    /** \brief Looks over all parallel arcs between position \a srcP and 
     *  transition \a targT. If such an arc exists,
     *  returns true as a second part of a returning value, otherwise returns false there.
     *  If more than one arc exist, returns the very first.
     */
    PTArcRes getArc(Position srcP, Transition targT, ArcType at = ArcType::atReg)
    {
        //PoTArcIterPair getOutArcs

        // перебираем все выходные дуги данной позиции srcP
        PoTArcIter tCur, tEnd;
        for (boost::tie(tCur, tEnd) = getOutArcs(srcP); tCur != tEnd; ++tCur)
        {
            const PTArc& arc = *tCur;
            
            if(getTargTrans(arc) == targT && getArcType(arc) == at)
                return std::make_pair(arc, true);
        }

        // если вышли из цикла, значит нет такой
        return std::make_pair(PTArc(), false);
    }

    /** \brief Looks over all parallel arcs between position \a srcT and 
     *  transition \a targP. If such an arc exists,
     *  returns true as a second part of a returning value, otherwise returns false there.
     *  If more than one arc exist, returns the very first.
     */
    TPArcRes getArc(Transition srcT, Position  targP)
    {
        // перебираем все выходные дуги данной транзиции srcT
        ToPArcIter tCur, tEnd;
        for (boost::tie(tCur, tEnd) = getOutArcs(srcT); tCur != tEnd; ++tCur)
        {
            const TPArc& arc = *tCur;               
            
            if(getTargPos(arc) == targP)
                return std::make_pair(arc, true);           
        }

        // если вышли из цикла, значит нет такой
        return std::make_pair(TPArc(), false);
    }

#pragma region PN Objects Properties 

    /** \brief Returns the type of the given arc \a a. */
    //inline ArcType getArcType(Arc a) const
    inline ArcType getArcType(const BaseArc& a) const    
    {
        return PnArcBundle_traits<TArcData>::getArcType(getGraph()[a.edge]);
    }

    /** \brief Sets the type of the given arc \a a. 
     * 
     *  Available only for PT-arcs due to its ipossible to have inhibitor TP arcs.
     */
    inline void setArcType(PTArc a, ArcType at)
    {
        return PnArcBundle_traits<TArcData>::setArcType(getGraph()[a.edge], at);
    }

#pragma endregion // PN Objects Properties


protected:
    //----<Construction Helpers>----

    /** \brief Swap method for copy operator. */
    static void swap(GenPetriNet& lhv, GenPetriNet& rhv) // noexcept;   // VC12 does not know about this clause...
    {
        BaseGraph::swap(lhv, rhv);
    }

protected:
    // special data fields
    size_t _numOfPos;                               ///< Stores a number of positions.
    size_t _numOfTrans;                             ///< Stores a number of transitions.
}; // class GenPetriNet


//==============================================================================
// class GenPetriNetMarking
//==============================================================================


/** \brief Marking class for generic Petri Nets.
 *
 *  -
 */
template<
    typename TVertData = boost::no_property,        // Данные на позициях/переходах (можно только один общий тип!).
    typename TArcData = boost::no_property          // Данные на дугах.
>
class GenPetriNetMarking {
public:
    //-----<Types>-----
    //typedef unsigned int UInt;                          ///< Alias for unsigned int.

    //typedef typename GenPetriNet<TVertData, TArcData>::Uint UInt; ///< Alias for unsigned int.
    typedef typename GenPetriNet<TVertData, TArcData> PN;    ///< Corresponding Petrinet type.
    typedef typename PN::Position  Position;               ///< Alias for PN position type.
    typedef std::map<Position, UInt> PosNums;        ///< Mapping "position-to-int-numbers".

    /** \brief Operator for checking a marking in the given position \a pos. 
     * 
     *  Recommended to be used instead of operator[] wherever it is possible.
     */
    UInt operator()(Position pos) const
    {
        PosNums::const_iterator it = _marking.find(pos);
        if (it == _marking.end())
            return 0;

        return it->second;
    }

    /** \brief Used for setting a marking to the given position \a pos.
     *
     *  Can also be used to retrieve a marking for the position, but it is recommended to use operator()
     *  instead because that operator does not create a new 0-node in the mapping.
     */
    UInt& operator[](Position pos)
    {
        return _marking[pos];
    }


    /** \brief Const overloaded version of the operator[]. */
    UInt operator[](Position pos) const
    {
        return operator()(pos);
    }

    inline void clear()
    {
        _marking.clear();
    }

    /// Provides a r/o access to the internal position-numbers mapping.
    const PosNums& getMap() const { return _marking; }

protected:

    /** \brief Marking as a number of tokens for some positions. */
    PosNums _marking;   
}; // class GenPetriNetMarking



}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_PN_MODELS_GEN_PETRINET_H_
