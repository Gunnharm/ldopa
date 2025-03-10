///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Export/Import switch for exporting symbols for DLL-projects.
///
/// TODO: This is still a question where this header should be placed:
/// now (04.02.2015) it is in /src/ root dir, but may be it should be placed
/// in /src/xi/ like in /xi/dpmodel/ project...
///
/// * XILIB_DECLARE_EXPORT_IMPORT — macro defines that _dll(export) or _dll(import)
///   macros should be used (use only with DLL-target version of lib!)
/// * XILIB_EXPIMP — macro defining if a symbol should be exported or imported instead
///   XILIB — project name, _EXPIMP — standard notation for this macro
/// * XILIB_EXPORT — is the macro is set, XILIB_EXPIMP is turned to "export",
///   otherwise ti "import"
///////////////////////////////////////////////////////////////////////////////

#ifndef XILIB_XILIB_DLL_H_
#define XILIB_XILIB_DLL_H_


/** \brief If the library is assembled in the framework of Qt DLL project,
 *  this option is used to point of sublaying Qt Global Macro-system
 */
#ifdef XI_USE_QT_GLOBAL_NOTATION
#   include <QtCore/qglobal.h>
#endif // XI_USE_QT_GLOBAL

// 
// 
// 

#ifdef XILIB_DLL

    // If the Qt notation is used...
    #ifdef XI_USE_QT_GLOBAL_NOTATION

    #   ifdef XILIB_DLL_EXPORTS
    #       define XILIB_API Q_DECL_EXPORT
    #   else
    #       define XILIB_API Q_DECL_IMPORT
    #   endif

    #else
    // Default approach (if the Qt notation is not used)

    #   ifdef XILIB_DLL_EXPORTS
    #       define XILIB_API __declspec(dllexport)
    #       define XILIB_EXPIMP_TEMPLATE
    #   else
    #       define XILIB_API __declspec(dllimport)
    #       define XILIB_EXPIMP_TEMPLATE extern
    #   endif

    #endif // XI_USE_QT_GLOBAL

#else
    // we have to declare macro anyway, but for this case it will be empty
#   define XILIB_API

#endif // XILIB_DLL


#endif // XILIB_XILIB_DLL_H_
