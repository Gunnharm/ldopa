// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/graphs/algos/grviz/graph_dot_writer.h"

// ldopa
#include "xi/ldopa/utils.h"

// std
#include <fstream>

namespace xi { namespace ldopa {; //

//==============================================================================
// class GraphDotWriter
//==============================================================================

const char* GraphDotWriter::DEF_IND_STR = "    ";    // 4 spaces

GraphDotWriter::GraphDotWriter(IGraphVisitor* graphVisitor, std::ostream* ostr)
    : _graphVisitor(graphVisitor),
    _ostr(ostr),
    _ofstr(nullptr),
    _indent(DEF_IND_STR)
    //_ts(nullptr)
{
    //createAssociatedObjects();
    //markUpLog();
}

//------------------------------------------------------------------------------

GraphDotWriter::GraphDotWriter(IGraphVisitor* graphVisitor,
    const std::string& fileName, std::ios_base::openmode mode)
    : _graphVisitor(graphVisitor),
    _ostr(nullptr),
    _ofstr(nullptr),
    _fileName(fileName),
    _fOpenMode(mode),
    _indent(DEF_IND_STR)
{

}

//------------------------------------------------------------------------------

GraphDotWriter::GraphDotWriter(IGraphVisitor* graphVisitor)
    : GraphDotWriter(graphVisitor, "")                                   // delegating
{
}

//------------------------------------------------------------------------------

GraphDotWriter::~GraphDotWriter()
{
    closeFile();
}

//------------------------------------------------------------------------------

bool GraphDotWriter::openFile()
{
    // if a file is already open, do nothing
    if (isFileOpen())
        return true;

    // if a log isn't open, then no istr is associated, so we need to create a new one
    // based on input file stream
    createOstrFile();

    bool openOk = isFileOpen();
    return openOk; // isOpen();
}

//------------------------------------------------------------------------------

void GraphDotWriter::closeFile()
{
    // if no log open, do nothing
    if (!isFileOpen())
        return;

    closeOstrFile();
}

//------------------------------------------------------------------------------

bool GraphDotWriter::isFileOpen()
{
    return (_ostr != nullptr);
}

//------------------------------------------------------------------------------


void GraphDotWriter::setFileName(const std::string& fn)
{
    if (isFileOpen())
        throw LdopaException("Can't change a file name while a file is open");

    _fileName = fn;
}

//------------------------------------------------------------------------------

void GraphDotWriter::createOstrFile()
{
    if (_ofstr)
        throw LdopaException("Can't create second ofstream with active one");

    _ofstr = new std::ofstream(_fileName);
    if (_ofstr->fail())
    {
        delete _ofstr;
        _ofstr = nullptr;

        throw LdopaException::f("File %s cannot be open", _fileName.c_str());
    }

    // if it's opened successfully, connect f-stream with i-stream
    _ostr = _ofstr;
}

//------------------------------------------------------------------------------

void GraphDotWriter::closeOstrFile()
{
    if (!_ofstr)
        return;             // just do nothing, hence it is alaready ok

    delete _ofstr;
    _ofstr = nullptr;
    _ostr = nullptr;        // as it is rigidly connected with a file stream
}

//------------------------------------------------------------------------------

void GraphDotWriter::write()
{
    if (!_graphVisitor)
        throw LdopaException("No graph element visitor is given");

    // anyway try to open file
    openFile();

    if (!_ostr)
        throw LdopaException("Can't write a TS: an output stream is not set");

    visitGraph();
}

//------------------------------------------------------------------------------

void GraphDotWriter::visitGraph() const
{
    IGraphVisitor::ParValStrCollection parList;
    const std::string grName = _graphVisitor->visitGraph(parList);

    // TODO: вывод заголовка графа и СООТВЕТСТВУЮЩИХ АТРИБУТОВ
    *_ostr << "digraph \"" << grName << "\" {\n";

    // 2) visit all vertices
    visitGraphVertices();

    // 3) visit all edges
    visitGraphEdges();

    *_ostr << "}\n";
}

//------------------------------------------------------------------------------

void GraphDotWriter::visitGraphVertices() const
{
    std::string vid;
    IGraphVisitor::ParValStrCollection parList;

    for (_graphVisitor->resetVertexEnum();
        _graphVisitor->visitVertex(vid, parList);)
    {
        std::string parStr;
        formParamValueStr(parList, parStr);

        // output node name and params str
        *_ostr << _indent << vid;
        //*_ostr << vid;

        if (!parStr.empty())
            *_ostr << " " << parStr;
        *_ostr << std::endl;

        parList.clear();        // ready for next invokation
    }
}

//------------------------------------------------------------------------------

void GraphDotWriter::visitGraphEdges() const
{
    IGraphVisitor::StrStrPair nodesNames;
    IGraphVisitor::ParValStrCollection parList;

    for (_graphVisitor->resetEdgeEnum();
        _graphVisitor->visitEdge(nodesNames, parList);)
    {
        std::string parStr;
        formParamValueStr(parList, parStr);

        // output node name and params str
        *_ostr << _indent;
        *_ostr << nodesNames.first << " -> " << nodesNames.second;

        if (!parStr.empty())
            *_ostr << " " << parStr;
        *_ostr << std::endl;

        parList.clear();        // ready for next invokation
    }

}


//------------------------------------------------------------------------------
// For a given param-value list creates a DOT-compatible string representation
// and puts it to the end of a given str
//------------------------------------------------------------------------------
void GraphDotWriter::formParamValueStr(const IGraphVisitor::ParValStrCollection& parList,
    std::string& res)
{
    IGraphVisitor::ParValStrCollectionConstIter it = parList.begin();

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


}} // namespace xi { namespace ldopa
