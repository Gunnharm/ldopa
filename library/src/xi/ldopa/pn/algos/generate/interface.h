////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri net Library: Base interface for Process Mining algorithms
/// \author    ganvas
/// \version   0.1.0
/// \date      2025-03-20
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_GENERATE_INTERFACE_H_
#define XI_LDOPA_PN_ALGOS_GENERATE_INTERFACE_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/pn/models/evlog_ptnets.h"

namespace xi { namespace ldopa { namespace pn {;   //

/** \brief Base interface for algorithms that produce Petri nets
 */
class LDOPA_API IPetriNetMiner {
public:
    //----<Types>----
    typedef EventLogPetriNet<> PN;

    /** \brief Virtual destructor */
    virtual ~IPetriNetMiner() {}

    /** \brief Main mining method that produces a Petri net
     *  \returns Newly created Petri net that caller must manage
     */
    virtual PN* mine(eventlog::IEventLog& log) = 0;
};

}}} // namespace xi { namespace ldopa { namespace pn {

#endif // XI_LDOPA_PN_ALGOS_GENERATE_INTERFACE_H_