///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Classes for writing an arbitrary graph as DOT text
///
/// Classes of the module replaces the most typed BidiGraphDotWriter, which
/// dealing with specific graph types; namely, BidiGraph. This module
/// provides with a generic writing tools dealing with specific graph types
/// through interfaces that hide an underlying graph storage engine entirely.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_GRAPHS_GRVIZ_GRAPHDOTWRITER_H_
#define XI_LDOPA_GRAPHS_GRVIZ_GRAPHDOTWRITER_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"


// std
#include <string>
#include <vector>
#include <ios>  // openmode

namespace xi { namespace ldopa {

/** \brief Declare interface for methods invoked when a graph, its vertices and edges are visited
 *
 *  Interface provides with methods for filling up an outpu DOT with a needful DOT-attributes
 *  based on abstract graph attributes.
 *
 *  \see IGrElVisitor
 */
class LDOPA_API IGraphVisitor {
public:
    // types
    typedef std::pair<std::string, std::string> StrStrPair;
    //typedef std::list<StrStrPair> ListOfParamValueStr;
    typedef std::vector<StrStrPair> ParValStrCollection;
    typedef ParValStrCollection::const_iterator ParValStrCollectionConstIter;
public:

    /** \brief Visits a graph
     *  \param attrList a list of graph parameters
     *  \returns graph name 
     */
    virtual std::string visitGraph(ParValStrCollection& attrList) = 0;

    /** \brief Visits another graph vertex (node) 
     *  \param vid out param for placing  ID of the current vertex
     *  \param attrList a list of vertex parameters
     *  \returns true, if another vertex has been proceeded; false, if no more vertex to visit
     */
    virtual bool visitVertex(std::string& vid, ParValStrCollection& attrList) = 0;

    /** \brief Resets a vertex enumerator object 
     *
     *  After calling this method invokation of visitVertex() will work from the very first vertex
     */
    virtual void resetVertexEnum() = 0;


    /** \brief Visits another graph edge 
     *  \param verts out param for placing IDs of source and destination vertices of of the current edge
     *  \param attrList a list of edge parameters
     *  \returns true, if another vertex has been proceeded; false, if no more vertex to visit
     */
    virtual bool visitEdge(StrStrPair& verts, ParValStrCollection& attrList) = 0;

    /** \brief Resets an edge enumerator object 
     *
     *  After calling this method invokation of visitEdge() will work from the very first vertex
     */
    virtual void resetEdgeEnum() = 0;

    // TODO: one need to add support of subgraphs, clusters and so on

protected:
    /** \brief protected Destructor: It is not intended to delete an object through this interface!! */
    ~IGraphVisitor() {}
}; // class IGraphVisitor


//==============================================================================

/** \brief Exporter of a arbitrary as a GraphViz text definition
 *
 *  
 */
class LDOPA_API GraphDotWriter
{
public:
    //-----<Const>-----
    /** \brief Default indentation string */
    static const char* DEF_IND_STR;
public:
    /** \brief Default constructor */
    GraphDotWriter(IGraphVisitor* graphVisitor = nullptr);

    /** \brief Initializa with a name of output file */
    GraphDotWriter(IGraphVisitor* graphVisitor, const std::string& fileName,
        std::ios_base::openmode mode = std::ios_base::out);

    /** \brief Initialize with a given stream object */
    GraphDotWriter(IGraphVisitor* graphVisitor, std::ostream* istr);

    /** \brief Destructor */
    ~GraphDotWriter();

protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    GraphDotWriter(const GraphDotWriter&);                 // Prevent copy-construction
    GraphDotWriter& operator=(const GraphDotWriter&);      // Prevent assignment

public:
    bool openFile();
    void closeFile();
    bool isFileOpen();
public:
    /** \brief Sets a filename for a log file. If a log is already open, an exception is thrown */
    void setFileName(const std::string& fn);

    /** \brief Returns a filename for a log */
    const std::string getFileName() const { return _fileName; }

    /** \brief Sets a Graph Element Visitor object */
    void setGraphVisitor(IGraphVisitor* grelVisitor) { _graphVisitor = grelVisitor;  }

    /** \brief Gets a Graph Element Visitor object */
    IGraphVisitor* getGraphVisitor() const { return _graphVisitor; }


    /** \brief Writes a TS that has been previously set 
     *
     *  If no TS has been set in advance, an exception is thrown
     */
    void write();

public:
    /** \brief For a given param-value list creates a DOT-compatible string representation
     *  and puts it to the end of a given str
     */
    static void formParamValueStr(const IGraphVisitor::ParValStrCollection& parList, std::string& res);

protected:
    /** \brief Tries to create an ostream based on file with a filename that is set in advance */
    void createOstrFile();

    /** \brief Tries to close an ofstream file if exists */
    void closeOstrFile();

    void visitGraph() const;

    /** \brief Visits all graph vertices */
    void /*GraphDotWriter::*/visitGraphVertices() const;

    /** \brief Visits all graph edges */
    void /*GraphDotWriter::*/visitGraphEdges() const;

protected:
    /** \brief Graph (Element) Visitor object */
    IGraphVisitor* _graphVisitor;

    /** \brief Output stream */
    std::ostream* _ostr;

    /** \brief A file-base output stream for the case of local creating */
    std::ofstream* _ofstr;

    /** \brief File name for maintaining an internal file stream */
    std::string _fileName;

    /** \brief Stores openmode for the internal file */
    std::ios_base::openmode _fOpenMode;

    /** \brief Indentation string */
    std::string _indent;
}; // class GraphDotWriter

//
}} // namespace xi { namespace ldopa

#endif // XI_LDOPA_GRAPHS_GRVIZ_GRAPHDOTWRITER_H_
