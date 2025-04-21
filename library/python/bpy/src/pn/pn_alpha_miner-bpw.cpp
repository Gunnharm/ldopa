////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for Alpha Miner algorithm.
/// \author    ganvas
/// \version   0.1.0
/// \date      21.04.2025
///
////////////////////////////////////////////////////////////////////////////////

// boost
#include <boost/python.hpp>

// ldopa
#include "xi/ldopa/pn/algos/generate/alpha/alpha_miner.h"

namespace bp = boost::python;

struct PnAlphaMiner_PyWrappers
{
    using AlphaMiner = xi::ldopa::pn::alpha::AlphaMiner;
    using IEventLog  = xi::ldopa::pn::alpha::AlphaMiner::IEventLog;
    using PN         = xi::ldopa::pn::alpha::AlphaMiner::PN;

    static AlphaMiner::PN* alphaMinerMine(AlphaMiner & self, IEventLog & log)
    {
        return self.mine(log);
    }

    static void exportClasses()
    {
        bp::class_<AlphaMiner, boost::noncopyable>("AlphaMiner", bp::init<>())
            .def("mine", 
                 &alphaMinerMine,
                 bp::return_value_policy<bp::manage_new_object>(),
                 "Запускает алгоритм alpha-mine на переданном журнале событий.")
        ;
    }
};

void export_PnAlphaMiner()
{
    PnAlphaMiner_PyWrappers::exportClasses();
}