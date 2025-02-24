// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include <set>

#include "xi/ldopa/graphs/bidigraph.h"
#include "xi/ldopa/utils.h"

// #define SUPPRESS_INDENTATION 
// here ; is for suppressing stupid VS2013 auto indentation inside namspaces 
// see http://stackoverflow.com/questions/3727862/is-there-any-way-to-make-visual-studio-stop-indenting-namespaces
namespace xi { namespace ldopa {; // !<---


//==============================================================================
// class ListVerticesStorage
//==============================================================================

//------------------------------------------------------------------------------
// Initialize with a linked graph.
//------------------------------------------------------------------------------
ListVerticesStorage::ListVerticesStorage(BidiGraph* gr)
    : _graph(gr)
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ListVerticesStorage(): %#010x\n", (unsigned int)this);
#endif
}

//------------------------------------------------------------------------------
ListVerticesStorage::~ListVerticesStorage()
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ~ListVerticesStorage(): %#010x\n", (unsigned int)this);
#endif
}

//------------------------------------------------------------------------------
BidiGraphVertex* ListVerticesStorage::addNewVertex()
{
    BidiGraphVertex* v = new BidiGraphVertex(_graph);
    _vertices.push_back(v);

    return v;
}


//------------------------------------------------------------------------------
void ListVerticesStorage::deleteAllVertices()
{
    // deleting all vertices implies preliminary deleting all vertices
    _graph->getEdgeStorage()->deleteAllEdges();

    // iterates over all vertices
    for (VerticesListIterator it = _vertices.begin(); it != _vertices.end(); ++it)
    {
        BidiGraphVertex* vert = *it;
        // TODO: возможно надо сперва что-то с вершиной сделать перед удалением?

        delete vert;
    }

    // clears lists of all vertices
    _vertices.clear();
}

//------------------------------------------------------------------------------
size_t ListVerticesStorage::getVerticesNum() const
{
    return _vertices.size();
}

//------------------------------------------------------------------------------
BidiGraph* ListVerticesStorage::getLinkedGraph() const
{
    return _graph;
}

//------------------------------------------------------------------------------
void ListVerticesStorage::setLinkedGraph(BidiGraph* gr)
{
    _graph = gr;
}


IVertexEnumerator* ListVerticesStorage::enumerateAllVertices()
{
    return new MyEnumerator(_vertices.begin(), _vertices.end());
}


IVertexEnumeratorConst* ListVerticesStorage::enumerateAllVerticesConst() const
{
    return new MyConstEnumerator(_vertices.begin(), _vertices.end());
}


//==============================================================================
// class Str2VerticeMapVerticesStorage
//==============================================================================

//------------------------------------------------------------------------------
MapNamedVerticesStorage::MapNamedVerticesStorage(BidiGraph* gr, bool createReverseMap)
    : _graph(gr),
    _reverseLookupMap(nullptr)
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> MapNamedVerticesStorage(): %#010x\n", (unsigned int)this);
#endif
    if (createReverseMap)
        _reverseLookupMap = new Vertex2StrMap();
}

MapNamedVerticesStorage::MapNamedVerticesStorage(bool createReverseMap):
    _graph(nullptr),
    _reverseLookupMap(nullptr)
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> MapNamedVerticesStorage(): %#010x\n", (unsigned int)this);
#endif
    if (createReverseMap)
        _reverseLookupMap = new Vertex2StrMap();
}

//------------------------------------------------------------------------------
MapNamedVerticesStorage::~MapNamedVerticesStorage()
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ~MapNamedVerticesStorage(): %#010x\n", (unsigned int)this);
#endif

    // if reverse map is used
    if (_reverseLookupMap)
        delete _reverseLookupMap;

}

//------------------------------------------------------------------------------
BidiGraphVertex* MapNamedVerticesStorage::addNewVertex(const std::string& name)
{
    if (isVertexExists(name))
        return nullptr;

    return addNewVertexInternal(name);
}

//------------------------------------------------------------------------------
BidiGraphVertex* MapNamedVerticesStorage::getOrAddNewVertex(const std::string& name)
{
    BidiGraphVertex* vert = getVertex(name);
    if (vert)
        return vert;

    return addNewVertexInternal(name);
}


//------------------------------------------------------------------------------
BidiGraphVertex* MapNamedVerticesStorage::addNewVertexInternal(const std::string& name)
{
    BidiGraphVertex* v = new BidiGraphVertex(_graph);
    _vertices[name] = v;

    // if reverse map is used
    if (_reverseLookupMap)
    {
        (*_reverseLookupMap)[v] = name;
    }

    return v;
}


//------------------------------------------------------------------------------
BidiGraph* MapNamedVerticesStorage::getLinkedGraph() const
{
    return _graph;
}


//------------------------------------------------------------------------------
void MapNamedVerticesStorage::setLinkedGraph(BidiGraph* gr)
{
    _graph = gr;
}


//------------------------------------------------------------------------------
IVertexEnumerator* MapNamedVerticesStorage::enumerateAllVertices()
{
    return new MyEnumerator(_vertices.begin(), _vertices.end());
    
}

//------------------------------------------------------------------------------
IVertexEnumeratorConst* MapNamedVerticesStorage::enumerateAllVerticesConst() const
{
    return new MyConstEnumerator(_vertices.begin(), _vertices.end());
}


//------------------------------------------------------------------------------
BidiGraphVertex* MapNamedVerticesStorage::getVertex(const std::string& name)
{
    Str2VertexMapConstIterator it = getVertexIt(name);
    if (it == _vertices.end())
        return nullptr;

    return it->second;
}


//------------------------------------------------------------------------------
bool MapNamedVerticesStorage::getNameOfVertex(const BidiGraphVertex* vert, std::string& name) const
{
    
    // if reverse map is used
    if (_reverseLookupMap)
    {
        Vertex2StrMapConstIterator it = _reverseLookupMap->find(vert);
        if (it == _reverseLookupMap->end())
            return false;     //LdopaException::throwException(errMs);

        name = it->second;
        return true; //it->second;
    }


    // forward lookup (slow)
    Str2VertexMapConstIterator it = findVertexByName(vert);
    if (it == _vertices.end())
        return false; //LdopaException::throwException(errMs);

    // found
    name = it->first;
    return true; //it->second;
}

//------------------------------------------------------------------------------
MapNamedVerticesStorage::Str2VertexMapConstIterator 
    MapNamedVerticesStorage::findVertexByName(const BidiGraphVertex* vert) const
{
    Str2VertexMapConstIterator it = _vertices.begin();
    for (; it != _vertices.end(); ++it)
    {
        if (it->second == vert)
            return it;
    }

    return it;
}


//------------------------------------------------------------------------------
void MapNamedVerticesStorage::deleteAllVertices()
{
    // deleting all vertices implies preliminary deleting all vertices
    _graph->getEdgeStorage()->deleteAllEdges();

    // iterates over all vertices
    for (Str2VertexMapIterator it = _vertices.begin(); it != _vertices.end(); ++it)
    {
        BidiGraphVertex* vert = it->second;
        // TODO: возможно надо сперва что-то с вершиной сделать перед удалением?

        delete vert;
    }

    // clears lists of all vertices
    _vertices.clear();


    if (_reverseLookupMap)
        _reverseLookupMap->clear();
}

//------------------------------------------------------------------------------
size_t MapNamedVerticesStorage::getVerticesNum() const
{
    return _vertices.size();
}



//==============================================================================
// class OutInMMapEdgeStorage
//==============================================================================


//------------------------------------------------------------------------------
// Initialize with a linked graph.
//------------------------------------------------------------------------------
OutInMMapEdgeStorage::OutInMMapEdgeStorage(BidiGraph* gr)
    : _graph(gr)
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> OutInMMapEdgeStorage(): %#010x\n", (unsigned int)this);
#endif
}


OutInMMapEdgeStorage::~OutInMMapEdgeStorage()
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ~OutInMMapEdgeStorage(): %#010x\n", (unsigned int)this);
#endif

    cleanMapsInternal();
}

//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::deleteAllEdges()
{
    cleanMapsInternal();
}


//------------------------------------------------------------------------------
BidiGraphEdge* OutInMMapEdgeStorage::addEdge(BidiGraphVertex* srcVertex, 
                BidiGraphVertex* dstVertex, bool allowDuplicate /*= true*/)
{
    check4NullVertices(srcVertex, dstVertex);

    if (!allowDuplicate)
        if (isOutEdgeExists(srcVertex, dstVertex))
            return nullptr;					// no duplicates and an edge is already presented

    // do not check graph equivalence due to it is done in edge constructor
    BidiGraphEdge* edge = addNewEdgeInternal(srcVertex, dstVertex);

    return edge;
}


////------------------------------------------------------------------------------
BidiGraphEdge* OutInMMapEdgeStorage::addEdge(BidiGraphVertex* srcVertex, 
        BidiGraphVertex* dstVertex, IEdgeAddChecker* addChecker)
{
    check4NullVertices(srcVertex, dstVertex);

    // asks the oracle if an edge can be added
    if (!addChecker->ifAnEdgeCanBeAdded(srcVertex, dstVertex))
        return nullptr;

    // if can
    BidiGraphEdge* edge = addNewEdgeInternal(srcVertex, dstVertex);

    return edge;
}

//------------------------------------------------------------------------------
// If an edge between the vertex and a given vertex exists, return it (first occurence),
// otherwise adds a new one and returns it
//------------------------------------------------------------------------------
BidiGraphEdge* OutInMMapEdgeStorage::getOrAddNewEdge(BidiGraphVertex* srcVertex,
            BidiGraphVertex* dstVertex)
{
    check4NullVertices(srcVertex, dstVertex);

    BidiGraphEdge* resEdge = getFirstOutEdge(srcVertex, dstVertex);
    if (resEdge)
        return resEdge;

    // need to add a new edge
    resEdge = addNewEdgeInternal(srcVertex, dstVertex);

    return resEdge;
}



//------------------------------------------------------------------------------
BidiGraphEdge* OutInMMapEdgeStorage::addNewEdgeInternal(BidiGraphVertex* srcVertex, 
    BidiGraphVertex* dstVertex)
{
    BidiGraphEdge* edge = new BidiGraphEdge(srcVertex, dstVertex);

    // adds to output maps
    _outEdges.insert(std::make_pair(std::make_pair(srcVertex, dstVertex), edge));   

    // adds (inversed vertices order!!!)to input map
    _inEdges.insert(std::make_pair(std::make_pair(dstVertex, srcVertex), edge));

    return edge;
}




//------------------------------------------------------------------------------
size_t OutInMMapEdgeStorage::getEdgesNum() const
{
    return _outEdges.size();
    //throw std::logic_error("The method or operation is not implemented.");
}

//------------------------------------------------------------------------------
bool OutInMMapEdgeStorage::isOutEdgeExists(BidiGraphVertex* srcVertex, 
            BidiGraphVertex* dstVertex) const
{
    //PairOfVertices2EdgeMMapIterator elIt;
    PairOfVertices2EdgeMMapConstIterator elIt;
    getFirstOutEdge(srcVertex, dstVertex, elIt);

    return elIt != _outEdges.end();
}

//------------------------------------------------------------------------------
BidiGraphEdge* OutInMMapEdgeStorage::getFirstOutEdge(BidiGraphVertex* srcVertex,
    BidiGraphVertex* dstVertex) const
{
    // do not check if vertices belong to different graphs
    check4NullVertices(srcVertex, dstVertex);

    PairOfVertices2EdgeMMapConstIterator elIt = _outEdges.find(std::make_pair(srcVertex, dstVertex));
    if (elIt != _outEdges.end())
        return elIt->second;

    return nullptr;				// no edge found
}

//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getFirstOutEdge(BidiGraphVertex* srcVertex,
    BidiGraphVertex* dstVertex, PairOfVertices2EdgeMMapIterator& elIt)
{
    // do not check if vertices belong to different graphs
    check4NullVertices(srcVertex, dstVertex);

    elIt = _outEdges.find(std::make_pair(srcVertex, dstVertex));
}


//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getFirstOutEdge(BidiGraphVertex* srcVertex,
    BidiGraphVertex* dstVertex, PairOfVertices2EdgeMMapConstIterator& elIt) const
{
    // do not check if vertices belong to different graphs
    check4NullVertices(srcVertex, dstVertex);

    elIt = _outEdges.find(std::make_pair(srcVertex, dstVertex));
}

//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getAllEdges(BidiGraphVertex* srcVertex, 
        BidiGraphVertex* dstVertex, ListOfEdges& resList) const
{
    check4NullVertices(srcVertex, dstVertex);

    std::pair <PairOfVertices2EdgeMMapConstIterator, PairOfVertices2EdgeMMapConstIterator> ret;
    ret = _outEdges.equal_range(std::make_pair(srcVertex, dstVertex));


    // fill the given list
    for (; ret.first != ret.second; ++ret.first)
    {
        //PairOfVertices2EdgeMMapConstIterator it = ret.first;
        BidiGraphEdge* edg = (ret.first)->second;
        resList.push_back(edg);
    }
}

//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getAllEdges(ListOfEdges& resList) const
{
    for (PairOfVertices2EdgeMMapConstIterator it = _outEdges.begin();
                    it != _outEdges.end(); ++it)
        resList.push_back(it->second);
}

//------------------------------------------------------------------------------
BidiGraphEdge* OutInMMapEdgeStorage::getFirstInEdge(BidiGraphVertex* dstVertex, 
    BidiGraphVertex* srcVertex) const
{
    // do not check if vertices belong to different graphs
    check4NullVertices(srcVertex, dstVertex);

    PairOfVertices2EdgeMMapConstIterator elIt = _inEdges.find(std::make_pair(dstVertex, srcVertex));
    if (elIt != _inEdges.end())
        return elIt->second;

    return nullptr;				// no edge found
}

//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getFirstInEdge(BidiGraphVertex* dstVertex, 
            BidiGraphVertex* srcVertex, PairOfVertices2EdgeMMapIterator& elIt)
{
    // do not check if vertices belong to different graphs
    check4NullVertices(srcVertex, dstVertex);

    elIt = _inEdges.find(std::make_pair(dstVertex, srcVertex));
}

//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getFirstInEdge(BidiGraphVertex* dstVertex,
    BidiGraphVertex* srcVertex, PairOfVertices2EdgeMMapConstIterator& elIt) const
{
    // do not check if vertices belong to different graphs
    check4NullVertices(srcVertex, dstVertex);

    elIt = _inEdges.find(std::make_pair(dstVertex, srcVertex));
}



//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::getAllInEdges(BidiGraphVertex* dstVertex, 
        BidiGraphVertex* srcVertex, ListOfEdges& resList) const
{
    check4NullVertices(srcVertex, dstVertex);
    
    std::pair <PairOfVertices2EdgeMMapConstIterator, PairOfVertices2EdgeMMapConstIterator> ret;
    ret = _inEdges.equal_range(std::make_pair(dstVertex, srcVertex));

    // fill the given list
    for (; ret.first != ret.second; ++ret.first)
    {
        //PairOfVertices2EdgeMMapConstIterator it = ret.first;
        BidiGraphEdge* edg = (ret.first)->second;
        resList.push_back(edg);
    }
}


//------------------------------------------------------------------------------
BidiGraph* OutInMMapEdgeStorage::getLinkedGraph() const
{
    return _graph;
}


//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::setLinkedGraph(BidiGraph* gr)
{
    _graph = gr;
}

//------------------------------------------------------------------------------
IEdgesEnumerator* OutInMMapEdgeStorage::enumerateAllEdges()
{
    return new MyEnumerator(_outEdges.begin(), _outEdges.end());
}

//------------------------------------------------------------------------------
IEdgesEnumeratorConst* OutInMMapEdgeStorage::enumerateAllEdgesConst() const
{
    return new MyConstEnumerator(_outEdges.begin(), _outEdges.end());
}




//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::cleanMapsInternal()
{
    // using output maps for deleting pointers

    // iterates over all edges    
    for (PairOfVertices2EdgeMMapIterator it = _outEdges.begin(); it != _outEdges.end(); ++it)
    {
        BidiGraphEdge* edge = it->second;
        // TODO: возможно надо сперва что-то с дугой сделать перед удалением?

        delete edge;
    }

    // clears lists of out and in edges
    _outEdges.clear();
    _inEdges.clear();
}


//------------------------------------------------------------------------------
void OutInMMapEdgeStorage::check4NullVertices(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex)
{
    if (srcVertex == nullptr || dstVertex == nullptr)
        throw LdopaException("One of the given vertices is null");

}


//==============================================================================
// class BidiGraph
//==============================================================================

    
//------------------------------------------------------------------------------
// Default constructor with ListVerticesStorage as a storage engine 
//------------------------------------------------------------------------------    
BidiGraph::BidiGraph(IVerticesStorage* vs, IEdgesStorage* es):
    _vertStorage(vs),
    _edgesStorage(es)
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> BidiGraph(): %#010x\n", (unsigned int)this);
#endif

    if (!_vertStorage)
        _vertStorage = new ListVerticesStorage(this);
    else
    {
        if (_vertStorage->getLinkedGraph())
            throw LdopaException("Given VerticesStorage is already attached to other graph");
        _vertStorage->setLinkedGraph(this);
    }
    
    if (!_edgesStorage)
        _edgesStorage = new OutInMMapEdgeStorage(this);
    else
    {
        if (_edgesStorage->getLinkedGraph())
            throw LdopaException("Given EdgesStorage is already attached to other graph");
        _edgesStorage->setLinkedGraph(this);
    }

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
BidiGraph::~BidiGraph()
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ~BidiGraph(): %#010x\n", (unsigned int)this);
#endif

    // frees the memory
    clearGraph();

    // deletes embedded storage!
    delete _edgesStorage;
    delete _vertStorage;
}



//------------------------------------------------------------------------------
// Clears all vertices and related edges
//------------------------------------------------------------------------------
void BidiGraph::clearGraph()
{
    // just ask storage engine to free all vertices, it also implies for deleting all edges
    _vertStorage->deleteAllVertices();

}





//==============================================================================
// class BidiGraphVertex
//==============================================================================


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
BidiGraphVertex::BidiGraphVertex(BidiGraph* ownerGr):
    _ownerGr(ownerGr) //,
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> BidiGraphVertex(): %#010x\n", (unsigned int)this);
#endif
}

BidiGraphVertex::~BidiGraphVertex()
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ~BidiGraphVertex(): %#010x\n", (unsigned int)this);
#endif
}

//==============================================================================
// class BidiGraphEdge
//==============================================================================


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
//BidiGraphEdge::BidiGraphEdge(BidiGraph* gr, BidiGraphVertex* srcV, BidiGraphVertex* dstV):
BidiGraphEdge::BidiGraphEdge(BidiGraphVertex* srcV, BidiGraphVertex* dstV) :
//_ownerGr(gr),
_srcVertex(srcV),
_dstVertex(dstV)
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> BidiGraphEdge(): %#010x\n", (unsigned int)this);
#endif

    if (!srcV->getOwnerGraph() || !dstV->getOwnerGraph())
        throw LdopaException("One of the object is null");


    // check if graph, src vertex and dst vertex are for the same graph
    if (srcV->getOwnerGraph() != dstV->getOwnerGraph())
        throw LdopaException("Can't create an edge between two vertices belonging to different graphs");
    //LdopaException::throwException("Can't create an edge to a vertex that doesn't belong to the same graph");
}


//------------------------------------------------------------------------------
BidiGraphEdge::~BidiGraphEdge()
{
#ifdef XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES
    printf("--[Debug]--> ~BidiGraphEdge(): %#010x\n", (unsigned int)this);
#endif
}



}} // namespace xi { namespace ldopa

