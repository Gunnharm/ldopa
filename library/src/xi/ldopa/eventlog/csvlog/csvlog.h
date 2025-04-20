#ifndef XI_LDOPA_EVENTLOG_CSVLOG_CSVLOG_H_
#define XI_LDOPA_EVENTLOG_CSVLOG_CSVLOG_H_

#pragma once

// std
#include <fstream>
#include <string>
#include <vector>
#include <map>

// ldopa
#include "xi/ldopa/ldopa_dll.h"
#include "xi/ldopa/eventlog/eventlog.h"
#include "xi/strutils/set_str_pool.h"

namespace xi { namespace ldopa { namespace eventlog { namespace csvlog {

class CSVTrace;

class LDOPA_API CSVEvent : public IEvent {
public:
    CSVEvent(const std::string& activity);
    virtual ~CSVEvent() override;

    // IEvent interface
    virtual bool getAttr(const char* id, Attribute& a) override;
    virtual int getAttrsNum() override;
    virtual IAttributesEnumerator* getAttrs() override;
    virtual IEventTrace* getTrace() override;

    // Дополнительные методы
    void setTrace(IEventTrace* trace);
    void setAttribute(const std::string& id, const Attribute& attr);

private:
    std::string _activity;
    std::map<std::string, Attribute> _attributes;
    IEventTrace* _trace;

    friend class CSVTrace;
};

class LDOPA_API CSVTrace : public IEventTrace {
public:
    CSVTrace();
    virtual ~CSVTrace() override;

    // IEventTrace interface
    virtual int getAttrsNum() override;
    virtual bool getAttr(const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getAttrs() override;
    virtual IEvent* getEvent(UInt eventNum) override;
    virtual int getSize() override;
    virtual IEventLog* getLog() override;

    // Дополнительные методы
    void addEvent(CSVEvent* event);
    void setLog(IEventLog* log);
    void setAttribute(const std::string& id, const Attribute& attr);

private:
    std::vector<CSVEvent*> _events;
    std::map<std::string, Attribute> _attributes;
    IEventLog* _log;

    friend class CSVLog;
};

class LDOPA_API CSVLog : public IEventLog {
public:
    CSVLog();
    virtual ~CSVLog() override;

    // IEventLog interface
    virtual void open() override;
    virtual void close() override;
    virtual bool isOpen() override;
    virtual int getEventsNum() override;
    virtual int getTracesNum() override;
    virtual int getActivitiesNum() override;
    virtual int getLogAttrsNum() override;
    virtual bool getLogAttr(const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getLogAttrs() override;
    virtual bool getEventAttr(int traceNum, int eventNum, const char* id, Attribute& a) override;
    virtual bool getEventAttr(int eventId, const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getEventAttrs(int traceNum, int eventNum) override;
    virtual IAttributesEnumerator* getEventAttrs(int eventId) override;
    virtual bool getTraceAttr(int traceNum, const char* id, Attribute& a) override;
    virtual IAttributesEnumerator* getTraceAttrs(int traceNum) override;
    virtual int getTraceSize(int traceNum) override;
    virtual std::string getEvActAttrId() const override;
    virtual std::string getEvTimestAttrId() const override;
    virtual std::string getEvCaseAttrId() const override;
    virtual std::string getInfoStr() const override;
    virtual IEventTrace* getTrace(int traceNum) override;

    // Дополнительные методы
    void setFileName(const std::string& fileName);
    const std::string& getFileName() const;

private:
    void parseCSVFile();
    void parseLine(const std::string& line);
    void clear();

private:
    std::string _fileName;
    std::ifstream _file;
    bool _isOpen;
    std::vector<CSVTrace*> _traces;
    std::map<std::string, Attribute> _attributes;
    xi::strutils::SetStrPool _activitiesPool;
};

}}}} // namespace xi { namespace ldopa { namespace eventlog { namespace csvlog {

#endif // XI_LDOPA_EVENTLOG_CSVLOG_CSVLOG_H_ 