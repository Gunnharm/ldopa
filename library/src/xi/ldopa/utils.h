///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Special types used by the library.
///
/// Exceptions...
///
///////////////////////////////////////////////////////////////////////////////



#ifndef XI_LDOPA_UTILS_H_
#define XI_LDOPA_UTILS_H_

#pragma once


#include "xi/ldopa/ldopa_dll.h"

// std
//#include <exception>
#include  <stdexcept>
#include <string>


// https://stackoverflow.com/questions/295120/c-mark-as-deprecated
#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif



namespace xi { namespace ldopa {


// экспорт конкретизированных шаблонов в DLL
// это должно быть ДО использования в нижележащих классах
// PIMPL (https://support.microsoft.com/en-us/kb/168958)
#ifdef LDOPA_DLL

//LDOPA_EXPIMP_TEMPLATE 
class LDOPA_API std::exception;



#endif // LDOPA_DLL


#pragma region General Types

typedef unsigned int UInt;          ///< Alias for unsigned int.

#pragma endregion 




/** \brief An ldopa library specific exception.
 *
 *  Upd. (06.12.2018): std::exception naturally doesn't have a constructor with
 *  a char* param (it's a MS extension), so we need to reformat this class to
 *  be based on the more specific type.
 *  https://stackoverflow.com/questions/28640553/exception-class-with-a-char-constructor
 */
//class DPMODEL_EXPIMP DPMException : public std::exception {
class LDOPA_API LdopaException 
    : public std::logic_error {
    //: public std::exception {
public:
    /** \brief Determine an scalar type for error codes. 
     *
     *  Code 0 is for common exceptions.
     *  Codes 1 — FFFF are reserved for the futher using
     *  Codes 1000 — MAX_INT can be used by parts of library; thus, it is a need
     *      code sets doesn't have interseption when more than one part is used simultaneously.
     */
    typedef unsigned int ErrorCode;
public:
    static const int MAX_FORMAT_STR_SIZE = 1024;
    static const ErrorCode ERC_DEF = 0;             // default
    static const ErrorCode ERC_USER = 0x10000;      // first user code

public:
    //------------<Public Static (Builder) Methods>----------------------

    // #thinkof: подумать над move-семантикой для этих методов (учесть реализацию базового)

    /** \brief Throws an exception with sprintf formatted string. */
    static LdopaException f(const char* _Format, ...);

    /** \brief Throws an exception with arguments appropriated for sprintf formatted string.  */
    static LdopaException f(const char* _Format, va_list argptr);

    static LdopaException f(ErrorCode erCode, const char* _Format, ...);
    static LdopaException f(ErrorCode erCode, const char* _Format, va_list argptr);



public:
    //------------<Constructors>---------------- 
    
    /// Default constructor.
    explicit LdopaException() :
        std::logic_error(""),
        _erCode(ERC_DEF)
            //,    _hasErrMes(false)
    {
    }
        
    /// With Info message 
    explicit  LdopaException(const char* const & wh, ErrorCode erCode = ERC_DEF) :
        std::logic_error(wh),
        //_msg(wh),
        _erCode(erCode)
            //,  _hasErrMes(false)
    {
    }

protected:

    /** \brief Error code if assigned. */
    ErrorCode _erCode;

}; // class LdopaException





}} // namespace xi { namespace ldopa

#endif // XI_LDOPA_UTILS_H_
