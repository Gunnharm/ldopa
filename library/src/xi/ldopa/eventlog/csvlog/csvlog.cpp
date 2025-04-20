#include "csvlog.h"

// std
#include <fstream>
#include <sstream>

// xilib
#include "xi/collections/enumerators.hpp"

// ldopa
#include "xi/ldopa/eventlog/eventlog.h"

namespace xi { namespace ldopa { namespace eventlog { namespace csvlog {

// CSVEvent implementation
CSVEvent::CSVEvent(const std::string& activity)
    : _activity(activity)
    , _trace(nullptr)
{
}

CSVEvent::~CSVEvent()
{
}

bool CSVEvent::getAttr(const char* id, Attribute& a)
{
    auto it = _attributes.find(id);
    if (it != _attributes.end()) {
        a = it->second;
        return true;
    }
    return false;
}

int CSVEvent::getAttrsNum()
{
    return static_cast<int>(_attributes.size());
}

IAttributesEnumerator* CSVEvent::getAttrs()
{
    return new xi::collections::Enumerator4ConstIterator<decltype(_attributes)::const_iterator, IEventLog::NamedAttribute>(_attributes.cbegin(), _attributes.cend());
}

IEventTrace* CSVEvent::getTrace()
{
    return _trace;
}

void CSVEvent::setTrace(IEventTrace* trace)
{
    _trace = trace;
}

void CSVEvent::setAttribute(const std::string& id, const Attribute& attr) {
    _attributes[id] = attr;
}

// CSVTrace implementation
CSVTrace::CSVTrace()
    : _log(nullptr)
{
}

CSVTrace::~CSVTrace()
{
    for (auto event : _events) {
        delete event;
    }
}

int CSVTrace::getAttrsNum()
{
    return static_cast<int>(_attributes.size());
}

bool CSVTrace::getAttr(const char* id, Attribute& a)
{
    auto it = _attributes.find(id);
    if (it != _attributes.end()) {
        a = it->second;
        return true;
    }
    return false;
}

IAttributesEnumerator* CSVTrace::getAttrs()
{
    return new xi::collections::Enumerator4ConstIterator<decltype(_attributes)::const_iterator, IEventLog::NamedAttribute>(_attributes.cbegin(), _attributes.cend());
}

IEvent* CSVTrace::getEvent(UInt eventNum)
{
    if (eventNum < _events.size()) {
        return _events[eventNum];
    }
    return nullptr;
}

int CSVTrace::getSize()
{
    return static_cast<int>(_events.size());
}

IEventLog* CSVTrace::getLog()
{
    return _log;
}

void CSVTrace::addEvent(CSVEvent* event)
{
    event->setTrace(this);
    _events.push_back(event);
}

void CSVTrace::setLog(IEventLog* log)
{
    _log = log;
}

void CSVTrace::setAttribute(const std::string& id, const Attribute& attr) {
    _attributes[id] = attr;
}

// CSVLog implementation
CSVLog::CSVLog()
    : _isOpen(false)
{
}

CSVLog::~CSVLog()
{
    close();
    clear();
}

void CSVLog::open()
{
    if (!_isOpen) {
        _file.open(_fileName);
        if (!_file.is_open()) {
            throw LdopaException::f("File %s cannot be open", _fileName.c_str());
        }
        _isOpen = true;
        parseCSVFile();
    }
}

void CSVLog::close()
{
    if (_isOpen) {
        _file.close();
        _isOpen = false;
    }
}

bool CSVLog::isOpen()
{
    return _isOpen;
}

int CSVLog::getEventsNum()
{
    int total = 0;
    for (auto trace : _traces) {
        total += trace->getSize();
    }
    return total;
}

int CSVLog::getTracesNum()
{
    return static_cast<int>(_traces.size());
}

int CSVLog::getActivitiesNum()
{
    return static_cast<int>(_activitiesPool.getPool().size());
}

int CSVLog::getLogAttrsNum()
{
    return static_cast<int>(_attributes.size());
}

bool CSVLog::getLogAttr(const char* id, Attribute& a)
{
    auto it = _attributes.find(id);
    if (it != _attributes.end()) {
        a = it->second;
        return true;
    }
    return false;
}

IAttributesEnumerator* CSVLog::getLogAttrs()
{
    return new xi::collections::Enumerator4ConstIterator<decltype(_attributes)::const_iterator, IEventLog::NamedAttribute>(_attributes.cbegin(), _attributes.cend());
}

bool CSVLog::getEventAttr(int traceNum, int eventNum, const char* id, Attribute& a)
{
    if (traceNum < static_cast<int>(_traces.size())) {
        IEvent* event = _traces[traceNum]->getEvent(eventNum);
        if (event) {
            return event->getAttr(id, a);
        }
    }
    return false;
}

bool CSVLog::getEventAttr(int eventId, const char* id, Attribute& a)
{
    int currentEvent = 0;
    for (auto trace : _traces) {
        for (int i = 0; i < trace->getSize(); ++i) {
            if (currentEvent == eventId) {
                return trace->getEvent(i)->getAttr(id, a);
            }
            ++currentEvent;
        }
    }
    return false;
}

IAttributesEnumerator* CSVLog::getEventAttrs(int traceNum, int eventNum)
{
    if (traceNum < static_cast<int>(_traces.size())) {
        IEvent* event = _traces[traceNum]->getEvent(eventNum);
        if (event) {
            return event->getAttrs();
        }
    }
    return nullptr;
}

IAttributesEnumerator* CSVLog::getEventAttrs(int eventId)
{
    int currentEvent = 0;
    for (auto trace : _traces) {
        for (int i = 0; i < trace->getSize(); ++i) {
            if (currentEvent == eventId) {
                return trace->getEvent(i)->getAttrs();
            }
            ++currentEvent;
        }
    }
    return nullptr;
}

bool CSVLog::getTraceAttr(int traceNum, const char* id, Attribute& a)
{
    if (traceNum < static_cast<int>(_traces.size())) {
        return _traces[traceNum]->getAttr(id, a);
    }
    return false;
}

IAttributesEnumerator* CSVLog::getTraceAttrs(int traceNum)
{
    if (traceNum < static_cast<int>(_traces.size())) {
        return _traces[traceNum]->getAttrs();
    }
    return nullptr;
}

int CSVLog::getTraceSize(int traceNum)
{
    if (traceNum < static_cast<int>(_traces.size())) {
        return _traces[traceNum]->getSize();
    }
    return 0;
}

std::string CSVLog::getEvActAttrId() const
{
    return "concept:name";
}

std::string CSVLog::getEvTimestAttrId() const
{
    return "time:timestamp";
}

std::string CSVLog::getEvCaseAttrId() const
{
    return "case:concept:name";
}

std::string CSVLog::getInfoStr() const
{
    std::stringstream ss;
    ss << "CSV Log: " << _fileName;
    return ss.str();
}

IEventTrace* CSVLog::getTrace(int traceNum)
{
    if (traceNum < static_cast<int>(_traces.size())) {
        return _traces[traceNum];
    }
    return nullptr;
}

void CSVLog::setFileName(const std::string& fileName)
{
    if (_isOpen) {
        throw LdopaException::f("File %s is already open", _fileName.c_str());
    }
    _fileName = fileName;
}

const std::string& CSVLog::getFileName() const
{
    return _fileName;
}

void CSVLog::clear()
{
    for (auto trace : _traces) {
        delete trace;
    }
    _traces.clear();
    _attributes.clear();
    _activitiesPool.clear();
}

void CSVLog::parseCSVFile()
{
    std::string line;
    // Пропускаем заголовок
    std::getline(_file, line);
    
    while (std::getline(_file, line)) {
        parseLine(line);
    }
}

void CSVLog::parseLine(const std::string& line)
{
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    
    // Разбиваем строку на токены по разделителю
    while (std::getline(ss, token, ';')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 3) {
        throw LdopaException::f("Invalid CSV line format: %s", line.c_str());
    }
    
    // Получаем основные атрибуты
    std::string caseId = tokens[0];
    std::string activity = tokens[1];
    std::string timestamp = tokens[2];
    
    // Создаем или получаем существующий трейс
    CSVTrace* trace = nullptr;
    Attribute attr;
    for (auto t : _traces) {
        if (t->getAttr("case:concept:name", attr)) {
            std::string traceCaseId;
            if (attr.getType() == Attribute::tStringSharedPtr) {
                traceCaseId = *boost::get<xi::attributes::StringSharedPtr>(attr.getRef());
            } else if (attr.getType() == Attribute::tCStrSharedArr) {
                traceCaseId = boost::get<xi::attributes::CStrSharedArr>(attr.getRef()).get();
            }
            if (traceCaseId == caseId) {
                trace = static_cast<CSVTrace*>(t);
                break;
            }
        }
    }
    
    if (!trace) {
        trace = new CSVTrace();
        trace->setLog(this);
        trace->setAttribute("case:concept:name", Attribute(caseId));
        _traces.push_back(trace);
    }
    
    // Создаем событие
    CSVEvent* event = new CSVEvent(activity);
    event->setTrace(trace);
    event->setAttribute("concept:name", Attribute(activity));
    event->setAttribute("time:timestamp", Attribute(timestamp));
    event->setAttribute("case:concept:name", Attribute(caseId));
    
    // Добавляем дополнительные атрибуты, если они есть
    for (size_t i = 3; i < tokens.size(); ++i) {
        std::string attrName = "attr" + std::to_string(i-2);
        event->setAttribute(attrName, Attribute(tokens[i]));
    }
    
    // Добавляем событие в трейс
    trace->addEvent(event);
    
    // Добавляем активность в пул
    _activitiesPool.insert(activity);
}

}}}} // namespace xi { namespace ldopa { namespace eventlog { namespace csvlog {