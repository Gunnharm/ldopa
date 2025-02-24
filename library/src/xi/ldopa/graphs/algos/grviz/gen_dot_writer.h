////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Graph Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      31.07.2018
/// \copyright (c) xidv.ru 2014Ц2018.
///            This source is for internal use only Ч Restricted Distribution.
///            All rights reserved.
///
/// Generic class template for output graph-based models as a DOT-files.
///
////////////////////////////////////////////////////////////////////////////////



#ifndef XI_LDOPA_GRAPHS_GRVIZ_GEN_DOT_WRITER_H_
#define XI_LDOPA_GRAPHS_GRVIZ_GEN_DOT_WRITER_H_

#pragma once

// std
#include <list>
#include <string>
#include <fstream>


namespace xi { namespace ldopa { namespace graph {


/** \brief Default visitor class.
 *
 *  -
 */
template <typename TGraph>
struct DefaultDotVisitor
{
    //----<Types>----
    typedef std::pair<std::string, std::string> StrStrPair;
    
    /** \brief List extension. */
    class ParamValueList : public std::list < StrStrPair >
    {
    public:
        /** \brief Appends a pair of param-value to the end of the list. */
        inline void append(const std::string& par, const std::string& val)
        {
            push_back(std::make_pair(par, val));
        }
    }; // class ParamValueList
    
    typedef typename ParamValueList::const_iterator ParamValueListCIter;

    //----<Helper methods>----

    /** \brief Creates a HEX representation of the given Uint value \a v prefixed by 'x'. */
    std::string makeUintHexId(unsigned int v)
    {
        // https://stackoverflow.com/questions/1042940/writing-directly-to-stdstring-internal-buffers
        char addrBuf[11];
        sprintf(addrBuf, "x%x", v);
        
        return std::string(addrBuf);
    }

    /** \brief Makes a formatted string correposing to the list of param-values. */
    std::string makeParamValueStr(const ParamValueList& parList)
    {
        ParamValueListCIter it = parList.begin();

        // check if no param at all
        if (it == parList.end())
            return "";

        std::string res = "[";

        bool first = true;

        for (; it != parList.end(); ++it)
        {
            if (!first)
                res += ',';
            else
                first = false;

            res += it->first;
            res += '=';
            res += it->second;            
        }

        res += ']';

        return res;
    }

    /** \brief Escapes a strings special symbols and enclose the result into dblquotes. */
    std::string makeEscapedString(const std::string& s)
    {
        std::string res;
        res.reserve(s.length() + 2);            // в минимальной версии
            
        res += '\"';
        for (char c : s) 
        {
            if (c == '"')           // замен€м кавычку на посл. \"
            {
                res += "\\\"";      
                continue;
            }

            if (c == '\\')          // замен€ем бэкслеш на два бекслеша
            {
                res += "\\\\";      
                continue;
            }

            // все остальное просто копируем
            res += c;
        }
        res += '\"';
        
        return res;
    }

    //----<Concept methods>----
//public:
    void outputHeader(std::ostream& str, const TGraph& gr, const char* grLbl)
    {
        str << "digraph G {\n";

        // если есть метка графа, добавим:
        if (grLbl)
            str << "    label=\"" << grLbl << "\";\n";

        str << "    node [width=0.5];\n";
    }

    void outputTail(std::ostream& str, const TGraph& gr)
    {
        str << "}\n";
    }
}; // class DefaultDotVisitor

//=============================================================================

/** \brief Generic DOT-writer.
 *
 *  \tparam TGraph typename for a graph-based model to output.
 *  \tparam TGraphVisitor traits class for individual visitors of graph elements.
 */

template <typename TGraph, typename TGraphVisitor = DefaultDotVisitor<TGraph> >
class GenDotWriter 
{
public:
    
    // Constructor.
    GenDotWriter(const TGraphVisitor& gv = TGraphVisitor())
        : _gv(gv)
    {
    }
public:
    /** \brief Writes a dump of the given model \a gr to a file with the name \a fn, 
     *  having the label \a grLbl and using the given visitor object \a gv.
     *
     *  -
     */
    void write(const std::string& fn, const TGraph& gr, const char* grLbl = nullptr)
    {
        //std::invalid_argument
        std::ofstream dfile(fn.c_str());
        if (!dfile.is_open())
            throw std::invalid_argument("Can't open dump file for GraphViz");

        // заголовок
        outputHeader(dfile, gr, grLbl);

        // тело
        outputBody(dfile, gr);

        // хвост
        outputTail(dfile, gr);

        dfile.flush();
    }

protected:


    /** \brief Outputs the main part (vertices and edges) of the graph to the output. */
    inline void outputBody(std::ostream& str, const TGraph& gr)
    {
        _gv.outputBody(str, gr);
    }


    /** \brief Outputs the header of the graph to the output. */
    inline void outputHeader(std::ostream& str, const TGraph& gr, const char* grLbl = nullptr)
    {
        _gv.outputHeader(str, gr, grLbl);
    }

    /** \brief Outputs the tail of the graph to the output. */
    inline void outputTail(std::ostream& str, const TGraph& gr)
    {
        _gv.outputTail(str, gr);
    }


protected:
    
    /** \brief Graph Visitor object. */
    TGraphVisitor _gv;
}; // class GenDotWriter 


//
}}} // namespace xi { namespace ldopa namespace graph {

#endif // XI_LDOPA_GRAPHS_GRVIZ_GENDOTWRITER_H_
