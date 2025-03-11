// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

//#include "xi/dpmodel/utils.h"
#include "xi/ldopa/utils.h"


// std
#include <stdio.h>          //  sprintf
#include <stdarg.h>

namespace xi { namespace ldopa {


//==============================================================================
// class DPMException
//==============================================================================


LdopaException LdopaException::f(const char * _Format, ...)
{
    char errMes[MAX_FORMAT_STR_SIZE];

    va_list argptr;
    va_start(argptr, _Format);
    vsnprintf(errMes, sizeof(errMes), _Format, argptr);
    va_end(argptr);

    return LdopaException(errMes, ERC_DEF);
}

//------------------------------------------------------------------------------

LdopaException LdopaException::f(const char * _Format, va_list argptr)
{
    char errMes[MAX_FORMAT_STR_SIZE];
    vsnprintf(errMes, sizeof(errMes), _Format, argptr);

    return LdopaException(errMes, ERC_DEF);
}


//------------------------------------------------------------------------------

LdopaException LdopaException::f(ErrorCode erCode, const char * _Format, ...)
{
    char errMes[MAX_FORMAT_STR_SIZE];

    va_list argptr;
    va_start(argptr, _Format);
    vsnprintf(errMes, sizeof(errMes), _Format, argptr);
    va_end(argptr);

    return LdopaException(errMes, erCode);
}

//------------------------------------------------------------------------------

LdopaException LdopaException::f(ErrorCode erCode, const char * _Format, va_list argptr)
{
    char errMes[MAX_FORMAT_STR_SIZE];
    vsnprintf(errMes, sizeof(errMes), _Format, argptr);

    return LdopaException(errMes, erCode);
}

}} // namespace xi { namespace ldopa

