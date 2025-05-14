////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri net Library: Alpha Miner algorithm implementation
/// \author    ganvas
/// \version   0.1.0
/// \date      2025-03-20
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_ALPHA_ALPHA_MINER_H_
#define XI_LDOPA_PN_ALGOS_ALPHA_ALPHA_MINER_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// ldopa
#include "xi/ldopa/pn/algos/generate/interface.h"
#include "xi/ldopa/eventlog/eventlog.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"
#include "relations.h"

namespace xi { namespace ldopa { namespace pn { namespace alpha {;   //

/** \brief Implementation of Alpha Miner algorithm
 */
class LDOPA_API AlphaMiner final : public IPetriNetMiner{
public:
    //----<Types>----
    using IEventLog = eventlog::IEventLog;
    using IEventTrace = eventlog::IEventTrace;
    using IEvent = eventlog::IEvent;
    using Attribute = IEventLog::Attribute;
    using PN = EventLogPetriNet<>;
    
    AlphaMiner() = default;
    virtual ~AlphaMiner() = default;

    //----<IPetriNetMiner implementation>----
    virtual PN* mine(IEventLog& log) override;

private:
    static constexpr size_t MAX_ACTIVITIES = 60;
    
    
};

}}}} // namespace xi { namespace ldopa { namespace pn { namespace alpha {

#endif // XI_LDOPA_PN_ALGOS_ALPHA_ALPHA_MINER_H_