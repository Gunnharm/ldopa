////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Contains interfaces needed to send callback messages about execution 
///            progress of some algorithm.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      05.06.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_UTILS_PROGRESS_CALLBACK_H_
#define XI_LDOPA_UTILS_PROGRESS_CALLBACK_H_

//#pragma once


// ldopa
#include "xi/ldopa/ldopa_dll.h"
#include "xi/ldopa/utils.h"

namespace xi { namespace ldopa { ;   //

class LDOPA_API IProgressCallback {
public:
    /** \brief Defines callback results. */
    enum Res
    {
        ResOK,                  ///< Continue execution.
        ResCancel               ///< Break execution.
    }; //

    typedef unsigned char Byte; ///< Alias for unsigned byte datatype.
public:
    //----<Main interface>----
    
    /** \brief Sends a notification on progress, value from 0 to 100, in percents. 
     * 
     *  Caller might return a result determining whether execution can be continued (OK)
     *  or not (Cancel).
     */
    virtual Res progress(Byte progress) = 0;
protected:
    /** \brief Not for lifetime management. */
    ~IProgressCallback() {};
}; // class IProgressCallback

}} // namespace xi { namespace ldopa 


#endif // XI_LDOPA_UTILS_PROGRESS_CALLBACK_H_
