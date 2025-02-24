///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Implementation of directed graph class based on pointers conception.
///
/// >> 
///
///////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_GRAPHS_BIDIRGRAPH_H_
#define XI_LDOPA_GRAPHS_BIDIRGRAPH_H_

#pragma once

// xi.lib
#include "xi/collections/enumerators_obsolete1.hpp"     // сент. 2017: новая версия теперь

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// std
#include <list>
#include <set>
#include <map>


// uncomment to let debug messages
#define XI_LDOPA_GRAPHS_DIRGRAPH_DEBUG_MESSAGES

namespace xi { namespace ldopa {;   //

class LDOPA_API BidiGraph;
class LDOPA_API BidiGraphVertex;
class LDOPA_API BidiGraphEdge;


typedef ::xi::collections::IEnumerator_obsolete1<BidiGraphVertex> IVertexEnumerator;
typedef ::xi::collections::IConstEnumerator_obsolete1<BidiGraphVertex> IVertexEnumeratorConst;

// экспорт конкретизированных шаблонов в DLL
// это должно быть ДО использования в нижележащих классах
// PIMPL (https://support.microsoft.com/en-us/kb/168958)
#ifdef LDOPA_DLL

//LDOPA_EXPIMP_TEMPLATE template class LDOPA_API
//    std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::list<BidiGraphVertex*>;
LDOPA_EXPIMP_TEMPLATE template class LDOPA_API std::map<std::string, BidiGraphVertex*>;
LDOPA_EXPIMP_TEMPLATE template class LDOPA_API
    std::multimap<std::pair<BidiGraphVertex*, BidiGraphVertex*>, BidiGraphEdge*>;

//LDOPA_EXPIMP_TEMPLATE template class LDOPA_API
//    std::basic_string<char, std::char_traits<char>, std::allocator<char>>;


#endif // LDOPA_DLL

//==============================================================================

/** \brief Declares interfaces determining storage engine for graph vertices.
 *
 *  An object of the derived class is to be passed to a graph when it is being created.
 */
class LDOPA_API IVerticesStorage {

public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr */
    virtual ~IVerticesStorage() {}

public:

    /** \brief Returns number of stored  vertices */
    virtual size_t getVerticesNum() const = 0;

    /** \brief Clears all vertices and corresponfong edges due to inability to storage them w/o vertices */
    virtual void deleteAllVertices() = 0;

    /** \brief \returns an associated graph */
    virtual BidiGraph* getLinkedGraph() const = 0;

    /** \brief Sets an associated graph */
    // UPD: 23/07/2017: полагаю, это ерунда: вершина создается один раз и навсегда для одного графа!
    virtual void setLinkedGraph(BidiGraph*) = 0;

    // /** \brief Returns enumerator for enumerating all vertices (enumerator should be deleted then) */
    //virtual IVerticesEnumerator* enumerateAllVertices() = 0;

    /** \brief Returns enumerator for enumerating all vertices (enumerator should be deleted then) */
    virtual IVertexEnumerator* enumerateAllVertices() = 0;

    /** \brief Const overloaded version of enumerateAllVertices() */
    virtual IVertexEnumeratorConst* enumerateAllVerticesConst() const = 0;

}; // IVerticesStorage


//==============================================================================


/** \brief Simple vertices storage engine based on list of vertices pointers.
 *
 *  Used when no additional indexing data are attached to vertices.
 */
class LDOPA_API ListVerticesStorage : public IVerticesStorage
{
public:
    //------------<Types>--------------------------------------

    typedef ListVerticesStorage MyTypeName;

    /** \brief Defines a type shortcut for list of vertices */
    typedef std::list<BidiGraphVertex*> VerticesList;
    typedef VerticesList::iterator VerticesListIterator;
    typedef VerticesList::const_iterator VerticesListConstIterator;

    typedef xi::collections::Enumerator4Iterator_obsolete1<BidiGraphVertex, VerticesListConstIterator> MyEnumerator;
    typedef xi::collections::ConstEnumerator4Iterator_obsolete1<BidiGraphVertex, VerticesListConstIterator> MyConstEnumerator;

public:
    /** \brief Initialize with a linked graph. */
    ListVerticesStorage(BidiGraph* gr = nullptr);
    virtual ~ListVerticesStorage();

public:

    /** \brief Simply adds a new vertex and returns a ptr to it  */
    BidiGraphVertex* addNewVertex();

    // IVerticesStorage::clearVertices()
    virtual void deleteAllVertices() override;

    // IVerticesStorage::getVerticesNum()
    virtual size_t getVerticesNum() const override;

    // IVerticesStorage::
    virtual BidiGraph* getLinkedGraph() const override;

    // IVerticesStorage::
    virtual void setLinkedGraph(BidiGraph* gr) override;


    //  IVerticesStorage::
    virtual IVertexEnumerator* enumerateAllVertices()  override;

    //  IVerticesStorage::
    virtual IVertexEnumeratorConst* enumerateAllVerticesConst() const override;

public:
    //------------<Sets/Gets>----------------
    /** \brief Returns a list of vertices. Should be used with a special care! */
    VerticesList& getVertices() { return _vertices; }

    /** \brief Returns a constant list of vertices */
    const VerticesList& getVertices() const { return _vertices; }

protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    ListVerticesStorage(const ListVerticesStorage&);                 // Prevent copy-construction
    ListVerticesStorage& operator=(const ListVerticesStorage&);      // Prevent assignment

protected:
    
    /** \brief Linked graph */
    BidiGraph* _graph;
    
    /** \brief Stores all graph's vertices as a list */
    VerticesList _vertices;
}; // class ListVerticesStorage


//==============================================================================

/** \brief Vertices storage engine based on str->Vertex map.
*
*   Strings represent vertices name that suppose to be unique
*/
class LDOPA_API MapNamedVerticesStorage : public IVerticesStorage
{
public:
    //------------<Types>--------------------------------------

    typedef MapNamedVerticesStorage MyTypeName;

    /** \brief Defines a type shortcut for list of vertices */
    typedef std::map<std::string, BidiGraphVertex*> Str2VertexMap;
    typedef Str2VertexMap::iterator Str2VertexMapIterator;
    typedef Str2VertexMap::const_iterator Str2VertexMapConstIterator;
    typedef std::map<const BidiGraphVertex*, std::string> Vertex2StrMap;
    typedef Vertex2StrMap::const_iterator Vertex2StrMapConstIterator;

    typedef xi::collections::Enumerator4Iterator_obsolete1<BidiGraphVertex, Str2VertexMapConstIterator> MyEnumerator;
    typedef xi::collections::ConstEnumerator4Iterator_obsolete1<BidiGraphVertex, Str2VertexMapConstIterator> MyConstEnumerator;


public:
    /** \brief Initialize with a linked graph.
     *
     *  \param createReverseMap determines if a reverse map for fast search is to be created
     */
    MapNamedVerticesStorage(BidiGraph* gr = nullptr, bool createReverseMap = false);
    MapNamedVerticesStorage(bool createReverseMap);
    virtual ~MapNamedVerticesStorage();// {}

public:

    /** \brief Tries to add a new vertex with a given name. If a vertex with the same
        name exists, returns null, otherwise returns a ptr to a new vertex*/
    BidiGraphVertex* addNewVertex(const std::string& name);

    /** \brief If a vertex with given name exists, return it,
        otherwise adds a new one and returns it */
    BidiGraphVertex* getOrAddNewVertex(const std::string& name);


    /** \brief Checks if there a vertex with given name exists */
    bool isVertexExists(const std::string& name) const
    {
        return (getVertexIt(name) != _vertices.end());
    }

    /** \brief Looks for a vertex with given name and if one exists, returns it.
        Otherwise returns nullptr */
    BidiGraphVertex* getVertex(const std::string& name);

    /** \brief Return an iterator for an element with given name */
    Str2VertexMapIterator getVertexIt(const std::string& name)
    {
        return _vertices.find(name);
    }

    /** \brief Return a const iterator for an element with given name */
    Str2VertexMapConstIterator getVertexIt(const std::string& name) const
    {
        return _vertices.find(name);
    }

    /** \brief For a given vertex (ptr) returns its name.
     *
     *  If no vertex with given ptr exists, \returns false, otherwise \returns true.
     *  If a special map for reverse ptr-2-str is set, use it for looking names
     */
    bool getNameOfVertex(const BidiGraphVertex* vert, std::string& name) const;

    /** \brief For a given vertex ptr searches for an element and returns its iterator */
    Str2VertexMapConstIterator findVertexByName(const BidiGraphVertex* vert) const;
public:
    // IVerticesStorage::clearVertices()
    virtual void deleteAllVertices() override;

    // IVerticesStorage::getVerticesNum()
    virtual size_t getVerticesNum() const override;

    // IVerticesStorage::
    virtual BidiGraph* getLinkedGraph() const override;

    // IVerticesStorage::
    virtual void setLinkedGraph(BidiGraph* gr) override;

    //  IVerticesStorage::
    virtual IVertexEnumerator* enumerateAllVertices() override;

    //  IVerticesStorage::
    virtual IVertexEnumeratorConst* enumerateAllVerticesConst() const override;


public:
    //------------<Sets/Gets>----------------
    /** \brief Returns a list of vertices. Should be used with a special care! */
    Str2VertexMap& getVertices() { return _vertices; }

    /** \brief Returns a constant list of vertices */
    const Str2VertexMap& getVertices() const { return _vertices; }

    /** \brief Returns a reverse lookup map ptr */
    const Vertex2StrMap* getReverseLookupMap() const {return _reverseLookupMap; }


protected:
    MapNamedVerticesStorage(const MapNamedVerticesStorage&);                 // Prevent copy-construction
    MapNamedVerticesStorage& operator=(const MapNamedVerticesStorage&);      // Prevent assignment


protected:
    /** \brief Adds a new vertex w/o checking for existance one with the same name */
    BidiGraphVertex* addNewVertexInternal(const std::string& name);


protected:

    /** \brief Linked graph */
    BidiGraph* _graph;

    /** \brief Stores all graph's vertices as a map */
    Str2VertexMap _vertices;

    /** \brief Additional map for rapid search of vertices name*/
    Vertex2StrMap* _reverseLookupMap;

}; // class MapNamedVerticesStorage


//==============================================================================

/** \brief Interface declares a method solving if a new edge between given vertices can be added */
class LDOPA_API IEdgeAddChecker {
public:

    /** \brief Returns true if a given a new edge can be added between given vertices */
    virtual bool ifAnEdgeCanBeAdded(BidiGraphVertex* srcv, BidiGraphVertex* dstv) = 0;

protected:
    /** \brief protected Destructor: It is not intended to delete an object through this interface!! */
    ~IEdgeAddChecker() {};
}; // class IVertexAddChecker


//==============================================================================


typedef xi::collections::IEnumerator_obsolete1<BidiGraphEdge> IEdgesEnumerator;
typedef xi::collections::IConstEnumerator_obsolete1<BidiGraphEdge> IEdgesEnumeratorConst;


/** \brief Declares interfaces determining storage engine for graph edges.
*
*  An object of the derived class is to be passed to a graph when it is being created.
*/
class LDOPA_API IEdgesStorage {
public:

    // types
    typedef std::list<BidiGraphEdge*> ListOfEdges;
public:
    /** \brief Virtual destructor: lifetime of descendant objects can be managed through this type ptr */
    virtual ~IEdgesStorage() {}

public:
    
    /** \brief Returns number of stored _uniq_ edges */
    virtual size_t getEdgesNum() const = 0;

    /** \brief Deletes all edges without affecting to vertices */
    virtual void deleteAllEdges() = 0; 

    /** \brief Tries to add a new edge between this vertex and a given vertex.
    *  If there a similar edges already is already presented and allowDuplicate is
    *  false, no edge is added and nullptr is returned. If both vertices belong
    *  to different graphs, an exception is thrown
    */
    virtual BidiGraphEdge* addEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        bool allowDuplicate = true) = 0;

    /** \brief Overloaded method for adding edge special  object for checking adding allowance */
    virtual BidiGraphEdge* addEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        IEdgeAddChecker* addChecker) = 0;


    /** \brief If an edge between the vertex and a given vertex exists, return it (first occurence),
        otherwise adds a new one and returns it */
    virtual BidiGraphEdge* getOrAddNewEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex) = 0;

    /** \brief Checks if there at least one edges between given vertices exists */
    virtual bool isOutEdgeExists(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex) const = 0;

    /** \brief Looks for an output edge between given vertices and if one exists, returns it (first).
    Otherwise returns nullptr*/
    virtual BidiGraphEdge* getFirstOutEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex) const = 0;

    /** \brief Looks for an input edge between a given vertex and the vertex and if one exists, returns it (first).
    Otherwise returns nullptr*/
    virtual BidiGraphEdge* getFirstInEdge(BidiGraphVertex* dstVertex, BidiGraphVertex* srcVertex) const = 0;

    /** \brief Put all output edges between two given vertices to a provided list (append)*/
    virtual void getAllEdges(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        ListOfEdges& resList) const = 0;

    /** \brief Put all input edges between two given vertices to a provided list (append)*/
    virtual void getAllInEdges(BidiGraphVertex* dstVertex, BidiGraphVertex* srcVertex,
        ListOfEdges& resList) const = 0;

    /** \brief Put all output edges between all vertices to a provided list (append)*/
    virtual void getAllEdges(ListOfEdges& resList) const = 0;

    ///** \brief Put all input edges between all vertices to a provided list (append)*/
    //virtual void getAllInEdges(ListOfEdges& resList) const = 0;

    /** \brief \returns an associated graph */
    virtual BidiGraph* getLinkedGraph() const = 0;

    /** \brief Sets an associated graph */
    virtual void setLinkedGraph(BidiGraph* gr) = 0;

    /** \brief Returns enumerator for enumerating all edges (enumerator should be deleted then) */
    virtual IEdgesEnumerator* enumerateAllEdges() = 0;

    /** \brief Returns const enumerator for enumerating all edges (enumerator should be deleted then) */
    virtual IEdgesEnumeratorConst* enumerateAllEdgesConst() const = 0;
    

}; // IEdgesStorage

//==============================================================================

/** \brief Defines a multimap-based edges storage with both outpu and input edges.
 *
 *  Storing output edges is standard way to represent edges. Storing input edges as well
 *  allows speed up for some algorithms, but need up to 2 times more memory for storing edges.
 */
class LDOPA_API OutInMMapEdgeStorage : public IEdgesStorage {
public:
    // types
    typedef std::pair<BidiGraphVertex*, BidiGraphVertex*> PairOfVertices;
    typedef std::multimap<PairOfVertices, BidiGraphEdge*> PairOfVertices2EdgeMMap;
    typedef PairOfVertices2EdgeMMap::iterator PairOfVertices2EdgeMMapIterator;
    typedef PairOfVertices2EdgeMMap::const_iterator PairOfVertices2EdgeMMapConstIterator;

    typedef xi::collections::Enumerator4Iterator_obsolete1<BidiGraphEdge, PairOfVertices2EdgeMMapConstIterator> MyEnumerator;
    typedef xi::collections::ConstEnumerator4Iterator_obsolete1<BidiGraphEdge, PairOfVertices2EdgeMMapConstIterator> MyConstEnumerator;


public:
public:
    /** \brief Initialize with a linked graph. */
    OutInMMapEdgeStorage(BidiGraph* gr = nullptr);
    virtual ~OutInMMapEdgeStorage(); // {}

public:

    // IEdgesStorage::
    virtual void deleteAllEdges() override; // { cleanMapsInternal(); };

    // IEdgesStorage::
    virtual size_t getEdgesNum() const override;

    // IEdgesStorage::
    virtual BidiGraphEdge* addEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        bool allowDuplicate = true) override;
   
    // IEdgesStorage::
    virtual BidiGraphEdge* addEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        IEdgeAddChecker* addChecker) override;

    // IEdgesStorage::
    virtual BidiGraphEdge* getOrAddNewEdge(BidiGraphVertex* srcVertex, 
        BidiGraphVertex* dstVertex) override;

    // IEdgesStorage::
    virtual bool isOutEdgeExists(BidiGraphVertex* srcVertex, 
        BidiGraphVertex* dstVertex) const override;

    // IEdgesStorage::
    virtual BidiGraphEdge* getFirstOutEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex) const;

    // IEdgesStorage::
    virtual BidiGraphEdge* getFirstInEdge(BidiGraphVertex* dstVertex, BidiGraphVertex* srcVertex) const;

    // IEdgesStorage::
    virtual void getAllEdges(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex, 
                ListOfEdges& resList) const override;

    // IEdgesStorage::
    virtual void getAllEdges(ListOfEdges& resList) const override;

    // IEdgesStorage::
    virtual void getAllInEdges(BidiGraphVertex* dstVertex, BidiGraphVertex* srcVertex, 
                ListOfEdges& resList) const override;

    // IEdgesStorage::
    virtual BidiGraph* getLinkedGraph() const override;

    // IVerticesStorage::
    virtual void setLinkedGraph(BidiGraph* gr) override;

    // IVerticesStorage::
    virtual IEdgesEnumerator* enumerateAllEdges()  override;

    // IVerticesStorage::
    virtual IEdgesEnumeratorConst* enumerateAllEdgesConst() const override;
public:


    /** \brief Overloaded version returning an iterator throug given ref */
    void getFirstOutEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        PairOfVertices2EdgeMMapIterator& elIt);

    /** \brief Overloaded version returning a CONST iterator throug given ref */
    void getFirstOutEdge(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex,
        PairOfVertices2EdgeMMapConstIterator& elIt) const;

    /** \brief Overloaded version returning an iterator throug given ref */
    void getFirstInEdge(BidiGraphVertex* dstVertex, BidiGraphVertex* srcVertex,
        PairOfVertices2EdgeMMapIterator& elIt);

    /** \brief Overloaded version returning a CONST iterator throug given ref */
    void getFirstInEdge(BidiGraphVertex* dstVertex, BidiGraphVertex* srcVertex,
        PairOfVertices2EdgeMMapConstIterator& elIt) const;


public:
    /** \brief Checks if a pair of given of vertices is no null. Throws an exception, if it is */
    static void check4NullVertices(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex);

protected:
    OutInMMapEdgeStorage(const OutInMMapEdgeStorage&);                 // Prevent copy-construction
    OutInMMapEdgeStorage& operator=(const OutInMMapEdgeStorage&);      // Prevent assignment

protected:

    /** \brief Clean all maps storing edges and deletes edges objects */
    void cleanMapsInternal();

    /** \brief Internal method for creating and adding to both vertices a new edge */
    BidiGraphEdge* addNewEdgeInternal(BidiGraphVertex* srcVertex, BidiGraphVertex* dstVertex);

protected:
    /** \brief Linked graph */
    BidiGraph* _graph;

    /** \brief Real storage for all output edges*/
    PairOfVertices2EdgeMMap _outEdges;

    /** \brief Real storage for all input edges*/
    PairOfVertices2EdgeMMap _inEdges;

}; // class OutInMMapEdgeStorage 


//==============================================================================

/** \brief Bi-directed graph implementation based on pointers concept.
 *
 *	There is no attributes associated with nodes and vertex. Instead, a special
 *	bundle object should use unique vertices and edges pointers with needful
 *	attributes maps.
 */
class LDOPA_API BidiGraph
{

public:
    //------------<Constructors and destructor>----------------
    
    /** \brief Default constructor with ListVerticesStorage as a storage engine */
    BidiGraph(IVerticesStorage* vs = nullptr, IEdgesStorage* es = nullptr);
    ~BidiGraph();


public:
    //------------<Main graph interface>----------------

    /** \brief Clears all vertices and related edges */
    void clearGraph();

     
    //------------<Sets/Gets>----------------
    
    /** \brief Return embedded vertices storage */
    IVerticesStorage* getVertexStorage() { return _vertStorage; }
    
    /** \brief Return embedded vertices storage. Const overload. */
    const IVerticesStorage* getVertexStorage() const { return _vertStorage; }
    
    /** \brief  Return embedded edges storage */
    IEdgesStorage* getEdgeStorage() { return _edgesStorage; }

    /** \brief Return embedded edge storage. Const overload. */
    const IEdgesStorage* getEdgeStorage() const { return _edgesStorage; }


protected:
    BidiGraph(const BidiGraph&);                 // Prevent copy-construction
    BidiGraph& operator=(const BidiGraph&);      // Prevent assignment

protected:
    //------------<Protected methods>--------------------------


protected:
    //------------<Protected fields>---------------------------


    /** \brief Vertices storage engine. Graph manages its lifetime */
    IVerticesStorage* _vertStorage;

    /** \brief Edges storage */
    IEdgesStorage* _edgesStorage;

}; // class BidiGraph

//==============================================================================

/** \brief Defines a vertex object type
 *
 *  We use lazy initialization of in- and out vertices lists, so until no input or output edges 
 *	to the vertex is established, there is no list created.
 *	Also, a compress method is provided to delete unused (empty) lists of edges/vertices.
 */
class LDOPA_API BidiGraphVertex
{
public:
    BidiGraphVertex(BidiGraph* ownerGr);
    ~BidiGraphVertex();

public:
    //------------<Sets/Gets>----------------------------------
    
    /** \brief Returns a ptr to an owner graph */
    BidiGraph* getOwnerGraph() { return _ownerGr; }

protected:
    BidiGraphVertex(const BidiGraphVertex&);                 // Prevent copy-construction
    BidiGraphVertex& operator=(const BidiGraphVertex&);      // Prevent assignment

protected:
    //------------<Protected fields>---------------------------

    /** \brief Owner graph object */
    BidiGraph* _ownerGr;   
}; // class BidiGraphVertex



//==============================================================================


class LDOPA_API BidiGraphEdge
{
public:
    //BidiGraphEdge(BidiGraph* gr, BidiGraphVertex* srcV, BidiGraphVertex* dstV);
    BidiGraphEdge(BidiGraphVertex* srcV, BidiGraphVertex* dstV);
    ~BidiGraphEdge();

public:
    //------------<Sets/Gets>----------------------------------
    /** \brief Returns a source vertex */
    BidiGraphVertex* getSrcVertex() { return _srcVertex; } 

    /** \brief const overloaded version of getSrcVertex() */
    const BidiGraphVertex* getSrcVertex() const { return _srcVertex; }

    /** \brief Returns a destination vertex */
    BidiGraphVertex* getDstVertex() { return _dstVertex; } 

    /** \brief Returns a destination vertex */
    const BidiGraphVertex* getDstVertex() const { return _dstVertex; }

protected:
    BidiGraphEdge(const BidiGraphEdge&);                 // Prevent copy-construction
    BidiGraphEdge& operator=(const BidiGraphEdge&);      // Prevent assignment


protected:
    //------------<Protected fields>---------------------------

    /** \brief Source vertex */
    BidiGraphVertex* _srcVertex;

    /** \brief Destination vertex */
    BidiGraphVertex* _dstVertex;

}; // class BidiGraphEdge

} //  namespace xi::ldopa::


//==============================================================================
// Template specifications from xi::cols:: namespace that cannot be defined inside
// xi::ldopa:: namespace
//==============================================================================

// namespace xi::cols::
//namespace cols {

namespace collections {

/** \brief Traits for MapNamedVerticesStorage */
template <>
struct /*collections::*/Enumerator4Iterator_traits_obsolete1 < ldopa::ListVerticesStorage::VerticesListConstIterator >
{
    ldopa::ListVerticesStorage::VerticesListConstIterator::value_type
        getValue(const ldopa::ListVerticesStorage::VerticesListConstIterator & cur)
    {
        return *cur;
    }
}; // template <> struct StorageIterator_traits<...>



/** \brief Traits for MapNamedVerticesStorage */
template <>
struct /*collections::*/Enumerator4Iterator_traits_obsolete1 < ldopa::MapNamedVerticesStorage::Str2VertexMapConstIterator >
{
    ldopa::ListVerticesStorage::VerticesListConstIterator::value_type
        getValue(const ldopa::MapNamedVerticesStorage::Str2VertexMapConstIterator & cur)
    {
        return cur->second;
    }
}; // template <> struct StorageIterator_traits<...>


/** \brief Traits for MapNamedVerticesStorage */
template <>
struct /*collections::*/Enumerator4Iterator_traits_obsolete1 < ldopa::OutInMMapEdgeStorage::PairOfVertices2EdgeMMapConstIterator >
{
    //ListVerticesStorage::VerticesListConstIterator::value_type
    ldopa::BidiGraphEdge*
        getValue(const ldopa::OutInMMapEdgeStorage::PairOfVertices2EdgeMMapConstIterator & cur)
    {
        return cur->second;
    }
}; // template <> struct StorageIterator_traits<...>

} // namespace collections
} // namespace xi

#endif // XI_LDOPA_GRAPHS_BIDIRGRAPH_H_
