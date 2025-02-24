///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Class adaptor for bidigraph with named vertices
///
/// A more elaborate module description
///
///////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_GRAPHS_NAMEDBIDIRGRAPH_H_
#define XI_LDOPA_GRAPHS_NAMEDBIDIRGRAPH_H_

//#pragma once

#include "xi/ldopa/graphs/bidigraph.h"


namespace xi { namespace ldopa {;   //


class NamedBidiGraph : public BidiGraph
{
public:

    //------------<Constructors and destructor>----------------

    /** \brief Default constructor with ListVerticesStorage as a storage engine */
    NamedBidiGraph(); // IVerticesStorage* vs, IEdgesStorage* es);
    ~NamedBidiGraph();
}; // class NamedBidiGraph


}} // namespace xi { namespace ldopa

#endif // 
