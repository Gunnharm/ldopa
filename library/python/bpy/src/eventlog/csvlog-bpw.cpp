////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     boost::python wrappers for CSV-based event log Implementation.
/// \author    ganvas
/// \version   0.1.0
/// \date      2025-04-21
///
////////////////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

// Подключаем основное описание CSVLog, CSVTrace, CSVEvent
#include "xi/ldopa/eventlog/csvlog/csvlog.h"

// Подключаем интерфейсы, так как они нужны для приведения типов — 
// i.e. IEventLog, IEventTrace, IEvent, т. п.
#include "xi/ldopa/eventlog/eventlog.h"

using namespace boost::python;

// Чтобы не тащить длинные имена пространств:
namespace xiled = xi::ldopa::eventlog;
namespace csv   = xi::ldopa::eventlog::csvlog;

/* ------------------------------------------------------------------
   1) Сперва вспомогательный класс-конвертер для поля IEventLog::Attribute.

   Точно так же, как в примере для SQLiteLog, мы можем написать
   Attribute2PyType, чтобы автоматически приводить IEventLog::Attribute
   в объекты Python. Ниже — упрощённый вариант без сложного каста
   (оставьте у себя логику обработки tStringSharedPtr / tInt ит.д.,
   если нужно).
   ------------------------------------------------------------------ */
struct Attribute2PyType
{
    typedef xiled::IEventLog::Attribute Attribute;
    typedef xiled::IEventLog::Attribute::AType AType;

    static PyObject* convert(const Attribute& atr)
    {
        // Для примера сделаем простую логику: 
        // если это строка, вернуть Python-строку, если число — Python int, иначе None

        AType atype = atr.getType();
        object pyObj;  // объект, который завернём в PyObject*

        switch(atype)
        {
        case Attribute::tStringSharedPtr:
            pyObj = object(atr.asStr());
            break;

        case Attribute::tInt:
        case Attribute::tUInt:
        case Attribute::tUChar:
            pyObj = object(atr.asInt());
            break;

        case Attribute::tDouble:
            pyObj = object(atr.asDouble());
            break;

        default:
            // Если не можем привести, вернём None
            pyObj = object();  
            break;
        }

        return incref(pyObj.ptr()); 
    }
};


/* ------------------------------------------------------------------
   2) Точно так же, как в примере SQLiteLog, нам нужен итератор 
      Python для перебора атрибутов (IAttributesEnumerator).
      Ниже — сокращённый вариант, который просто выдаёт NamedAttribute.
   ------------------------------------------------------------------ */
struct IAttributesEnumerator2PyIter
{
    // В реальности NamedAttribute = std::pair<std::string, IEventLog::Attribute>
    typedef xiled::IEventLog::NamedAttribute NamedAttribute;

    static NamedAttribute next(xiled::IAttributesEnumerator* en)
    {
        if(!en->hasNext())
        {
            PyErr_SetString(PyExc_StopIteration, "No more attributes.");
            throw_error_already_set();
        }
        return en->getNext();
    }

    static object passThrough(const object& o) { return o; }

    static void wrap(const char* pyName = "IAttributesEnumerator")
    {
        class_<xiled::IAttributesEnumerator, boost::noncopyable>(pyName, no_init)
            .def("next", &next)
            .def("__iter__", &passThrough)
        ;
    }
};


/* ------------------------------------------------------------------
   3) Базовые обёртки для IEventLog, IEventTrace, IEvent 
      (см. аналогию с примером для SQLiteLog).
      Чтобы из Python вызывать методы getAttr(...) и т.п., 
      удобнее дополнительно вставить вспомогательные функции, 
      возвращающие Python-объекты.
   ------------------------------------------------------------------ */
struct IEventLog_PyWrappers
{
    typedef xiled::IEventLog IEventLog;
    typedef xiled::IEventTrace IEventTrace;
    typedef xiled::IEvent IEvent;

    // Функция-утилита — заворачивает IEventLog::Attribute (если он есть) в PyObject*
    static PyObject* wrapEventLogAttr(bool isValid, const IEventLog::Attribute* attr)
    {
        if(!isValid)
        {
            // Не найден атрибут => возвращаем None
            return incref(object().ptr());
        }

        // Если атрибут найден, превращаем его в PyObject через наш конвертер:
        object pyObj(*attr);
        return incref(pyObj.ptr());
    }

    // обёртка для IEventTrace::getAttr
    static PyObject* getTraceAttr(IEventTrace* tr, const char* id)
    {
        if(!tr) 
        {
            PyErr_SetString(PyExc_ValueError, "Trace pointer is null!");
            throw_error_already_set();
        }
        IEventLog::Attribute a;
        bool res = tr->getAttr(id, a);
        return wrapEventLogAttr(res, &a);
    }

    // обёртка для IEvent::getAttr
    static PyObject* getEventAttr(IEvent* ev, const char* id)
    {
        if(!ev)
        {
            PyErr_SetString(PyExc_ValueError, "Event pointer is null!");
            throw_error_already_set();
        }
        IEventLog::Attribute a;
        bool res = ev->getAttr(id, a);
        return wrapEventLogAttr(res, &a);
    }

    static void exportClasses()
    {
        // 3.1) IEventTrace
        class_<IEventTrace, boost::noncopyable>("IEventTrace", no_init)
            .def("get_attr",       &getTraceAttr)
            .def("get_attrs",      &IEventTrace::getAttrs, return_value_policy<manage_new_object>())
            .def("get_event",      &IEventTrace::getEvent, return_internal_reference<>())
            .add_property("attrs_num", &IEventTrace::getAttrsNum)
            .add_property("size",      &IEventTrace::getSize)
            .def("get_log",        &IEventTrace::getLog,   return_internal_reference<>())
        ;

        // 3.2) IEvent
        class_<IEvent, boost::noncopyable>("IEvent", no_init)
            .def("get_attr",  &getEventAttr)
            .def("get_attrs", &IEvent::getAttrs, return_value_policy<manage_new_object>())
            .add_property("attrs_num", &IEvent::getAttrsNum)
            .def("get_trace", &IEvent::getTrace, return_internal_reference<>())
        ;

        // 3.3) IEventLog
        class_<IEventLog, boost::noncopyable>("IEventLog", no_init)
            .def("open",              &IEventLog::open)
            .def("close",             &IEventLog::close)
            .add_property("is_open",  &IEventLog::isOpen)

            .def("get_events_num",    &IEventLog::getEventsNum)
            .def("get_traces_num",    &IEventLog::getTracesNum)
            .def("get_activities_num",&IEventLog::getActivitiesNum)

            .def("get_log_attrs",     &IEventLog::getLogAttrs, return_value_policy<manage_new_object>())

            .def("get_trace_size",    &IEventLog::getTraceSize)
            .def("get_trace_attrs",   &IEventLog::getTraceAttrs, return_value_policy<manage_new_object>())
            .def("get_trace",         &IEventLog::getTrace, return_internal_reference<>())
        ;
    }
};


/* ------------------------------------------------------------------
   4) Собственно обёртка для CSVLog (по аналогии с SQLiteLog_PyWrapper).
   ------------------------------------------------------------------ */
struct CSVLog_PyWrapper
{
    // Удобная утилита для getLogAttr
    static PyObject* getLogAttr(csv::CSVLog& log, const char* id)
    {
        xiled::IEventLog::Attribute a;
        bool res = log.getLogAttr(id, a);
        return IEventLog_PyWrappers::wrapEventLogAttr(res, &a);
    }

    // Утилита для getTraceAttr (через CSVLog; он унаследован от IEventLog)
    static PyObject* getTraceAttr(csv::CSVLog& log, int traceNum, const char* id)
    {
        xiled::IEventLog::Attribute a;
        bool res = log.getTraceAttr(traceNum, id, a);
        return IEventLog_PyWrappers::wrapEventLogAttr(res, &a);
    }

    // Пример, если захотите вместо .def("get_trace", &CSVLog::getTrace, ...):
    static xiled::IEventTrace& getTraceAlt(csv::CSVLog& log, int traceNum)
    {
        xiled::IEventTrace* tr = log.getTrace(traceNum);
        if(!tr)
        {
            PyErr_SetString(PyExc_IndexError, "Trace index is out of range.");
            throw_error_already_set();
        }
        return *tr;  // Возвращаем ссылку, чтобы работал return_internal_reference
    }

    static void exportClass()
    {
        // Оборачиваем CSVLog как класс, наследуемый (bases<>) от IEventLog
        class_<csv::CSVLog, bases<xiled::IEventLog>, boost::noncopyable>("CSVLog", init<>())
            // методы из IEventLog уже унаследованы, но можно повторить
            .def("open",              &csv::CSVLog::open)
            .def("close",             &csv::CSVLog::close)
            .add_property("is_open",  &csv::CSVLog::isOpen)

            .def("get_events_num",    &csv::CSVLog::getEventsNum)
            .def("get_traces_num",    &csv::CSVLog::getTracesNum)
            .def("get_activities_num",&csv::CSVLog::getActivitiesNum)

            // лог-атрибуты
            .def("get_log_attr",      &getLogAttr)
            .def("get_log_attrs",     &csv::CSVLog::getLogAttrs, return_value_policy<manage_new_object>())

            // трейс-атрибуты и работа с трейсами
            .def("get_trace_attr",    &getTraceAttr)
            .def("get_trace_attrs",   &csv::CSVLog::getTraceAttrs, return_value_policy<manage_new_object>())
            .def("get_trace_size",    &csv::CSVLog::getTraceSize)

            // По умолчанию делаем return_internal_reference, т.к. внутри CSVLog следит за временем жизни.
            .def("get_trace",         &csv::CSVLog::getTrace, return_internal_reference<>())

            // Собственные особенности CSVLog
            .add_property("filename",
                          make_function(&csv::CSVLog::getFileName,
                                        return_value_policy<copy_const_reference>()),
                          &csv::CSVLog::setFileName)

            // Можно добавить и infoStr, если нужно:
            .def("get_info_str",      &csv::CSVLog::getInfoStr)
        ;
    }
};


/* ------------------------------------------------------------------
   5) Наконец, функция export, которую вы зовёте из своего основного
      модуля (PyInit_...) либо как-то ещё, чтобы зарегистрировать
      обёртки в Boost.Python.
   ------------------------------------------------------------------ */
void export_CSVLog()
{
    // 5.1) Говорим Boost.Python, как конвертировать IEventLog::Attribute в Python
    to_python_converter<xiled::IEventLog::Attribute, Attribute2PyType>();

    // Если нужно, также конвертируем NamedAttribute и т.д.:
    // to_python_converter<xiled::IEventLog::NamedAttribute, NamedAttribute2PyTuple>();

    // 5.2) Регистрируем класс-итератор для IAttributesEnumerator
    IAttributesEnumerator2PyIter::wrap("IAttributesEnumerator");

    // 5.3) Экспортируем базовые интерфейсы (IEventLog, IEventTrace, IEvent)
    IEventLog_PyWrappers::exportClasses();

    // 5.4) Экспортируем сам CSVLog
    CSVLog_PyWrapper::exportClass();
}