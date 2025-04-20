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
    
    AlphaMiner();

    virtual ~AlphaMiner();

    //----<IPetriNetMiner implementation>----
    virtual PN* mine(IEventLog* log) override;

private:
    //----<Alpha algorithm steps>----
    bool validateInput();
    void extractActivities();
    void buildDirectSuccession();
    void buildCausalDependency();
    void buildParallelRelation();
    void findMaximalSets();
    void constructPetriNet();

private:
    IEventLog* _log;                 ///< Input event log
    PN* _pn;                         ///< Output Petri net
    std::set<Attribute> _activities; ///< Set of all activities
    
    DirectSuccessionRelations<Attribute> _directSuccession;
    CausalDependencyRelations<Attribute> _causalDependency;
    ParallelRelations<Attribute> _parallel;

    /** \brief Структура для хранения максимальных множеств */
    struct MaximalSet {
        std::set<Attribute> input;   ///< Входные активности
        std::set<Attribute> output;  ///< Выходные активности
    };
    std::vector<MaximalSet> _maximalSets; ///< Вектор максимальных множеств
};

}}}} // namespace xi { namespace ldopa { namespace pn { namespace alpha {

#endif // XI_LDOPA_PN_ALGOS_ALPHA_ALPHA_MINER_H_