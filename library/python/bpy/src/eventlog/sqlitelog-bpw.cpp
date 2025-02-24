////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for SQLite-based event log Implementation.
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      27.02.2018
/// \copyright (c) xidv.ru 2014–2018.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

// ldopa.log
#include "xi/ldopa/eventlog/sqlite/sqlitelog.h"

using namespace boost::python;
using xi::ldopa::eventlog::IEventLog;
using xi::ldopa::eventlog::SQLiteLog;
using xi::ldopa::eventlog::IAttributesEnumerator;

// конвертер Attribute в питоновский тип
struct Attribute2PyType
{
    typedef IEventLog::Attribute Attribute;
    typedef IEventLog::Attribute::AType AType;

    static PyObject* convert(const IEventLog::Attribute& atr)
    {
        // иначе под каждый конкретный тип что-нибудь хорошенькое
        AType atype = atr.getType();
        bool converted = true;
        object pyObj;

        switch (atype)
        {
        case Attribute::tBlank:                                // пустой атрибут (Null) — туплет с нулем д.б.
        {
            break;
        }
        case Attribute::tChar:                                 // char
        {
            // специального типа для char нет, поэтому превращаем в строку
            pyObj = object(atr.toString());
            break;
        }
        case Attribute::tStringSharedPtr:
        {
            pyObj = object(atr.asStr());
            break;
        }
        case Attribute::tUChar:
        case Attribute::tInt:
        {
            pyObj = object(atr.asInt());
            break;
        }
        case Attribute::tUInt:
        case Attribute::tInt64:
        case Attribute::tUInt64:
        {
            pyObj = object(atr.asInt64());
            break;
        }
        case Attribute::tDouble:
        {
            pyObj = object(atr.asDouble());
            break;
        }

        case Attribute::tDestrObjSharedPtr:
        {
            // https://stackoverflow.com/questions/23064407/expose-c-buffer-as-python-3-bytes 
            //pyObj = object("BLOB data");
            xi::attributes::IDestructableObject* dob = atr.asDestrObj();    // храним через полиморфный указатель
            xi::attributes::DestrByteArray* bArr = 
                dynamic_cast<xi::attributes::DestrByteArray*>(dob);         // но знаем, что там внутри
            if (!bArr)
            {
                converted = false;
                break;
            }
            pyObj = object(handle<>(PyMemoryView_FromMemory((char*)bArr->getArr(), bArr->getSize(), PyBUF_READ)));
            break;
        }
        default:
        {
            converted = false;
        }
        }

        if (converted)
            return incref(pyObj.ptr());
            //return incref(make_tuple(pyObj).ptr());

        // если конверсия провалилась, кинем исключение
        PyErr_SetString(PyExc_ResourceWarning, "Can't convert IEventLog::Attribute.");
        boost::python::throw_error_already_set();
        return nullptr;         // это, конечно, unreachable code, но чтобы компилятор не ругался
    }
};

// конвертер AttributeRes в питоновскую tuple
struct AttributeRes2PyTuple
{
    typedef IEventLog::Attribute Attribute;
    //typedef IEventLog::Attribute::AType AType;

    static PyObject* convert(const IEventLog::AttributeRes& na)
    {
        if (!na.second)
            return boost::python::incref(object().ptr());   // должен создать None

        // иначе под каждый конкретный тип что-нибудь хорошенькое
        object pyObj = object(na.first);
        return incref(make_tuple(pyObj).ptr());
    }
};


// конвертер NamedAttribute в питоновскую tuple
struct NamedAttribute2PyTuple
{
    typedef IEventLog::Attribute Attribute;
    typedef IEventLog::Attribute NamedAttribute;

    static PyObject* convert(const IEventLog::NamedAttribute& na)
    {
        object aName = object(na.first);        // str
        object attr = object(na.second);        // attribute

        return incref(make_tuple(aName, attr).ptr());
    }
};


/** \brief Python wrapper for IAttributesEnumerator, making a python iterator from it.
 *
 *  Based on ideas from:
 *  * https://wiki.python.org/moin/boost.python/iterator 
 *  * http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/tutorial/tutorial/iterators.html
 */
struct IAttributesEnumerator2PyIter
{
    typedef IEventLog::Attribute        Attribute;
    typedef IEventLog::NamedAttribute   NamedAttribute;

    // реализует протокол движения/эксцепции-по-окончании итератора python
    //static const Attribute next(IAttributesEnumerator* en)
    static NamedAttribute next(IAttributesEnumerator* en)
    {
        if (!en->hasNext())
        {
            PyErr_SetString(PyExc_StopIteration, "No more attributes.");
            boost::python::throw_error_already_set();
        }
        // иначе итерируема
        return en->getNext();          
    }

    // вспомогательная 
    static inline object PassThrough(const object& o) { return o; }

    // обертка
    static void wrap() //const char* pyName)
    {
        class_<IAttributesEnumerator, boost::noncopyable>("IAttributesEnumerator", no_init) // для абстрактного так!
            .def("next", next)
            .def("__iter__", PassThrough)
        ;
    }
};


//=============================================================================

/** \brief Wrappers for IEventLog-related interfaces. */
struct IEventLog_PyWrappers
{
    typedef xi::ldopa::eventlog::IEventLog IEventLog;
    typedef xi::ldopa::eventlog::IEventTrace IEventTrace;
    typedef xi::ldopa::eventlog::IEvent IEvent;

    //-------------------------------------------------------------------------

    /** \brief Wraps a give attribute \a attr in a python wrapper if the attributes is valid
     *  (not Null) which is determined by a parameter \a isValid.
     */
    static PyObject* wrapEventLogAttr(bool isValid, const IEventLog::Attribute* attr)
    {
        // если извлечь не получилось
        if (!isValid)
            return incref(object().ptr());   // должен создать None

        // если получилось, то под каждый конкретный тип что-нибудь хорошенькое
        object pyObj = object(*attr);
        return incref(make_tuple(pyObj).ptr());
    }

    //-------------------------------------------------------------------------

    /** \brief Returns a trace attribute as a cooked py-object. */
    static PyObject* getTraceAttr(IEventTrace* tr, const char* id)
    {
        SQLiteLog::Attribute a;
        bool res = tr->getAttr(id, a);

        return /*SQLiteLog_PyWrapper::*/wrapEventLogAttr(res, &a);
    }

    //-------------------------------------------------------------------------

    /** \brief Returns an event attribute as a cooked py-object. */
    static PyObject* getEventAttr(IEvent* ev, const char* id)
    {
        SQLiteLog::Attribute a;
        bool res = ev->getAttr(id, a);

        return /*SQLiteLog_PyWrapper::*/wrapEventLogAttr(res, &a);
    }

    //-------------------------------------------------------------------------

    static void exportClasses()
    {
        // interface IEventTrace
        class_<IEventTrace, /*boost::shared_ptr<IEventTrace>,*/ boost::noncopyable>("IEventTrace", no_init)
            //.def("get_attrs_num", &IEventTrace::getAttrsNum)            
            .def("get_attr", getTraceAttr)
            .def("get_attrs", &IEventTrace::getAttrs, return_value_policy<manage_new_object>())// питоновский GC грохнет
            .def("get_event", &IEventTrace::getEvent, return_internal_reference<>())
            .add_property("attrs_num", &IEventTrace::getAttrsNum)      // вообще, это может привести к запросу всех атрибутов...
            .add_property("size", &IEventTrace::getSize)          // это тоже может привести к запросу числа ивентов и т.д.

            .def("get_log", &IEventTrace::getLog, return_internal_reference<>())
            ;

        // interface IEvent
        class_<IEvent, boost::noncopyable>("IEvent", no_init)
            .def("get_attr", getEventAttr)
            .def("get_attrs", &IEvent::getAttrs, return_value_policy<manage_new_object>())
            .add_property("attrs_num", &IEvent::getAttrsNum)

            .def("get_trace", &IEvent::getTrace, return_internal_reference<>())
            ;

        // interface IEventLog
        class_<IEventLog, boost::noncopyable>("IEventLog", no_init)
            //----[Open/close methods]----
            .def("open", &IEventLog::open)
            .def("close", &IEventLog::close)

            //----[EventLog matter]----
            .def("get_events_num", &IEventLog::getEventsNum)
            .def("get_traces_num", &IEventLog::getTracesNum)
            .def("get_activities_num", &IEventLog::getActivitiesNum)

            // log attributes
            .def("get_log_attrs", &IEventLog::getLogAttrs, return_value_policy<manage_new_object>())

            // traces and attributes
            .def("get_trace_attrs", &IEventLog::getTraceAttrs, return_value_policy<manage_new_object>())// питоновский GC грохнет
            .def("get_trace_size", &IEventLog::getTraceSize)
            .def("get_trace", &IEventLog::getTrace, return_internal_reference<>())

            //----[Properties]----
            .add_property("is_open", &IEventLog::isOpen)
            ; // class_<IEventLog>

    }
}; // struct IEventLog_PyWrappers


/** \brief Wrappers for some types and methods of SQLiteLog class. */
struct SQLiteLog_PyWrapper
{
    typedef xi::ldopa::eventlog::IEventTrace IEventTrace;
    //-------------------------------------------------------------------------

    /** \brief Returns a log attribute as a cooked py-object. */
    static PyObject* getLogAttr(SQLiteLog& log, const char* id)
    {
        SQLiteLog::Attribute a;
        bool res = log.getLogAttr(id, a);

        return IEventLog_PyWrappers::wrapEventLogAttr(res, &a);
    }

    //-------------------------------------------------------------------------

    /** \brief Returns a trace attribute as a cooked py-object. */
    static PyObject* getTraceAttr(SQLiteLog& log, int traceNum, const char* id)
    {
        SQLiteLog::Attribute a;
        bool res = log.getTraceAttr(traceNum, id, a);

        return IEventLog_PyWrappers::wrapEventLogAttr(res, &a);
    }

    //-------------------------------------------------------------------------

    // альтернативный подход к получению трассы
    static IEventTrace& getTraceAlt(SQLiteLog& log, int traceNum)
    {
        IEventTrace* tr = log.getTrace(traceNum);
        return *tr;
    }

    //-------------------------------------------------------------------------

    static void exportClass()
    {
        // здесь, в общем-то, необязательно создавать объект sqlogClass, но его можно использовать
        // https://stackoverflow.com/questions/10142417/boostpython-compilation-fails-because-copy-constructor-is-private
        object sqlogClass = class_<SQLiteLog, bases<IEventLog>, boost::noncopyable>("SQLiteLog", 
            init<const std::string&>())

            //----[Open/close methods]----
            .def("open",                        &SQLiteLog::open)
            .def("close",                       &SQLiteLog::close)

            //----[EventLog matter]----
            .def("get_events_num",              &SQLiteLog::getEventsNum)
            .def("get_traces_num",              &SQLiteLog::getTracesNum)
            .def("get_activities_num",          &SQLiteLog::getActivitiesNum)

            //----[SQLite EventLog specific]----
            .def("get_config_param_or_empty",   &SQLiteLog::getConfigParamOrEmpty)
            .def("set_config_param",            &SQLiteLog::setConfigParam)

            // log attributes
            .def("get_log_attr",                getLogAttr)
            .def("get_log_attrs",               &SQLiteLog::getLogAttrs, 
                    return_value_policy<manage_new_object>())    // питоновский GC грохнет

            // traces and atra attributes
            .def("get_trace_attr",              getTraceAttr)
            .def("get_trace_attrs",             &SQLiteLog::getTraceAttrs, 
                    return_value_policy<manage_new_object>())// питоновский GC грохнет
            .def("get_trace_size",              &SQLiteLog::getTraceSize)
            // http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/reference/function_invocation_and_creation/models_of_callpolicies.html
            // http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/tutorial/tutorial/functions.html
            // https://wiki.python.org/moin/boost.python/CallPolicy
            .def("get_trace",                   &SQLiteLog::getTrace, return_internal_reference<>())

            //----[Properties]----
            .add_property("filename",           &SQLiteLog::getFileName, &SQLiteLog::setFileName)
            .add_property("is_open",            &SQLiteLog::isOpen)

            //----[Properies for Queries et al]----
            .add_property("autoload_conf_qry",  &SQLiteLog::getAutoLoadConfigQry, 
                                                &SQLiteLog::setAutoLoadConfigQry)
            .add_property("autoload_conf",      &SQLiteLog::isAutoLoadConfig, 
                                                &SQLiteLog::setAutoLoadConfig)
            .add_property("vert_event_attr_extr", &SQLiteLog::isVerticalEventAttrExtracting, 
                                                  &SQLiteLog::setVerticalEventAttrExtracting)
            .add_property("vert_trace_attr_extr", &SQLiteLog::isVerticalTraceAttrExtracting, 
                                                  &SQLiteLog::setVerticalTraceAttrExtracting)
        ;
    }
}; // struct SQLiteLog_PyWrapper

//=============================================================================

// export function for SQLiteLig classes
void export_SQLiteLog()
{
    //--=регистрируем конвертеры=-    
    to_python_converter <SQLiteLog::Attribute, Attribute2PyType >();
    to_python_converter <SQLiteLog::AttributeRes, AttributeRes2PyTuple >();
    to_python_converter <SQLiteLog::NamedAttribute, NamedAttribute2PyTuple >();

    //--=регистрируем итераторы=-
    IAttributesEnumerator2PyIter::wrap();

    //--=регистрируем обертки для классов=-
    IEventLog_PyWrappers::exportClasses();
    SQLiteLog_PyWrapper::exportClass();
}

