// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"
//# 
#include "xi/ldopa/graphs/algos/grviz/bidigraph_dot_writer.h"

// ldopa
#include "xi/ldopa/utils.h"

// std
#include <fstream>

namespace xi { namespace ldopa {; //

//==============================================================================
// class BidiGraphDotWriter
//==============================================================================

const char* BidiGraphDotWriter::DEF_IND_STR = "    ";    // 4 spaces


//------------------------------------------------------------------------------
// Constructs with a name for output text file 
//------------------------------------------------------------------------------
BidiGraphDotWriter::BidiGraphDotWriter(const BidiGraph* gr, 
        IBidiGraphElVisitor* grelVisitor, const std::string& fileName)
    : _gr(gr),
    _grelVisitor(grelVisitor),
    _ostr(nullptr),
    _ofstr(nullptr),
    _indent(DEF_IND_STR)
{
    createOstreamFile(fileName);
}



//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
BidiGraphDotWriter::~BidiGraphDotWriter()
{
    finalizeOstream();
}

//------------------------------------------------------------------------------
// Creates dot output
//------------------------------------------------------------------------------
void BidiGraphDotWriter::dotIt()
{
    // some checks in advance
    if (_gr == nullptr)
        throw LdopaException("No graph to export is given");

    if (_grelVisitor == nullptr)
        throw LdopaException("No graph element visitor is given");

    if (_ostr == nullptr)
        throw LdopaException("No output stream is set");
    
    // do main action
    

    // 1) Visit Graph itself
    visitGraph();
}


//------------------------------------------------------------------------------
// Visits graph and extract its attributes
// Internal method
//------------------------------------------------------------------------------
void BidiGraphDotWriter::visitGraph() const
{
    IBidiGraphElVisitor::ListOfParamValueStr parList;
    const std::string grName = _grelVisitor->visitGraph(_gr, parList);

    // TODO: вывод заголовка графа и соответствующих атрибутов
    *_ostr << "digraph \"" << grName << "\" {\n";
   
    // 2) visit all vertices
    VertEnumeratorConstShPtr enp(_gr->getVertexStorage()->enumerateAllVerticesConst());

    while (enp->hasNext())
    {
        const BidiGraphVertex* v = enp->getNext();
        visitGraphVertex(v);
    }

    // 3) visit all edges
    EdgeEnumeratorConstShPtr edp(_gr->getEdgeStorage()->enumerateAllEdgesConst());

    while (edp->hasNext())
    {
        const BidiGraphEdge* e = edp->getNext();
        visitGraphEdge(e);
    }

    *_ostr << "}\n";
}


//------------------------------------------------------------------------------
// Visits a given graph vertex and extract its attributes
// Internal method
//------------------------------------------------------------------------------
void BidiGraphDotWriter::visitGraphVertex(const BidiGraphVertex* vert) const
{
    IBidiGraphElVisitor::ListOfParamValueStr parList;
    std::string nodeName = _grelVisitor->visitVertex(vert, parList);
    std::string parStr; // = " ";
    formParamValueStr(parList, parStr);

    // output node name and params str
    *_ostr << _indent;
    *_ostr << nodeName;

    if (!parStr.empty())
        *_ostr << " " << parStr;
    *_ostr << std::endl;
}


//------------------------------------------------------------------------------
// Visits a given graph edge and extract its attributes
// Internal method
//------------------------------------------------------------------------------
void BidiGraphDotWriter::visitGraphEdge(const BidiGraphEdge* edge) const
{
    IBidiGraphElVisitor::ListOfParamValueStr parList;
    IBidiGraphElVisitor::StrStrPair nodesNames = _grelVisitor->visitEdge(edge, parList);
    
    std::string parStr; // = " ";
    formParamValueStr(parList, parStr);

    // output node name and params str
    *_ostr << _indent;
    *_ostr << nodesNames.first << " -> " << nodesNames.second;
    //*_ostr << parStr;
    if (!parStr.empty())
        *_ostr << " " << parStr;
    *_ostr << std::endl;
}



//------------------------------------------------------------------------------
// For a given param-value list creates a DOT-compatible string representation
// and puts it to the end of a given str
//------------------------------------------------------------------------------
void BidiGraphDotWriter::formParamValueStr(const IBidiGraphElVisitor::ListOfParamValueStr& parList,
    std::string& res)
{
    IBidiGraphElVisitor::ListOfParamValueStrConstIter it = parList.begin();
    
    // check if no param at all
    if (it == parList.end())
        return;

    res += '[';
    bool first = true;

    for (; it != parList.end(); ++it)
    {
        if (!first)
            res += ',';

        res += it->first;
        res += '=';
        res += it->second;

        first = false;
    }

    res += ']';
}


//------------------------------------------------------------------------------
// For a given filename, opens and stores ostream
// Internal
// If a ostream is already open, do nothing
//------------------------------------------------------------------------------
void BidiGraphDotWriter::createOstreamFile(const std::string& fileName)
{
    if (_ofstr)                 // already created
        return;

    _ofstr = new std::ofstream(fileName, std::ofstream::out);
    
    _ostr = _ofstr;             // ostr == oFstr
}


//------------------------------------------------------------------------------
// Closes and destroy an ofstream object if it has been created by class
//------------------------------------------------------------------------------
void BidiGraphDotWriter::finalizeOstream()
{
    if (!_ofstr)      // if no ofstream object has been created at all
        return;

    _ofstr->close();
    delete _ofstr;

    _ostr = _ofstr = nullptr;
}



}} // namespace xi { namespace ldopa
