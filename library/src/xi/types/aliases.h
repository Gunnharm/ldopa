////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     Common types monikers.
/// \author    Sergey Shershakov
/// \version   0.3.1
/// \date      28.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Contatins typedefs for very common types like unsigned... 
///
////////////////////////////////////////////////////////////////////////////////

#pragma     // чтобы заткнулась писать про PCH с товарищами

#ifndef XILIB_XI_TYPES_ALIASES_H_
#define XILIB_XI_TYPES_ALIASES_H_


namespace xi {
namespace types {



// Символ _MSC_VER определяет версию компилятора MSVCC. Если он определен, значит это Visual Studio!
#ifdef _MSC_VER

   typedef __int64            TInt64;             // определение для Windows'аую-
   typedef unsigned __int64   TQword;
#else
    // https://stackoverflow.com/questions/3108220/porting-windows-code-what-to-use-instead-of-int64-tmain-and-tchar
    typedef long long            TInt64;             // определение для Windows'а
    typedef unsigned long long   TQword;
#endif // _MSC_VER


typedef int                   TBool;
typedef unsigned char         TByte;
typedef unsigned short        TWord;
typedef unsigned long         TDword;

typedef float                 TFloat;

typedef TDword*               TPdword;
typedef void*                 TPvoid;
typedef const void*           TPCvoid;   // переименован из TLPCvoid

typedef int                   TInt;
typedef unsigned int          TUint;
typedef unsigned int*         TPUint;


}; // namespace types
}; // namespace xi


#endif // XILIB_XI_TYPES_ALIASES_H_
