////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri net Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.08.2018
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Extensitions for PNRegSyntesizer for testing protected methods.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_TEST_PN_ALGOS_REGIONS_PN_SYNTHESIS_EXT_H_
#define XI_LDOPA_TEST_PN_ALGOS_REGIONS_PN_SYNTHESIS_EXT_H_

#pragma once

// ldopa dll
#include <xi/ldopa/utils.h>

// ldopa
#include "xi/ldopa/pn/algos/regions/pn_synthesis.h"

namespace xi { namespace ldopa { namespace pn {;   //

/** \brief Testing extension for the template class for synthesis a Petri net from a TS.
 *
 *  \tparam TS type of a transition system.
 *  \tparam PN type of a Petri net.
 *  
 *  Requirements for template parameter types.
 *  \a PN is a petri net, which:
 *  *   has a method for addinf a new transition, named addTransition(lbl), and
 *      obtaining const Label& lbl as a parameter, where Label is the type TS::TrLabel.
 *      
 *  By agreement, all protected methods that are exposed to test have a prefix "_t_"
 */
template<typename TS, typename PN>
class PnRegSynthesizer_Tester
    : public PnRegSynthesizer<TS, PN>
{
public:
    // additiona typedefs 
    typedef typename PnRegSynthesizer<TS, PN> Base;
public:
    // Constructors
    /** \brief Default constructor. */
    PnRegSynthesizer_Tester(Options opts = 0)
        : Base(opts)
    {
    }

    /** \brief Initializes the synthesizer with the given transition system \a ts. */
    PnRegSynthesizer_Tester(TS* ts, Options opts = 0)
        : Base(ts, opts)
    {
    }

    /** \brief Destructor. */
    virtual ~PnRegSynthesizer_Tester()
    {
        clearPN();
    }
public:

    // expose a protected methods

    inline void _t_mapTransitions() { mapTransitions();  }
    
    inline SplitLabel _t_getTransSplitLblInt(const TsTransition& t) const
    {
        return getTransSplitLblInt(t);
    }

    inline void _t_setLblSplitIndex(const Label& lbl, unsigned int ind)
    {
        setLblSplitIndex(lbl, ind);
    }


    inline void _t_setTrSplitIndex(const TsTransition& t, unsigned int ind)
    {
        setTrSplitIndex(t, ind);
    }

}; // class PnRegSynthesizer_Tester


}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_TEST_PN_ALGOS_REGIONS_PN_SYNTHESIS_EXT_H_
