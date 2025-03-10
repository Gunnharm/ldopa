////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Boost lib based attributes system.
/// \author Sergey Shershakov
/// \version 0.1.0
/// \date 04.02.2015
///
/// Types for attributing components with use boost library for maintaining 
/// variant types.
/// Main Boost exported type for maintaining variant is boost::variant
///
/// #appnote: решено пока для шаблонов XILIB_API не выводить, т.к. они
/// должны инстанцироваться там, где используются:
/// (https://support.microsoft.com/en-us/kb/168958)
///
////////////////////////////////////////////////////////////////////////////////

#pragma     

#ifndef XILIB_XI_ATTRIBUTES_BOOST_ATTR_H_          // boostAttrH
#define XILIB_XI_ATTRIBUTES_BOOST_ATTR_H_


#include "xi/xilib_dll.h"

//STD
#include <bitset>
#include <list>
#include <map>
#include <memory>   // shared_ptr

// boost library
#include <boost/variant.hpp>            // boost::variant
#include <boost/shared_array.hpp>       // boost::shared_array
//#include <boost/variant.hpp>

#include "xi/xilib_dll.h"               // все-таки в каком пакете он должен лежать?
#include "xi/attributes/destructable_object.h"
#include "xi/types/aliases.h"


namespace xi {

/** \brief Namespace contains types for boost-based attributing. */
namespace attributes {




/** \brief Correct type definition for a shared pointer to a c-string. */
typedef boost::shared_array<char> CStrSharedArr;  // тестируем

/** \brief Type definition for a shared pointer to a IDestructableObject object */
typedef std::shared_ptr<IDestructableObject> DestrObjSharedPtr;

/** \brief Type definition for a shared pointer to std::string object */
typedef std::shared_ptr<std::string> StringSharedPtr;  // тестируем



/** \brief Visitor for converting Variant object to a string representations. */
//class XILIB_API VarAttributeToStrDefVisitor : public boost::static_visitor<std::string>
class VarAttributeToStrDefVisitor : public boost::static_visitor<std::string>
{
public:

    // не иниц. тип
    std::string operator()(boost::blank) const
    {
        return "<Empty>";
    }


    // здесь считаем параметр символом
    std::string operator()(char c) const
    {
        std::string s;
        s = c;
        return s;
    }

    // здесь считаем параметр числом
    std::string operator()(unsigned char c) const
    {
        std::string s = std::to_string(c);
        return s;
    }

    std::string operator()(int i) const
    {
        // https://stackoverflow.com/questions/5590381/easiest-way-to-convert-int-to-string-in-c
        std::string s = std::to_string(i);
        return s;
    }

    std::string operator()(unsigned int i) const
    {
        std::string s = std::to_string(i);
        return s;
    }

    std::string operator()(xi::types::TInt64 i) const
    {
        // https://stackoverflow.com/questions/5590381/easiest-way-to-convert-int-to-string-in-c
        std::string s = std::to_string(i);
        return s;
    }

    std::string operator()(xi::types::TQword i) const
    {
        std::string s = std::to_string(i);
        return s;
    }

    std::string operator()(double d) const
    {
        std::string s = std::to_string(d);
        return s;
    }


    std::string operator()(void* p) const
    {
        // TODO: здесь только адрес в 16-ричной форме
        std::string s = std::to_string((int)p);
        return s;
    }

    std::string operator()(const void* p) const
    {
        // TODO: здесь только адрес в 16-ричной форме
        std::string s = std::to_string((int)p);
        return s;
    }

    // интепретируем как строку
    std::string operator()(char* cs) const
    {
        std::string s = cs;
        return s;
    }

    // интепретируем как строку
    std::string operator()(const char* cs) const
    {
        std::string s = cs;
        return s;
    }

    // интерпретируем как адрес указателя
    std::string operator()(const DestrObjSharedPtr& dob) const
    {
        // TODO: в 16-ричной форме
        std::string s = std::to_string((int)dob.get());
        return s;
    }

    std::string operator()(const CStrSharedArr& sp) const
    {
        std::string s(sp.get());        // берем ук. на цепочку чаров до первой пусты
        return s;
    }

    std::string operator()(const StringSharedPtr& sp) const
    {
        std::string s(*sp);        // берем ук. на цепочку чаров до первой пусты
        return s;
    }



    // "вне конкурса"
    std::string operator()(const std::string& s) const
    {
        return s;
    }

}; // class VarAttribute1ToStrVisitor



/** \brief Main class for out VarAttribute containing some helper methods
 *  to access to an object given in form of...
 *
 *  Template parameter is for setiing toString() visitor.
 */
template<typename TVis = VarAttributeToStrDefVisitor>
class VarAttributeTVis1 { // : public BaseVarAttribute1 {
    // TODO:
public:
    //typedef BaseVarAttribute1 Base;

    /** \brief Variant type #1: 16 bytes for representation. */
    typedef boost::variant <
        boost::blank,                               // empty, uninitialized type

        char,
        unsigned char,
        int,
        unsigned int,

        xi::types::TInt64,                          // 4-й теперь
        xi::types::TQword,

        double,                                     // 6-й (был 4-й)

        // pointers
        void*,                                      // memory-unmanaged object pointer
        const void*,
        char*,                                      // for c-strings
        const char*,

        // shared_ptr<>                         // TODO: вынести в DestrObjSharedPtr операции сравнения или отдельным типом сравнимым сделать!
        DestrObjSharedPtr,                          // memory-managed object pointer    
        CStrSharedArr,                              // c-string // DONE: нельзя, вместо него  надо boost::shared_array
        StringSharedPtr                             // std::string
    > BaseVariant;


    enum AType {
        tBlank,                        // empty, uninitialized type
        tChar,
        tUChar,
        tInt,
        tUInt,
        tInt64,
        tUInt64,
        tDouble,
        //tBitset32,
        //tBitset64,
        tVoidP,
        tVoidCP,
        tCharP,
        tCharCP,
        tDestrObjSharedPtr,
        tCStrSharedArr,
        tStringSharedPtr,
        // atUnknown,   there are no such a state
    }; //
public:
    //---------<Constructors>-----------

    /** \brief Default constructor */
    VarAttributeTVis1() {} //               : BaseVarAttribute(val) {  } ;

    VarAttributeTVis1(char val) : _var(val) {  };
    VarAttributeTVis1(unsigned char val) : _var(val) {  };
                                                
    /** \brief Initializing with Integer */     
    VarAttributeTVis1(int val) : _var(val) {  };
    VarAttributeTVis1(unsigned int val) : _var(val) {  };
    VarAttributeTVis1(xi::types::TInt64 val) : _var(val) {  };
    VarAttributeTVis1(xi::types::TQword val) : _var(val) {  };


    VarAttributeTVis1(double val) : _var(val) {  };
    
    VarAttributeTVis1(void*  val) : _var(val) {  };  // тут const не пролезет
    VarAttributeTVis1(const void*  val) : _var(val) {  };
    VarAttributeTVis1(char*  val) : _var(val) {  };  // и тут
    VarAttributeTVis1(const char*  val) : _var(val) {  };  // и тут
    VarAttributeTVis1(const DestrObjSharedPtr& val) : _var(val) {  };
    VarAttributeTVis1(const CStrSharedArr& val) : _var(val) {  };
    VarAttributeTVis1(const StringSharedPtr& val) : _var(val) {  };


    // special for std::string
    VarAttributeTVis1(const std::string& s) 
        : _var(StringSharedPtr(new std::string(s))) {  };

    VarAttributeTVis1(std::string* s)
        : _var(StringSharedPtr(s)) {  };


    /** \brief copy constructor */
    VarAttributeTVis1(const VarAttributeTVis1& rhv) : _var(rhv.getRef()) {  }
public:
    //----<Static Factory Constructors>----
    
    // creates an std string from a const char*
    static VarAttributeTVis1 ss(const char* s)
    {
        return StringSharedPtr(new std::string(s));
    }
public:
    //--------<Operators>------------

    /** \brief copy (assigning) operator  */
    VarAttributeTVis1 operator=(const VarAttributeTVis1& rhv) 
    { 
        getRef() = rhv.getRef(); 
        return *this; 
    }


    inline bool operator==(const VarAttributeTVis1& rhv) const 
    { 

        // ура! никто, почему-то и не подумал, что у объектов для сравнения
        // вполне себе могут быть разные типы
        AType lt = getType();
        AType rt = rhv.getType();

        // случай 1, наивный: типы одинаковые
        if (lt == rt)
        {
            // для некоторых типов специальная проверка
            AType lt = getType();
            if (lt == tStringSharedPtr)                                  // std::string
                return (*boost::get<StringSharedPtr>(getRef()) ==
                *boost::get<StringSharedPtr>(rhv.getRef()));

            if (lt == tCStrSharedArr)                                    // c-строка
                return strcmp(boost::get<CStrSharedArr>(getRef()).get(),
                boost::get<CStrSharedArr>(rhv.getRef()).get()) == 0;
        }

        // случай 2, жизненный (а если попробовать на откуп ей отдать?!)
        // для всего остального...
        return (getRef() == rhv.getRef()); 
    }

    inline bool operator!=(const VarAttributeTVis1& rhv) const 
    { 
        return !operator==(rhv);
        //return !(getRef() == rhv.getRef()); 
    }

    /** \brief Return true if \a *this is equal to \a rhv */
    bool operator < (const VarAttributeTVis1& rhv) const
    {
        // ура! никто, почему-то и не подумал, что у объектов для сравнения
        // вполне себе могут быть разные типы
        AType lt = getType();
        AType rt = rhv.getType();

        // случай 1, наивный: типы одинаковые
        if (lt == rt)
        {
            // для некоторых типов специальная проверка

            if (lt == tStringSharedPtr)                                  // std::string
                return (*boost::get<StringSharedPtr>(getRef()) <
                *boost::get<StringSharedPtr>(rhv.getRef()));

            if (lt == tCStrSharedArr)                                    // c-строка
                return strcmp(boost::get<CStrSharedArr>(getRef()).get(),
                boost::get<CStrSharedArr>(rhv.getRef()).get()) < 0;

            //// для всего остального
            //return (getRef() < rhv.getRef());
        }

        // случай 2, жизненный (а если попробовать на откуп ей отдать?!)
        return (getRef() < rhv.getRef());
    }

    /** \brief Syntax sugar for operator < */
    bool operator > (const VarAttributeTVis1& that) const { return that < *this; };

public:
    //-----------<VarAttribute some info>----------

    /** \brief Returns a type of attribute data */
    AType getType() const { return (AType)(getRef().which()); } // {return atUnknown; } ;

    /** \brief Returns true if the pbject is "empty" (uninitialized). */
    bool isEmpty() const { return getType() == tBlank; }

    /** \brief Makes an object empty. */
    void clear() { getRef() = boost::blank(); }

public:
    //-----------<Gets as a type>----------
    
    
    /** \brief Return int representation. */
    int asInt() const
    {
        // TODO: может здесь тоже visitor, либо visitor только в конверсионных toXXX методах используем?
        AType t = getType();
        if (t == tInt)
            return boost::get<int>(getRef());

        if (t == tUInt)
            return (int)boost::get<unsigned int>(getRef());

        if (t == tInt64)
            return (int)boost::get<xi::types::TInt64>(getRef());

        if (t == tUInt64)
            return (int)boost::get<xi::types::TQword>(getRef());

        if (t == tUChar)
            return (int)boost::get<unsigned char>(getRef());

        if (t == tChar)
            return (int)boost::get<char>(getRef());

        return 0;
    }

    /** \brief Return int64 representation. */
    xi::types::TInt64 asInt64() const
    {
        // TODO: может здесь тоже visitor, либо visitor только в конверсионных toXXX методах используем?
        AType t = getType();

        if (t == tInt64)
            return boost::get<xi::types::TInt64>(getRef());

        if (t == tUInt64)
            return (xi::types::TInt64)boost::get<xi::types::TQword>(getRef());

        if (t == tInt)
            return boost::get<int>(getRef());

        if (t == tUInt)
            return boost::get<unsigned int>(getRef());


        if (t == tUChar)
            return boost::get<unsigned char>(getRef());

        if (t == tChar)
            return boost::get<char>(getRef());

        return 0;
    }

    /** \brief Return double representation. */
    double asDouble() const
    {
        AType t = getType();
        if (t == tDouble)
            return boost::get<double>(getRef());

        // иначе попробуем из целого привести
        return (double)asInt64();
    }

    /** \brief Return as a string pointer only for appropriate types. For convertion, see toString(). */
    //std::string& asStr() const
    std::string* asStrP() const
    {
        AType t = getType();
        if (t == tStringSharedPtr)
        { 
            const StringSharedPtr strPtr = boost::get<StringSharedPtr>(getRef());
            return strPtr.get();
        }

        return nullptr;
        //throw std::runtime_error("Not a string.");      // здесь не оч. удачно, что семантика страдает
    }

    std::string& asStr() const
    {
        std::string* sp = asStrP();
        if (!sp)
            throw std::runtime_error("Not a string.");
        
        return *sp;
    }


    /** \brief Returns as an destructable object. */
    IDestructableObject* asDestrObj() const
    {
        AType t = getType();
        if (t == tDestrObjSharedPtr)
        {
            const DestrObjSharedPtr oPtr = boost::get<DestrObjSharedPtr>(getRef());
            return oPtr.get();
        }

        return nullptr;
    }

    
    //-----------<Convertion methods>----------
    std::string toString() const 
    { 
        // визитор для конвертации в строку приходит параметром!
        std::string s = boost::apply_visitor(TVis(), getRef());
        return s;
    }    

public:
    //----------<sets/gets>-------------

    /** \brief Temporary helper method giving access to the object throught
     *  a reference to a base class
     */
    BaseVariant& getRef() { return _var; }
    const BaseVariant& getRef() const { return _var; }

    BaseVariant* getPtr() { return &_var; }
    const BaseVariant* getPtr() const { return &_var; }
protected:
    // instance of a base variant
    BaseVariant _var;

}; // class VarAttributeTVis1


typedef VarAttributeTVis1<VarAttributeToStrDefVisitor> VarAttribute1;



}; //  namespace attributes
}; // namespace xi



#endif // XILIB_XI_ATTRIBUTES_BOOST_ATTR_H_
