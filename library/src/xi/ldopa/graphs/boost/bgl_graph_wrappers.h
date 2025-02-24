////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Graphs Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      16.11.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Templates with shortcuts for Boost Graphs
///
////////////////////////////////////////////////////////////////////////////////


#ifndef XI_LDOPA_GRAPHS_BOOST_BGL_GRAPH_WRAPPER_H_
#define XI_LDOPA_GRAPHS_BOOST_BGL_GRAPH_WRAPPER_H_

#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>             // copy_graph
#include <boost/graph/iteration_macros.hpp> // BGL_FORALL_VERTICES

#include <boost/iterator/filter_iterator.hpp>


namespace xi { namespace ldopa { namespace gr {;   //



/** \brief Template for classes with shortcuts to BGL operations with a directed graph 
 *  that is stored through a pointer.
 *
 *  \tparam Graph is a graph type definition.
 *  \tparam VertexCopier is an object for copying vertices properties as it described in 
 *  http://www.boost.org/doc/libs/1_62_0/libs/graph/doc/copy_graph.html
 *  \tparam EdgeCopier is an object for copying vertices properties.
 *
 *  The template instantiation can be inherited or indirectly composed. In the former case consider
 *  to use protected type of inheritance.
 *
 */
template<typename Graph
    //, typename VertexCopier = boost::detail::vertex_copier<Graph, Graph>,
    //typename EdgeCopier = boost::detail::edge_copier<Graph, Graph>
>
class BoostGraphP {
public:
    //----<Types>----

#pragma region Basic Graph Types

    /** \brief Vertex type alias. */
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    /** \brief Edge type alias. */
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;


    /** \brief Iterator for vertices of the graph. */
    typedef typename boost::graph_traits<Graph>::vertex_iterator VertexIter;


    /** \brief Iterator for output edges of a graph. */
    typedef typename boost::graph_traits<Graph>::out_edge_iterator OedgeIter;


    /** \brief Iterator for edges of a graph. */
    typedef typename boost::graph_traits<Graph>::edge_iterator EdgeIter;

    /** \brief A pair of vertex iterators, which represents a collection of vertices. */
    typedef std::pair<VertexIter, VertexIter> VertexIterPair;

    /** \brief A pair of edge iterators, which represents a collection of output edges. */
    typedef std::pair<OedgeIter, OedgeIter> OedgeIterPair;


    /** \brief A pair of edge iterators, which represents a collection of edges. */
    typedef std::pair<EdgeIter, EdgeIter> EdgeIterPair;

    /** \brief Define a predicate that filters a destination vertex. */
    struct TargetVertexFilter
    {
        /** \brief Constructor sets a target (dest) vertex. */
        TargetVertexFilter(Graph g, Vertex t) : graph(g), target(t) { }
        bool operator()(Edge t) { return boost::target(t, graph) == target; }   ///< Predicate.
        
        Vertex target;                                                          ///< Destination Vertex.
        Graph graph;
    };

    /** \brief Filter iterator for dealing with only those edges, which have
     *  a given target vertex.
     */
    typedef boost::filter_iterator<TargetVertexFilter, OedgeIter> TargVertexEdgeIter;

    /** \brief A pair of edge iterators, which represents a collection of edges. */
    typedef std::pair<TargVertexEdgeIter, TargVertexEdgeIter> TargVertexEdgeIterPair;

    /** \brief Type alias for vertex returning result with some helper methods. */
    typedef std::pair<Vertex, bool> VertexRes;

    /** \brief Type alias for edge returning result. */
    typedef std::pair<Edge, bool> EdgeRes;

#pragma endregion // Basic Graph Types


#pragma region Some Utility Types for Copying Graphs

    // see https://stackoverflow.com/questions/9261602/copy-a-graph-adjacency-list-to-another-one

    /** \brief A map of vertices to numbers for indexing vertices. */
    typedef std::map<Vertex, size_t> VertexIndexMap;

    /** \brief Associative Property Map for VertexIndexMap.  */
    typedef boost::associative_property_map<VertexIndexMap> VertexIndexAPM;


#pragma endregion // Some Utility Types for Copying Graphs

public:
    
    /** \brief Virtual destructor. */
    virtual ~BoostGraphP()
    {
        deleteGraph();
    }
protected:
    // prevent to instantiate the class directly
    BoostGraphP() //IVertexCopier* vc = nullptr) //VertexCopier& vc = VertexCopier())
        : _graph(nullptr)
        //, _vertCopier(vc)
    {
        // чтобы не нарушать структурность создания/удаления, все-таки граф туточки
        // сделаю, однако, если вдруг захочется отложенное создание какое-нибудь применить,
        // можно ввести флаг (или параметр конструктора noCreate)
        createGraph();
    }
    
  
    /** \brief Copy constructor */
    BoostGraphP(const BoostGraphP& other)
    {
        createGraph();
        clone(other, *this);            // здесь выполняется актуальное клонирование
    }

public:

    /** \brief Returns a ref to the underlying graph. */
    Graph& getGraph() { return *_graph; }

    /** \brief Const overloaded version of LabeledTS::getGraph(). */
    const Graph& getGraph() const { return *_graph; }

public:
    //-----<BGL Shortcuts>----

    /** \brief Return a number of vertices in the Graph. */
    inline size_t getVerticesNum() const { return boost::num_vertices(getGraph()); }

    /** \brief Return a total number of edges in the Graph. */
    inline size_t getEdgesNum() const  { return boost::num_edges(getGraph()); }


    /** \brief Returns all graph vertices. */
    inline VertexIterPair getVertices() { return boost::vertices(getGraph()); }

    /** \brief Returns all graph vertices (const overload). */
    inline VertexIterPair getVertices() const { return boost::vertices(getGraph()); }

    /** \brief Returns all graph edges. */
    inline EdgeIterPair getEdges() { return boost::edges(getGraph()); }

    /** \brief Returns all graph edges (const). */
    inline EdgeIterPair getEdges() const { return boost::edges(getGraph()); }

    /** \brief Shortcut gets all output edges of a vertex \a v. */
    inline OedgeIterPair getOutEdges(Vertex v) { return boost::out_edges(v, getGraph()); }

    /** \brief Shortcut gets all output edges of a vertex \a v (const). */
    inline OedgeIterPair getOutEdges(Vertex v) const { return boost::out_edges(v, getGraph()); }

    /** \brief Return the total numbers of input and output edges of a vertix \a v. */
    inline size_t getEdgesNum(Vertex v) const { return (size_t)boost::degree(v, getGraph()); }

    /** \brief Return the numbers of output edges of a vertix \a v. */
    inline size_t getOutEdgesNum(Vertex v) const { return (size_t)boost::out_degree(v, getGraph()); }

    /** \brief Returns the source vertex of the given edge \param e. */
    inline Vertex getSrcVertex(const Edge& e) const { return boost::source(e, getGraph()); }

    /** \brief Returns the target vertex of the given edge \param e. */
    inline Vertex getTargVertex(const Edge& e) const { return boost::target(e, getGraph()); }


    /** \brief Adds a new empty vertex into the graph. */
    inline Vertex addVertex() { return boost::add_vertex(getGraph()); }

    /** \brief Adds a new edge into the graph.
     *
     *  It is assumed that the graph is guaranteed to allow parallel arcs,
     *  so boost::add_edge() always adds a new arc.
     */
    inline Edge addEdge(Vertex s, Vertex t) { return boost::add_edge(s, t, getGraph()).first; }



    /** \brief Removes edge.
     *
     *  If a given edge descriptor is not valid, behavior is specifically
     *  undefined. In DEBUG configuration, an assertion is invocated while in RELEASE
     *  configuration all assertions are disabled.
     */
    inline void removeEdge(const Edge& e) { boost::remove_edge(e, getGraph()); }

    /** \brief Clears all output edges of a vertex \a v. */
    inline void clearOutEdges(Vertex v) { boost::clear_out_edges(v, getGraph()); }

    // /** \brief Clears all input edges of a vertex \a v. */
    //inline void clearInEdges(Vertex v) { boost::clear_in_edges(v, getGraph()); }

    /** \brief Clears all (input and output) edges of a vertex \a v. */
    inline void clearEdges(Vertex v) { boost::clear_vertex(v, getGraph()); }

    /** \brief Removes a prepared (w/o any trasitions) vertex from a graph. */
    inline void removeVertex(Vertex v) { boost::remove_vertex(v, getGraph()); }


protected:

    // The following method should normally not be exposed.


    /** \brief Clones a graph \a from to a graph \a to. 
     *
     *  Method isolates graph cloning from the copy constructor.
     *  Graph \a to must be empty!
     */
    static void clone(const BoostGraphP& from, BoostGraphP& to)
    {
        //исходим

        // // копирователь графа кастомизированный, передаем по наследству
        //_vertCopier = other._vertCopier;

        // http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/copy_graph.html
        // https://stackoverflow.com/questions/9261602/copy-a-graph-adjacency-list-to-another-one
        // https://stackoverflow.com/questions/7935417/how-provide-a-vertex-index-property-for-my-graph


        // облегчаем понимание доступа к источнику и получателю
        const Graph& srcG = from.getGraph();
        Graph& destG = to.getGraph();


        // associative property map for indexing vertices
        VertexIndexMap sim;                  // подлежащая мапа
        VertexIndexAPM apm(sim);             // обертка для нее
        fillAPMUpForGraph(apm, srcG);


        // при копировании явным образом задает копирователей вершин и дуг
        boost::copy_graph(srcG, destG,
            boost::vertex_index_map(apm).

            vertex_copy(boost::detail::vertex_copier<Graph, Graph>(srcG, destG))    // по дефолту
            );
    }

    /** \brief Swap method for copy operator. */
    static void swap(BoostGraphP& lhv, BoostGraphP& rhv) // noexcept;   // VC12 does not know about this clause...
    {
        // меняем основные поля местами!
        std::swap(lhv._graph, rhv._graph);
    }

    /** \brief Fills the given associateive property map for the current graph.
     *
     *  For an example, see https://stackoverflow.com/questions/9261602/copy-a-graph-adjacency-list-to-another-one
     */
    static void fillAPMUpForGraph(VertexIndexAPM& apm, const Graph& g)
    {
        int i = 0;
        BGL_FORALL_VERTICES(v, g, Graph)
        {
            put(apm, v, i++);
        }
    }

    /** \brief Creates a graph object. */
    void createGraph()
    {
        _graph = new Graph();
    }

    /** \brief Deletes the graph objects. */
    void deleteGraph()
    {
        if (_graph)
        {
            delete _graph;
            _graph = nullptr;
        }
    }

protected:
    /** \brief Stores underlying graph */
    Graph* _graph;
}; // class BoostGraphP


/** \brief Extended version of the template for classes with shortcuts to BGL operations 
 *  with a BI-directed graph that is stored through a pointer.
 *
 *  \tparam Graph is a graph type definition.
 *  \tparam VertexCopier is an object for copying vertices properties as it described in
 *  http://www.boost.org/doc/libs/1_62_0/libs/graph/doc/copy_graph.html
 *  \tparam EdgeCopier is an object for copying vertices properties.
 *
 *  The template instantiation can be inherited or indirectly composed. In the former case consider
 *  to use protected type of inheritance.
 */
template<typename Graph
    //, typename VertexCopier = boost::detail::vertex_copier<Graph, Graph>,
    //typename EdgeCopier = boost::detail::edge_copier<Graph, Graph>
>
class BoostBidiGraphP : public BoostGraphP<Graph> //, VertexCopier, EdgeCopier> 
{
public:
    //----<Types>----

    /** \brief Iterator for  input edges of a graph. */
    typedef typename boost::graph_traits<Graph>::in_edge_iterator IedgeIter;

    /** \brief A pair of edge iterators, which represents a collection of input edges. */
    typedef std::pair<IedgeIter, IedgeIter> IedgeIterPair;

public:

    /** \brief Virtual destructor. */
    virtual ~BoostBidiGraphP()
    {
        deleteGraph();
    }

public:
    //-----<BGL Shortcuts>----

    /** \brief Gets all input edges of a vertex \a v. */
    inline IedgeIterPair getInEdges(Vertex v) { return boost::in_edges(v, getGraph()); }

    /** \brief Gets all input edges of a vertex \a v (const). */
    inline IedgeIterPair getInEdges(Vertex v) const { return boost::in_edges(v, getGraph()); }

    /** \brief Returns the numbers of input  edges of a vertix \a v. */
    inline size_t getInEdgesNum(Vertex v) const { return (size_t)boost::in_degree(v, getGraph()); }

     /** \brief Clears all input edges of a vertex \a v. */
    inline void clearInEdges(Vertex v) { boost::clear_in_edges(v, getGraph()); }

}; // class BoostBidiGraphP


}}} // namespace xi { namespace ldopa namespace gr 



#endif // XI_LDOPA_GRAPHS_BOOST_BGL_GRAPH_WRAPPER_H_
