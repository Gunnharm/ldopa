///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Implementation of directed graph class based on pointers conception.
///
/// >> 
///
///////////////////////////////////////////////////////////////////////////////


#ifndef XI_LDOPA_GRAPHS_BOOST_BGLGRAPHADAPTERS_H_
#define XI_LDOPA_GRAPHS_BOOST_BGLGRAPHADAPTERS_H_

#pragma once


#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

// uncomment to let debug messages
#define XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES

namespace xi { namespace ldopa {;   //


/** \brief Adapter for boost bidi graph
 *
 *  In fact, the class is just an adapter for BGL specific class.
 *  About policies for storage Vertexes and Edges:
 *      http://stackoverflow.com/questions/21457531/performance-vertex-descriptor-of-the-boost-graph-library
 */
template <typename TEdgeStorage, typename TVertStorage, typename TDirection = boost::directedS>
class BglGraphTempl {
public:
    // types
    typedef boost::adjacency_list
        <
            TEdgeStorage,   //boost::vecS,
            TVertStorage,   //boost::vecS,
            TDirection      //boost::bidirectionalS
        > Graph;

    typedef typename boost::graph_traits<Graph>::vertex_descriptor   Vertex;
    typedef typename boost::graph_traits<Graph>::edge_descriptor     Edge;

    typedef typename boost::graph_traits<Graph>::vertex_iterator VertexIterator;
    typedef typename boost::graph_traits<Graph>::edge_iterator EdgeIterator;
    typedef typename boost::graph_traits<Graph>::out_edge_iterator OutEdgeIterator;


    // pairs
    typedef std::pair<Edge, bool> EdgeRetResult;
    typedef std::pair<VertexIterator, VertexIterator> VertexIteratorsPair;
    typedef std::pair<EdgeIterator, EdgeIterator> EdgeIteratorsPair;
    typedef std::pair<OutEdgeIterator, OutEdgeIterator> OutEdgeIteratorsPair;

    typedef std::pair<Vertex, bool> VertexRetResult;

public:

    /** \brief Adds a vertex */
    Vertex addVertex() { return boost::add_vertex(_gr); }

    /** \brief Removes a given vertex */
    void removeVertex(Vertex v) { boost::remove_vertex(v, _gr); }

    /** \brief Adds an edge */
    EdgeRetResult addEdge(Vertex src, Vertex dst) { return boost::add_edge(src, dst, _gr); }


    /** \brief Return a sequence of vertices */
    VertexIteratorsPair getVertices() const { return boost::vertices(_gr); }

    /** \brief Returns a sequence of all the edges */
    EdgeIteratorsPair getEdges() const { return boost::edges(_gr); }

    /** \brief Returns a sequence of all edges between two given vertices 
     *  
     *  Important info from BGL documentation:
     *  This function only works when the OutEdgeList for the adjacency_list is a container that sorts the out edges 
     *  according to target vertex, and allows for parallel edges. The multisetS selector chooses such a container. 
     */
    OutEdgeIteratorsPair getEdges(Vertex src, Vertex dst) const { return boost::edge_range(src, dst, _gr); }

    /** \brief Returns the edge connecting two given vertices if exists */
    EdgeRetResult getEdge(Vertex src, Vertex dst) const { return boost::edge(src, dst, _gr); }

    /** \brief Returns the source vertex for a given edge */
    Vertex getSrcVertex(const Edge& e) const { return boost::source(e, _gr); }

    /** \brief Returns the destination vertex for a given edge */
    Vertex getDstVertex(const Edge& e) const { return boost::target(e, _gr); };

    /** \brief Return a number of vertices in the graph */
    size_t getVerticesNum() const { return boost::num_vertices(_gr); }

    /** \brief Return a number of edges in the graph */
    size_t getEdgesNum() const { return boost::num_edges(_gr); }

    //operator 

    /** \brief Gets an underlying BGL graph */
    Graph& getBglGraph() { return _gr; }

    /** \brief Overloaded version of getBglGraph() */
    const Graph& getBglGraph() const { return _gr; }

protected:
    Graph _gr;

}; // class BGraphTempl

/** \brief A graph type based on lists of vertices and edges 
 *
 *  Doesn't allow edges duplicates; hence, getEdges(Vertex src, Vertex dst) is not working for it, hance.
 */
typedef BglGraphTempl<boost::listS, boost::listS, boost::bidirectionalS> BaseListBGraph;


}} // namespace xi { namespace ldopa



#endif // XI_LDOPA_GRAPHS_BOOST_BGLGRAPHADAPTERS_H_
