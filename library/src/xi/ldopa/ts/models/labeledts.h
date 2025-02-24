////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Transision Systems Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      14.09.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Definitions for creating types based on labeled transion systems.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TRSS_MODELS_LABELEDTS_H_
#define XI_LDOPA_TRSS_MODELS_LABELEDTS_H_

#pragma once


// boost
#include <boost/iterator/filter_iterator.hpp>

#include <boost/blank.hpp>

// ldopa
#include "xi/ldopa/graphs/boost/bgl_graph_wrappers.h"

// std
#include <set>
#include <map>

namespace xi { namespace ldopa { namespace ts {;   //

#pragma region StateID and State Traits

/** \brief Defines a template for TS StateID traits classes. */
template<typename TStateID>
class StateID_traits {
public:
    typedef         TStateID    TVal;       ///< Value type.
    typedef const   TStateID&   TСArg;      ///< Const argument type.
    //typedef         TStateID&   TArg;      ///< Argument type.
    typedef         TStateID&   TRef;      ///< Ref-argument type.
    typedef const   TStateID&   TCRes;      ///< Constant res.
    //typedef TElement&   TRet;               ///< Default return type: reference.
}; // class StateID_traits



/** \brief Base class for all StateID traits. */
template<typename TStateID>
struct StateProperty_traits_base 
{
    /** \brief StateID as a const argument of a method. */
    typedef typename StateID_traits<TStateID>::TСArg StateIDCArg;

    ///** \brief StateID as an argument of a method. */
    //typedef typename StateID_traits<TStateID>::TArg StateIDArg;

    /** \brief StateID as a ref-argument of a method. */
    typedef typename StateID_traits<TStateID>::TRef StateIDRef;

    /** \brief StateID as a returning result of a method. */
    typedef typename StateID_traits<TStateID>::TCRes StateIDCRes;
};

/** \brief Determines some state property types and helper method depending on given 
 *  StateProperty template.
 */
template<typename TStateID, typename TStateProperty>
struct StateProperty_traits : public StateProperty_traits_base<TStateID>
{
    /** \brief Default behavior is to embed both an ID and a custom property. */
    struct _StateProperty
    {
        TStateID id;
        TStateProperty prop;
    };

    /** \brief Defines a type for bundle property associated with TS state. */
    typedef _StateProperty StPropBundle;

    /** \brief Defines a type for StateProperty as a const parameter of a method. */
    typedef const   _StateProperty& StPropBundleCArg;

    /** \brief Defines a type for StateProperty as a const res of a method. */
    typedef const   _StateProperty& StPropBundleCRes;

    /** \brief Defines a type for StateProperty as a parameter of a method. */
    typedef         _StateProperty& StPropBundleRef;

    /** \brief Type for a reference to a custom data object. */
    typedef TStateProperty& StDataRes;
    //typedef typename Data_traits<TStateID>::TRes StDataRes;


    /** \brief Function extracts a state ID from the complex property. */
    static StateIDCRes getID(StPropBundleCArg stp) { return stp.id; }

    /** \brief Sets a state id. */
    static void setID(StPropBundleRef sp, StateIDCArg id) { sp.id = id; }

    /** \brief Returns a ref to a custom data object associated with the given bundle. */
    static StDataRes getData(StPropBundleRef stp) { return stp.prop; }
}; 



/** \brief Special variant when no custom property is defined. */
template<typename TStateID>
struct StateProperty_traits<TStateID, boost::no_property> : 
    public StateProperty_traits_base<TStateID>
{
    /** \brief Redefines a StateProperty type. */
    typedef TStateID StPropBundle;

    /** \brief Redefines a type for StateProperty as a parameter of a method. */
    typedef StateIDCArg StPropBundleCArg;


    /** \brief Defines a type for StateProperty as a const res of a method. */
    typedef StateIDCRes StPropBundleCRes;

    /** \brief Redefines a type for StateProperty as a parameter of a method. */
    typedef StateIDRef StPropBundleRef;

    /** \brief Type for a reference to a custom data object. For boost::no_property type,
     *  results is returnd by value.
     */
    typedef typename boost::no_property StDataRes;

    /** \brief Function returns a state ID, which the property is itself. */
    static StateIDCRes getID(StPropBundleCArg stp) { return stp; }

    /** \brief Sets a state id. */
    static void setID(StPropBundleRef sp, StateIDCArg id) { sp = id; }

    /** \brief Returns a ref to a custom data object associated with the given bundle. */
    static StDataRes getData(StPropBundleRef stp) { return boost::no_property(); }// _noProp; }

protected:
}; //

#pragma endregion // StateID and State Traits


#pragma region Label and Transition Traits


/** \brief Defines a template for TS TLabel traits classes. */
template<typename TLabel>
class Label_traits {
public:
    typedef         TLabel      TVal;       ///< Value type.
    typedef const   TLabel&     TСArg;      ///< Argument type.
    typedef         TLabel&     TRef;       ///< Ref-argument type.
    typedef const   TLabel&     TCRes;      ///< Constant res.
};



/** \brief Base class for all Label traits. */
template<typename TLabel>
struct TransProperty_traits_base 
{
    /** \brief Label as a const argument of a method. */
    typedef typename Label_traits<TLabel>::TСArg LabelCArg;

    /** \brief Label as a ref-argument of a method. */
    typedef typename Label_traits<TLabel>::TRef LabelRef;

    /** \brief Label as a returning result of a method. */
    typedef typename Label_traits<TLabel>::TCRes LabelCRes;
};


/** \brief Determines some additional label property types and helper method depending on given
 *  TransProperty template.
 */
template<typename TLabel, typename TTransProperty>
struct TransProperty_traits : public TransProperty_traits_base<TLabel>
{
    /** \brief Default behavior is to embed both a label and a custom property. */
    struct _TransProperty
    {
        TLabel lbl;
        TTransProperty prop;
    };

    /** \brief  Defines a type for bundle property associated with TS transition. */
    typedef _TransProperty TrPropBundle;

    /** \brief Defines a type for Transition Prop Bundle as a const parameter of a method. */
    typedef const   _TransProperty& TrPropBundleCArg;

    /** \brief Defines a type for Transition Prop Bundle as a const res of a method. */
    typedef const   _TransProperty& TrPropBundleCRes;

    /** \brief Defines a type for Transition Prop Bundle as a parameter of a method. */
    typedef         _TransProperty& TrPropBundleRef;

    /** \brief Type for a reference to a custom data object. */
    typedef TTransProperty& TrDataRes;


    // Functions

    /** \brief Function extracts a trans label from the complex property. */
    static LabelCRes getLbl(TrPropBundleCArg trp) { return trp.lbl; }

    /** \brief Sets a label of the trans. */
    static void setLbl(TrPropBundleRef trp, LabelCArg lbl) { trp.lbl = lbl; }

    /** \brief Returns a ref to a custom data object associated with the given bundle. */
    static TrDataRes getData(TrPropBundleRef trp) { return trp.prop; }
}; // class TransProperty_traits


/** \brief Special variant when no custom property is defined. */
template<typename TLabel>
struct TransProperty_traits<TLabel, boost::no_property> :
    public TransProperty_traits_base<TLabel>
{
    /** \brief  Defines a type for bundle property associated with TS transition. */
    typedef TLabel TrPropBundle;

    /** \brief Defines a type for Transition Prop Bundle as a const parameter of a method. */
    typedef LabelCArg TrPropBundleCArg;

    /** \brief Defines a type for Transition Prop Bundle as a const res of a method. */
    typedef LabelCRes TrPropBundleCRes;

    /** \brief Defines a type for Transition Prop Bundle as a parameter of a method. */
    typedef LabelRef TrPropBundleRef;

    /** \brief Type for a reference to a custom data object. For boost::no_property type,
     *  results is returned by value.
     */
    typedef typename boost::no_property TrDataRes;


    // Functions

    /** \brief Function extracts a trans label from the complex property. */
    static LabelCRes getLbl(TrPropBundleCArg trp) { return trp; }

    /** \brief Sets a label of the trans. */
    static void setLbl(TrPropBundleRef trp, LabelCArg lbl) { trp = lbl; }

    /** \brief Returns a ref to a custom data object associated with the given bundle. */
    static TrDataRes getData(TrPropBundleRef trp) { return boost::no_property(); }

}; //

#pragma endregion //Label and Transition Traits


#pragma region BGL Traits Classes

/** \brief xi traits base class for some details of BGL class. */
template<typename TGraph>
struct BGL_traits_base
{
    /** \brief State type alias for graph Vertex. */
    typedef typename boost::graph_traits<TGraph>::vertex_descriptor State;
    //typedef Graph::Vertex State;

    /** \brief Transition type alias for graph Edge. */
    typedef typename boost::graph_traits<TGraph>::edge_descriptor Transition;

    typedef typename TGraph::out_edge_list_selector OutEdgeListS;
};


/** \brief xi traits class for some details of BGL class. */
template<typename TGraph, typename TOutEdgeListS = TGraph::out_edge_list_selector >
struct BGL_traits : public BGL_traits_base<TGraph>
{
};

/** \brief Variant for the list selector as OutEdge. */
template<typename TGraph>
struct BGL_traits<TGraph, boost::listS> : 
    public BGL_traits_base<TGraph>
{
    static const int a = 1;
};

/** \brief Variant for the multiset selector as OutEdge. */
template<typename TGraph>
struct BGL_traits<TGraph, boost::multisetS> :
    public BGL_traits_base<TGraph>
{
    static const int a = 2;
};

#pragma endregion // BGL Traits Classes


/** \brief Template base class for labeled transition systems. Primarily defines types.
 *
 *  \tparam TStateID is a type for representing (IDs of) states.
 *  \tparam TLabel is a type for representing labels for the TS.
 *  \tparam TStateEqual is a comparator for checking equality of two states.
 *  \tparam TStateProperty defines some additional vertex properties to store
 *  by being ebmedded into internal vertex propertis.
 *
 *  TS is internally based on BGL bidigraph class.
 *
 *  As defined here, LTS is used the label type for labeling transitions only.
 *  If one need to label states or add other similar semantics, use this class as a
 *  base, possibly use Void (struct {}) type as TLabel if there is no need
 *  to label transitions.
 *
 *  By definition, each transition must be labelled, so the mapping of transitions
 *  to labels are totally defined.
 *
 *  Зачем мы отдельно выносим свойства состояний и свойства переходов (от TStateID и TLabel
 *  соответственно? Вкладываемые свойства имеют принципиально иную окраску: к ним не применимы
 *  требования об уникальности (идентификатора) и несколько параллельных дуг между парой вершин
 *  (u, v) могут быть помечены одинаковыми свойствами TTransProperty, но не одинаковыми
 *  пометками TLabel.
 */
template<
    typename TStateID,
    typename TLabel,

    // additional properties
    typename TStateProperty = boost::no_property,
    typename TTransProperty = boost::no_property
>
class BaseLabeledTS
{
public:
    /** \brief Type pack for embedding as graph vertices property. */
    typedef typename StateProperty_traits<TStateID, TStateProperty>::StPropBundle
        StPropBundle;

    /** \brief Type pack for embedding as graph vertices property. */
    typedef typename TransProperty_traits<TLabel, TTransProperty>::TrPropBundle
        TrPropBundle;

    /** \brief Defining a type for the underlying graph.
     *
     *  The most newest approach is to use BGD directly for making a most appropriate type.
     */
    typedef boost::adjacency_list<
        boost::listS,               // out edge storage
        boost::listS,               // vertices storage
        boost::bidirectionalS,      // bi-directional      
        // properties
        StPropBundle,               // vertex properties
        TrPropBundle
        //TLabel                    // edge properties
    > Graph;
};



/** \brief Template class for labeled transition systems.
 *
 *  \tparam TStateID is a type for representing (IDs of) states.
 *  \tparam TLabel is a type for representing labels for the TS.
 *  \tparam TStateEqual is a comparator for checking equality of two states.
 *  \tparam TStateProperty defines some additional vertex properties to store
 *  by being ebmedded into internal vertex propertis.
 *
 *  TS is internally based on BGL bidigraph class.
 *  
 *  As defined here, LTS is used the label type for labeling transitions only.
 *  If one need to label states or add other similar semantics, use this class as a
 *  base, possibly use Void (struct {}) type as TLabel if there is no need 
 *  to label transitions.
 *  
 *  By definition, each transition must be labelled, so the mapping of transitions
 *  to labels are totally defined.
 *  
 *  UPD. Since 11/01/18 TS allows to have so-called anonymous transitions. Such 
 *  transitions are represented by regular graph vertices, but doesn't have IDs.
 *  So, they are managed by special methods and tracked respectively (by a special set).
 *
 */
template<
    typename TStateID, 
    typename TLabel,

    // additional properties
    typename TStateProperty = boost::no_property,
    typename TTransProperty = boost::no_property//,
>
class LabeledTS : 
    public BaseLabeledTS<TStateID, TLabel, TStateProperty, TTransProperty>, 
    protected gr::BoostBidiGraphP<typename BaseLabeledTS<TStateID, TLabel, TStateProperty, TTransProperty>::Graph>
{
public:
    //-----<Traits types>-----
    
#pragma region State Traits Types
    // State Properties

    /** \brief StateID as a const argument of a method. */
    typedef typename StateID_traits<TStateID>::TСArg StateIDCArg;

    /** \brief StateID as a const argument of a method. */
    typedef typename StateID_traits<TStateID>::TCRes StateIDCRes;

    /** \brief Type for vertix property const argument. */
    typedef typename StateProperty_traits<TStateID, TStateProperty>::StPropBundleCArg
        StPropBundleCArg;

    /** \brief Type for vertix property const (ref) result. */
    typedef typename StateProperty_traits<TStateID, TStateProperty>::StPropBundleCRes
        StPropBundleCRes;    

    /** \brief Type for vertix property ref-argument. */
    typedef typename StateProperty_traits<TStateID, TStateProperty>::StPropBundleRef
        StPropBundleRef;

    /** \brief Typename for returning result (ref) for getData() method. */
    typedef typename StateProperty_traits<TStateID, TStateProperty>::StDataRes StDataRes;

#pragma endregion // State Traits Types

#pragma region Transition Traits Types
    // Transition Properties

    /** \brief StateID as a const argument of a method. */
    typedef typename Label_traits<TLabel>::TСArg LabelCArg;

    /** \brief StateID as a const argument of a method. */
    typedef typename Label_traits<TLabel>::TCRes LabelCRes;

    /** \brief Type for vertix property const argument. */
    typedef typename TransProperty_traits<TLabel, TTransProperty>::TrPropBundleCArg
        TrPropBundleCArg;

    /** \brief Type for vertix property const (ref) result. */
    typedef typename TransProperty_traits<TLabel, TTransProperty>::TrPropBundleCRes
        TrPropBundleCRes;

    /** \brief Type for vertix property ref-argument. */
    typedef typename TransProperty_traits<TLabel, TTransProperty>::TrPropBundleRef
        TrPropBundleRef;

    /** \brief Typename for returning result (ref) for getData() method. */
    typedef typename TransProperty_traits<TLabel, TTransProperty>::TrDataRes TrDataRes;

#pragma endregion //Transition Traits Types 

    //----<Custom data properties>----

    /** \brief Alias for a base graph wrapper type. */
    typedef gr::BoostBidiGraphP<BaseLabeledTS<TStateID, TLabel, 
        TStateProperty, TTransProperty>::Graph>
            BaseGraph;


    /** \brief State type alias for graph Vertex. */
    typedef typename Vertex State;          // BoostGraphP

    /** \brief Transition type alias for graph Edge. */
    typedef typename Edge Transition;

    /** \brief Iterator for states modeled by vertices of a graph. */
    typedef typename VertexIter StateIter;

    /** \brief Iterator for transitions modeled by output edges of a graph. */
    typedef typename OedgeIter OtransIter;

    /** \brief Iterator for transitions modeled by input edges of a graph. */
    typedef typename IedgeIter ItransIter;

    /** \brief Iterator for transitions modeled by edges of a graph. */
    typedef typename EdgeIter TransIter;

    /** \brief A pair of state iterators, which represents a collection of states. */
    typedef VertexIterPair StateIterPair;

    /** \brief A pair of transition iterators, which represents a collection of output edges. */
    typedef OedgeIterPair OtransIterPair;

    /** \brief A pair of transition iterators, which represents a collection of input edges. */
    typedef IedgeIterPair ItransIterPair;

    /** \brief A pair of transition iterators, which represents a collection of edges. */
    typedef EdgeIterPair TransIterPair;

    //----<Filter Iterator Predicates>----
    
    /** \brief Define a predicate that filters a destination state. */
    typedef TargetVertexFilter TargetStateFilter;

    /** \brief Filter iterator for dealing with only those transition, which have 
     *  a given target state.
     */
    typedef TargVertexEdgeIter TargStateTransIter;

    /** \brief A pair of transition iterators, which represents a collection of edges. */
    typedef std::pair<TargStateTransIter, TargStateTransIter> TargStateTransIterPair;
    // тж. возможно typedef TargVertexEdgeIterPair TargStateTransIterPair;

    /** \brief Type alias for state/vertex returning result with some helper methods. */
    typedef std::pair<State, bool> StateRes;

    /** \brief Type alias for edge/transition returning result. 
     * 
     *  Normally, if the second part of this pair is true, then the result
     *  is presented (found, exists etc), otherwise it is false.
     */
    typedef std::pair<Transition, bool> TransRes;

    /** \brief Declares a set of states, mostly for defining anon states. */
    typedef std::set<State> StateSet;

    // т.к. теперь вкладываем состояние непосредственно в вершины, необходимости
    // в двойной мапе нет

    /** \brief A more simple map for looking for a state by an id. */
    typedef std::map<TStateID, State> StateIDStateMap;

    /** \brief Pair of element for inserting in a StateIDStateMap object. */
    typedef typename StateIDStateMap::value_type  SISPair;          //SVBPair;
    
    /** \brief Const iterator for traversing a StateIDStateMap object. */
    typedef typename StateIDStateMap::const_iterator SISCIter;

    /** \brief Using for mapping transitions onto labels. */
    typedef std::map<Transition, TLabel> TransLabelsMap;

public:
    /** \brief Constructor. */
    LabeledTS()
    {
        createObjects();
    }

    /** \brief Copy constructor */
    LabeledTS(const LabeledTS& other) : BaseGraph(other)//BaseGraph(other.getGraph())
    {
        // копированием графа теперь занимается родительский КК, а здесь только доиндексируем мапу идентификаторов
        initStID2VertMap();
    }

    /** \brief Virtual destructor. */
    virtual ~LabeledTS() 
    {
        delete _stid2verts;
        delete _anonStates;         // ADDED: 11/01/2018
        BaseGraph::deleteGraph();
        //delete _gr;
    }

    /** \brief Copy operator. */
    LabeledTS& operator=(const LabeledTS& other)
    {
        // implements copy-and-swap idiom
        LabeledTS tmp(other);
        swap(tmp, *this);

        return *this;
    }
protected:    
public:
    //----<Static Traits Helpers>----
    
    /** \brief Extracts a state ID from a complex state property. */
    static StateIDCRes extractStateID(StPropBundleCArg stp)
    {
        return StateProperty_traits<TStateID, TStateProperty>::getID(stp);
    }

    /** \brief Places an ID inside state property whatever it is... */
    static void emplaceStateID(StPropBundleRef sp, StateIDCArg id)
    {
        StateProperty_traits<TStateID, TStateProperty>::setID(sp, id);
    }

    /** \brief Returns a ref to custom data that is associated with the state. */
    static StDataRes extractStateData(StPropBundleRef sp)
    {
        return StateProperty_traits<TStateID, TStateProperty>::getData(sp);
    }

    /** \brief Extracts a transition label from a complex transition property. */
    static LabelCRes extractTransLbl(TrPropBundleCArg trp)
    {
        return  TransProperty_traits<TLabel, TTransProperty>::getLbl(trp);
    }

    /** \brief Places a label inside labl property whatever it is... */
    static void emplaceTransLbl(TrPropBundleRef trp, LabelCArg lbl)
    {
        TransProperty_traits<TLabel, TTransProperty>::setLbl(trp, lbl);
    }

    /** \brief Returns a ref to custom data that is associated with the transition. */
    static TrDataRes extractTransData(TrPropBundleRef trp)
    {
        return TransProperty_traits<TLabel, TTransProperty>::getData(trp);
    }

public:
    /** \brief Returns a ref to the underlying graph. */
    Graph& getGraph() { return BaseGraph::getGraph(); }

    /** \brief Const overloaded version of LabeledTS::getGraph(). */
    const Graph& getGraph() const { return BaseGraph::getGraph(); }

public:
    //----<TS Interface through Graph interface>----

    /** \brief Return a number of states in the TS. */
    size_t getStatesNum() const { return BaseGraph::getVerticesNum(); } //{ return _gr.getVerticesNum(); }

    /** \brief Return a number of regular states in the TS. */
    size_t getRegStatesNum() const { return _stid2verts->size(); }

    /** \brief Return a number of anonymous states in the TS. */
    size_t getAnonStatesNum() const { return _anonStates->size(); }

    /** \brief Return a number of edges in the TS. */
    size_t getTransitionsNum() const  { return BaseGraph::getEdgesNum(); }//{ return _gr.getEdgesNum(); }

    /** \brief Returns a collection of States (by a pair of state iterators). */
    inline StateIterPair getStates()
    {
        return BaseGraph::getVertices();
    }

    /** \brief Returns a collection of States (by a pair of state iterators)/Const overload. */
    inline StateIterPair getStates() const { return BaseGraph::getVertices(); }


    /** \brief Returns a collection of Transitions (by a pair of state iterators). */
    inline TransIterPair getTransitions()
    {
        return BaseGraph::getEdges();
    }

    /** \brief Returns a collection of Transitions (by a pair of state iterators) / const. */
    inline TransIterPair getTransitions() const { return BaseGraph::getEdges(); }

    /** \brief Returns a collection of Input Transitions of a state \a s. */
    inline ItransIterPair getInTransitions(State s)
    {
        return BaseGraph::getInEdges(s);
        //return getGraphInEdges(s);
    }

    /** \brief Returns a collection of Input Transitions of a state \a s (const). */
    inline ItransIterPair getInTransitions(State s) const { return BaseGraph::getInEdges(s); }

    /** \brief Returns a collection of Output Transitions of a state \a s. */
    inline OtransIterPair getOutTransitions(State s)
    {
        return BaseGraph::getOutEdges(s);
        //return getGraphOutEdges(s);
    }

    /** \brief Returns a collection of Output Transitions of a state \a s (const). */
    inline OtransIterPair getOutTransitions(State s) const { return BaseGraph::getOutEdges(s); }

    /** \brief For a given state ID \a id, returns a state, which is a vertex in 
     *  the underlying graph.
     *  If no vertex exists, a new one with the given id is added and returned.
     */
    //State getOrAddStateById(StateIDCArg id)
    State getOrAddState(StateIDCArg id)
    {
        SISCIter it = _stid2verts->find(id);
        if (it != _stid2verts->end())               // found one
            return it->second;

        // no found
        return addStateInternal(id);
    }

    /** \brief Looks for a state with id \a id and returns it if exists.
     *
     *  If a state exists, sets a second field of StateRes to true, otherwise sets it to false.
     */
    StateRes getState(StateIDCArg id)
    {
        SISCIter it = _stid2verts->find(id);
        if (it != _stid2verts->end())               // found one
            return std::make_pair(it->second, true);

        return std::make_pair(State(), false);          // not found
    }

    /** \brief Adds a new anonymous state to the TS.
     *
     *  Anonymous states don't have emplaced IDs and are tracked by a special set.
     */
    State addAnonState()
    {
        State v = BaseGraph::addVertex();             // новую вершину в граф
        _anonStates->insert(v);

        return v;
    }

    /** \brief Returns true if a given state \a s is anonymous one, false otherwise (regular one). */
    bool isStateAnon(State s) const
    {
        return (_anonStates->find(s) != _anonStates->end());
    }

    /** \brief Returns true if a given state \a s is regular one, false otherwise (anon one). */
    bool isStateRegular(State s) const
    {
        return !isStateAnon(s);
    }

    /** \brief Extracts a transition label from a complex transition property
     *  through the given transition.
     */
    LabelCRes extractTransLbl(const Transition& t)
    {
        TrPropBundleCRes trBundle = getBundle(t);
        return  TransProperty_traits<TLabel, TTransProperty>::getLbl(trBundle);
    }

    /** \brief Looks over all parallel transitions between pair of states \a s and \a t
     *  for one labeled with the given label \a lbl. If such a transition exists,
     *  returns true as a second part of a returning value, otherwise returns false there.
     */
    TransRes getTrans(State s, State t, LabelCArg lbl)
    {
        // новый подход от 16.02.2018
        // перебираем все выходные транзиции данного состояния s
        OtransIter tCur, tEnd;
        for (boost::tie(tCur, tEnd) = getOutTransitions(s); tCur != tEnd; ++tCur)
        {
            // сперва проверяем, помечена ли она заданной меткой
            // проверяем ПЕРЕД тем, как таргет смотрим, т.к. исходим из того, что чаще детерм. система
            LabelCRes cLbl = extractTransLbl(*tCur);             // обновил 10.01.2018
            if (cLbl == lbl)
            {
                // пометка есть, проверим таргетное состояние
                if (getTargState(*tCur) == t)
                    return std::make_pair(*tCur, true);
            }
        }

        // если вышли из цикла, значит нет такой
        return std::make_pair(Transition(), false);
    }

    /** \brief Looks over all output transitions of a given state \a s for the first 
     *  one, which is labeled by the label \a lbl.
     *
     *  \returns If at least one such a transition if found, returns its (arbitrary first).
     */
    TransRes getFirstOutTrans(State s, LabelCArg lbl)
    {
        // перебираем все выходные транзиции данного состояния s
        OtransIter tCur, tEnd;
        for (boost::tie(tCur, tEnd) = getOutTransitions(s); tCur != tEnd; ++tCur)
        {
            // т.к. свойство, навешенное на транзицию, м.б. составное, необходимо хитро извлекать метку
            //TrPropBundleCRes trBundle = getBundle(*tCur);       // см. тж. getTrans             
            LabelCRes trLbl = extractTransLbl(*tCur);
            if (lbl == trLbl)
                return TransRes(*tCur, true);
        }

        // если же ничего не найдено
        return TransRes(Transition(), false);
    }


    /** \brief Acts like getFirstOutTrans(), but returns the state, to which a found transition lead.
     *
     *  It is just a shortcut for a chain of methods getFirstOutTrans() and getTargState().
     *  \returns if a corresponding transition found, returns a corresponding state and true as a second parameter.
     */
    StateRes getFirstOutState(State s, LabelCArg lbl)
    {
        TransRes trRes = getFirstOutTrans(s, lbl);
        if (!trRes.second)
            return StateRes(State(), false);
       
        // если найдено
        return StateRes(getTargState(trRes.first), true);
    }


    /** \brief Gets a transition between given pair of states \a s and \a t, 
     *  marked by a label \a lbl.
     *
     *  If no transition exists, creates a new one.
     */
    Transition getOrAddTrans(State s, State t, LabelCArg lbl)
    {
        // сперва ищем существующую
        TransRes tr = getTrans(s, t, lbl);
        if (tr.second)                      // есть такая
            return tr.first;

        // иначе — добавляем новую
        Transition newTr = addTransitionInternal(s, t, lbl);

        return newTr;
    }

    /** \brief Adds a new unlabeled (anonymous) transition between two states.
     *
     *  Method always creates a new arc and returns it.
     *  The actual label value for the added transition is undefined.
     *  In order to track all anonymous transitions, there can be
     *  a special set in derived classes defined.
     */
    Transition addAnonTrans(State s, State t)
    {
        Transition trans = BaseGraph::addEdge(s, t);
        //emplaceTransLbl(getGraph()[trans], lbl);      // вот тут что будет, то будет
        return trans;
    }


    /** \brief Returns the source State (vertex) of the given transitions \param t. */
    State getSrcState(const Transition& t)
    {
        return BaseGraph::getSrcVertex(t);
        //return boost::source(t, getGraph());
    }

    /** \brief Returns the target State (vertex) of the given transitions \param t. */
    State getTargState(const Transition& t)
    {
        return BaseGraph::getTargVertex(t);
        //return boost::target(t, getGraph());
    }

    /** \brief Removes the given transition \a t. 
     *  
     *  Does not check whether the given transition exists or not.
     */
    inline void removeTrans(const Transition& t)
    {
        BaseGraph::removeEdge(t);
        //removeGraphEdge(t);
    }


    /** \brief Removes a transition that is obtained by states and a label.
     *
     *  If such a transition exists, it is removed and the method returns true. 
     *  Otherwise, returns false.
     */
    bool removeTrans(State s, State t, LabelCArg lbl)
    {
        // сперва ищем существующую
        TransRes tr = getTrans(s, t, lbl);
        if (tr.second)                              // есть искомая
        {
            removeTrans(tr.first);
            return true;
            //return tr.first;
        }

        // если подходящей транзиции нет, просто вернем ложь
        return false;
    }

    
    /** \brief Clears all output transitions of the given state \a s. */
    inline void clearOutTransitions(State s)
    {
        BaseGraph::clearOutEdges(s);
        //clearGraphOutEdges(s);
    }

    /** \brief Clears all input transitions of the given state \a s. */
    inline void clearInTransitions(State s)
    {
        BaseGraph::clearInEdges(s);
        //clearGraphInEdges(s);
    }

    /** \brief Clears all (input and output) transitions of the given state \a s. */
    inline void clearTransitions(State s)
    {
        BaseGraph::clearEdges(s);
        //clearGraphEdges(s);
    }

    /** \brief Removes the state \a s and all its (output and input) transitions. */
    void removeState(State s)
    {
        // пока никаких проверок, что может привести к разному...

        // сперва проверим, не является ли данное состояние анонимным
        if (isStateAnon(s))                     // анонимное
        {
            _anonStates->erase(s);
        }
        else                                    // обычное
        {
            StateIDCRes sid = getStateID(s);    // берем ид
            _stid2verts->erase(sid);            // пытаемся убрать связку из мапы

        }

        clearTransitions(s);                    // очищаем все дуги (требование!)
        BaseGraph::removeVertex(s);             // и саму вершину напоследок
    }

    /** \brief Returns the numbers of input transitions of a state \a s. */
    inline size_t getInTransNum(State s) const { return BaseGraph::getInEdgesNum(s); }
        
    /** \brief Returns the numbers of output transitions of a state \a s. */
    inline size_t getOutTransNum(State s) const { return BaseGraph::getOutEdgesNum(s); }

    /** \brief Returns true if the given state \a s has input transitions, otherwise false. */
    inline bool hasInTransitions(State s) const { return getInTransNum(s) != 0; }

    /** \brief Returns true if the given state \a s has output transitions, otherwise false. */
    inline bool hasOutTransitions(State s) const { return getOutTransNum(s) != 0; }
public:
    //----<Associated Properties>----

    /** \brief Gets an ID of a state \a st. */
    inline StateIDCRes getStateID(State st)
    {
        return extractStateID(getGraph()[st]);
    }

    /** \brief Return bundled property for the given state \a st. */
    inline StPropBundleCRes getBundle(State st) { return getGraph()[st]; }

    /** \brief Return bundled property for the given transition \a tr. */
    inline TrPropBundleCRes getBundle(const Transition& tr) { return getGraph()[tr]; }


    /** \brief Returns data, which is associated with the state \a st. 
     *  
     *  Data is returned as a ref, so it can be freeely modified.
     *  If no custom data have been attached to graph states, default boost::no_property
     *  is returned.
     */
    inline StDataRes getData(State st) { return extractStateData(getGraph()[st]); }
        //1)  возвращает неконст. ссылку на второе поле StateBundle, если это no_property — 
        //значит на статический объект какой - нить(в треитс-классе), его можно менять непосредственно


    /** \brief Returns data, which is associated with the transition \a tr.
     *
     *  Data is returned as a ref, so it can be freeely modified.
     *  If no custom data have been attached to graph trnsitions, default boost::no_property
     *  is returned.
     */
    inline TrDataRes getData(const Transition& tr) { return extractTransData(getGraph()[tr]); }
protected:
    //----<Construction Helpers>----
    
    /** \brief Swap method for copy operator. */
    static void swap(LabeledTS& lhv, LabeledTS& rhv) // noexcept;   // VC12 does not know about this clause...
    {
        // меняем основные поля местами!
        BaseGraph::swap(lhv, rhv);
        std::swap(lhv._stid2verts, rhv._stid2verts);
        std::swap(lhv._anonStates, rhv._anonStates);        // ADDED: 11/01/2018
    }


    /** \brief Dedicated method for creating and populating of a stid2Vertices Map. 
     *
     *  Method should be invocked in a constructor only, where no previously created
     *  map exists.
     */
    void initStID2VertMap()
    {
        // TODO: анонимные состояния

        // НЕ проверяем, что пред. существует, т.к. по договоренности этот метод
        // можно вызвать ТОЛЬКО из конструктора, когда этой мапы быть не должно
        _stid2verts = new StateIDStateMap();
        _anonStates = new StateSet();           // ADDED: 11/01/2018

        // отдельно мапу состояний заполняем по графу: это возможно (индексирование)
        StateIterPair ss = getStates();
        for (; ss.first != ss.second; ++ss.first)
        {
            State s = *ss.first;

            // проверим, не является ли данное состояние анонимным
            if (isStateAnon(s))                         // анонимное
            {
                _anonStates->insert(s);
            }
            else                                        // обычное
            {
                StateIDCRes curID = getStateID(s);
                _stid2verts->insert(SISPair(curID, s));
            }
        }
    }


    /** \brief Create all object that this graph composes. */
    void createObjects()
    {
        _stid2verts = new StateIDStateMap();
        _anonStates = new StateSet();           // ADDED: 11/01/2018
    }


    //----<Internal method for manipulation of the TS>----

    /** \brief Adds into the TS a new regular state with the given state id \a id.
     *
     *  Does not check if another state with the same ID exists, as it should be checked
     *  in a caller by convention.
     */
    State addStateInternal(StateIDCArg id)
    {
        State v = BaseGraph::addVertex();             // вершину в граф
        emplaceStateID(getGraph()[v], id);
        _stid2verts->insert(SISPair(id, v));    // связываем вершину с ид. = state

        return v;
    }


    /** \brief Adds into the TS a new transition between the given states \a s and \a t,
     *  labeled by label \a lbl.
     *
     *  Does not check if another transition, which meets the conditions above, exists. 
     *  It should be checked by a caller by convention.
     */
    Transition addTransitionInternal(State s, State t, LabelCArg lbl)
    {
        //Transition trans = addGraphEdge(s, t);
        Transition trans = BaseGraph::addEdge(s, t);
        emplaceTransLbl(getGraph()[trans], lbl);

        return trans;
    }

protected:

    /** \brief Stores a mapping of StateIDs of the TS to the underlying graph's vertices. */
    StateIDStateMap* _stid2verts;

    /** \brief Set of anonymous states. */
    StateSet* _anonStates;

}; // class LabeledTS



/** \brief Define special traits class for int-id. */
template<>
class StateID_traits<int> {
public:
    typedef int TVal;       ///< Value type.
    typedef int TСArg;      ///< Const argument type.
    //typedef int TArg;       ///< Argument type.
    typedef int& TRef;       ///< Ref-argument type.
    typedef int TCRes;      ///< Constant res.
}; // class StateID_traits


/** \brief Define special traits clss for trans labeled by "const char*". */
template<>
class Label_traits<const char*> {
public:
    typedef const char*     TVal;       ///< Value type.
    typedef const char*     TСArg;      ///< Const argument type.    
    typedef const char* &   TRef;       ///< Ref-argument type.
    typedef const char*     TCRes;      ///< Constant res.
}; // class StateID_traits


/** \brief Define special traits clss for trans labeled by "double". */
template<>
class Label_traits<double> {
public:
    typedef double     TVal;       ///< Value type.
    typedef double     TСArg;      ///< Const argument type.    
    typedef double &   TRef;       ///< Ref-argument type.
    typedef double     TCRes;      ///< Constant res.
}; // class StateID_traits



}}} // namespace xi { namespace ldopa { namespace ts {


#endif // XI_LDOPA_TRSS_MODELS_LABELEDTS_H_
