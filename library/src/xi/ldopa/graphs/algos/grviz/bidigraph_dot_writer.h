///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Classes for writing graphviz-compatible intermediate data from 
/// BiDi graph from BidiGraph
///
/// \deprecated
///
///////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_GRAPHS_GRVIZ_BIDIGRAPHDOTWRITER_H_
#define XI_LDOPA_GRAPHS_GRVIZ_BIDIGRAPHDOTWRITER_H_

#pragma once

#include "xi/ldopa/graphs/bidigraph.h"

// std
#include <string>
#include <memory>
#include <utility>  // pair

namespace xi { namespace ldopa {



/** \brief Declare interface for methods invoked when a node or an edge is visited
 *
 *  Interface provides with methods for filling up an outpu DOT with a needful DOT-attributes
 *  based on abstract graph attributes.
 */
class IBidiGraphElVisitor {
public:
    // types
    typedef std::pair<std::string, std::string> StrStrPair;
    typedef std::list<StrStrPair> ListOfParamValueStr;
    typedef ListOfParamValueStr::const_iterator ListOfParamValueStrConstIter;
public:

    /** \brief Visits a graph. \returns graph name */
    virtual std::string visitGraph(const BidiGraph* gr, ListOfParamValueStr& attrList) = 0;

    /** \brief Visits a graph node. \returns node's ID  */
    virtual std::string visitVertex(const BidiGraphVertex* vert, ListOfParamValueStr& attrList) = 0;

    /** \brief Visits a graph edge. \returns begin and end nodes names */
    virtual StrStrPair visitEdge(const BidiGraphEdge* edge, ListOfParamValueStr& attrList) = 0;

protected:
    /** \brief protected Destructor: It is not intended to delete an object through this interface!! */
    ~IBidiGraphElVisitor() {};
}; // class IBidiGraphElVisitor

//======================================================================================

/** \brief Class writes a given ldopa-graph in  DOT format.
 *
 *  Constructs with ldopa-graph and a graph element visitor object.
 *  Ex \a DotWriter class. Now, this name is assigned to a more generic writer.
 */
class BidiGraphDotWriter
{
public:
    // types
    ///typedef std::pair<std::string, std::string> ParamValueStrs;
    typedef std::shared_ptr<IVertexEnumerator>      VertEnumeratorShPtr;
    typedef std::shared_ptr<IVertexEnumeratorConst> VertEnumeratorConstShPtr;

    typedef std::shared_ptr<IEdgesEnumerator>       EdgeEnumeratorShPtr;
    typedef std::shared_ptr<IEdgesEnumeratorConst>  EdgeEnumeratorConstShPtr;
public:
    /** \brief Default indentation string */
    static const char* DEF_IND_STR;
public:
    
    /** \brief Constructs with a name for output text file */
    BidiGraphDotWriter(const BidiGraph* gr, IBidiGraphElVisitor* grelVisitor, const std::string& fileName);
    
    /** \brief Destructor */
    ~BidiGraphDotWriter();

public:
    /** \brief For a given param-value list creates a DOT-compatible string representation 
        and puts it to the end of a given str*/
    static void formParamValueStr(const IBidiGraphElVisitor::ListOfParamValueStr& parList, std::string& res);
public:
    
    /** \brief Creates dot output */
    void dotIt();

protected:
    /** \brief Visits graphs and extract its attributes. Internal method */
    void visitGraph() const;

    /** \brief Visits a given graph vertex and extract its attributes. Internal method */
    void visitGraphVertex(const BidiGraphVertex* vert) const;

    /** \brief Visits a given graph edge and extract its attributes. Internal method */
    void visitGraphEdge(const BidiGraphEdge* edge) const;
    
    /** \brief For a given filename, opens and stores ostream */
    void createOstreamFile(const std::string& fileName);

    /** \brief Closes and destroy an ofstream object if it has been created by class */
    void finalizeOstream();
protected:

    /** \brief ldopa-graph */
    const BidiGraph* _gr;
    
    /** \brief Graph Element Visitor object */
    IBidiGraphElVisitor* _grelVisitor;

    /** \brief Output stream for DOT-text output */
    std::ostream* _ostr;

    /** \brief A file-base output stream for the case of local creating */
    std::ofstream* _ofstr;

    /** \brief Indentation string */
    std::string _indent;
}; // class BidiGraphDotWriter


}} // namespace xi { namespace ldopa

#endif // XI_LDOPA_GRAPHS_GRVIZ_BIDIGRAPHDOTWRITER_H_
