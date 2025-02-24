////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Contains class definition for simplification of calculation of elapsed time.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      15.09.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_UTILS_ELAPSED_TIME_H_
#define XI_LDOPA_UTILS_ELAPSED_TIME_H_

//#pragma once


#include <ctime>


// utility macros
#define XI_LDOPA_ELAPSEDTIME_START(timer) clock_t timer = clock();
#define XI_LDOPA_ELAPSEDTIME_STOP(timer) _elapsed = (double)(clock() - timer) * 1000.0 / CLOCKS_PER_SEC;


namespace xi { namespace ldopa { ;   //

/** \brief Helps calculate elapsed time for an algorithm. 
 *
 *  Can be simply used for multiple inheritance.
 */
class /*LDOPA_API*/ ElapsedTimeStore {
public:
    ElapsedTimeStore() : _elapsed(0) {}

    /// Returns elapsed time (ms) for the last synthesis run.
    double getElapsed() const { return _elapsed; }
protected:
    /// Stores elapsed time (ms) for the last synthesis run.
    double _elapsed;
}; // class ElapsedTimeStore



}} // namespace xi { namespace ldopa 


#endif // XI_LDOPA_UTILS_ELAPSED_TIME_H_
