////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA DLL Import/Export
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      29.12.2016
/// \copyright (c) xidv.ru 2014–2016.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// 
///
////////////////////////////////////////////////////////////////////////////////



#ifndef XI_LDOPA_LDOPADLL_H_
#define XI_LDOPA_LDOPADLL_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LDOPADLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SIMPLEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

// см. https://support.microsoft.com/en-us/kb/168958 к вопросу об STL

#ifdef LDOPA_DLL

// #pragma message("LDOPA_DLL")

// транзитивной зависимые подключения связанных либ:
#define XILIB_DLL           // xilib



#ifdef LDOPA_DLL_EXPORTS
#   define LDOPA_API __declspec(dllexport)
#   define LDOPA_EXPIMP_TEMPLATE

    #pragma message("LDOPA_DLL_EXPORTS")    

#else
#   define LDOPA_API __declspec(dllimport)
#   define LDOPA_EXPIMP_TEMPLATE extern

#pragma message("NOT LDOPA_DLL_EXPORTS")  

#endif



// пробуем




#else

// #pragma message("NOT LDOPA_DLL")

#   define LDOPA_API

#endif // LDOPA_DLL


#endif // XI_LDOPA_LDOPADLL_H_
