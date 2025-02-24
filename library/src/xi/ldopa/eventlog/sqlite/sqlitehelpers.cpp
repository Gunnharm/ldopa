////////////////////////////////////////////////////////////////////////////////
// Module Name:  sqlitelog.h/cpp
// Authors:      Sergey Shershakov
// Version:      0.1.0
// Date:         23.08.2017
// Copyright (c) xidv.ru 2014–2017.
//
// This source is for internal use only — Restricted Distribution.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////


// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

// ldopa
#include "xi/ldopa/eventlog/sqlite/sqlitehelpers.h"
#include "xi/ldopa/utils.h"


namespace xi {namespace ldopa {;   //


//==============================================================================
// class SQLiteDB
//==============================================================================

SQLiteDB::SQLiteDB()
    : _dbHandle(NULL)
    , _sqlExecCB(nullptr)
{
    reset();
}

//------------------------------------------------------------------------------

SQLiteDB::SQLiteDB(const std::string& fn)
    : SQLiteDB()   
{
    _fileName = fn;
}

//------------------------------------------------------------------------------

SQLiteDB::~SQLiteDB()
{
    close();
}

//------------------------------------------------------------------------------

void SQLiteDB::open()
{
    // если уже открыт, ничего более не открываем
    if (isOpen())
        return;

    // открываем БД
    openDB();
}

//------------------------------------------------------------------------------

void SQLiteDB::close()
{
    // неоткрытый закрыть нельзя
    if (!isOpen())
        return;

    closeDB();
    reset();
}

//------------------------------------------------------------------------------

void SQLiteDB::setFileName(const std::string& fn)
{
    if (isOpen())
        throw LdopaException("Can't change a file name while a DB is open.");

    _fileName = fn;
}

//------------------------------------------------------------------------------

void SQLiteDB::openDB()
{
    // https://www.sqlite.org/c3ref/open.html
    // т.к. метод закрытый, не делаем проверок, был ли файл уже открыт

    if (_fileName.empty())
        throw LdopaException("Cannot open a DB file because the filename is empty.");        

    _sqlLastCode = sqlite3_open(_fileName.c_str(), &_dbHandle);
    if (_sqlLastCode != SQLITE_OK)
    {
        if (_dbHandle != NULL)
        {
            throw LdopaException::f
            //LdopaException::throwException
                ("Can't open DB %s. Error: %d: %s",
                    _fileName.c_str(), _sqlLastCode, sqlite3_errmsg(_dbHandle));
        }
        else
        {
            throw LdopaException::f
            //LdopaException::throwException
                ("Can't open DB %s. Error: %d",
                    _fileName.c_str(), _sqlLastCode);
        }
    }
}

//------------------------------------------------------------------------------

void SQLiteDB::closeDB()
{
    // https://www.sqlite.org/c3ref/close.html
    sqlite3_close(_dbHandle);
    _dbHandle = NULL;               // вне зависимости от результата
}

//------------------------------------------------------------------------------

void SQLiteDB::reset()
{
    _sqlLastCode = 0;
    _sqlLastErrMsg.clear();
}

//------------------------------------------------------------------------------

int SQLiteDB::sqlExecCallback(void* sql, int colsNum, char** colsData, char** colNames)
{
    if (!sql)
        throw LdopaException("Bad callback for sqlExec.");

    SQLiteDB* self = (SQLiteDB*)sql;
    return self->sqlExecCallback(colsNum, colsData, colNames);
}

//------------------------------------------------------------------------------

int SQLiteDB::sqlExecCallback(int colsNum, char** colsData, char** colNames)
{
    if (_sqlExecCB)
        return _sqlExecCB->sqlExecCallback(this, colsNum, colsData, colNames);

    return 0;           // TODO: возможно тут надо -1, чего зазря гонять?!
}

//------------------------------------------------------------------------------

void SQLiteDB::extractLastErrMsg()
{
    // https://www.sqlite.org/c3ref/errcode.html
    // память управляется SQLite-ом автоматически
    _sqlLastErrMsg = sqlite3_errmsg(_dbHandle);     
}

//------------------------------------------------------------------------------

void SQLiteDB::checkResForOk(int errCode, const char* msg)
{
    _sqlLastCode = errCode;
    if (_sqlLastCode != SQLITE_OK)
    {
        extractLastErrMsg();

        // здесь только код и расшифровку SQL-ошибки. концептуальную — перехватывать в вызывающем
        throw LdopaException::f(msg, _sqlLastCode, _sqlLastErrMsg.c_str());
    }
}

//------------------------------------------------------------------------------

void SQLiteDB::sqlExec(const char* sql)
{
    sqlExec(sql, this, sqlExecCallback);
}

//------------------------------------------------------------------------------


void SQLiteDB::sqlExec(const char* sql, void* inv, SqlVoidCallback cb)
{
    if (!isOpen())
        throw LdopaException("Can't execute SQL: DB not open.");

    char* errmsg;
    _sqlLastCode = sqlite3_exec(
            _dbHandle,          /* An open database */
            sql,                /* SQL to be evaluated */
            cb,                 /* Callback function */
            inv,                /* 1st argument to callback (caller) */
            &errmsg             /* Error msg written here */
        );

    // TODO: переиспользовать checkResForOk()
    if (_sqlLastCode != SQLITE_OK)
    {
        _sqlLastErrMsg = errmsg;
        sqlite3_free(errmsg);

        // здесь только код и расшифровку SQL-ошибки. концептуальную — перехватывать в вызывающем
        throw LdopaException::f("SqlExec (%d): %s", _sqlLastCode, _sqlLastErrMsg.c_str());
    }
}

//------------------------------------------------------------------------------

SQLiteStmt* SQLiteDB::execSql(const char* sql, int qrySz /*= -1*/)
{
    if (!isOpen())
        throw LdopaException("Can't execute SQL: DB not open.");

    sqlite3_stmt* pStmt;
    const char* zTail;

    
    _sqlLastCode = sqlite3_prepare_v2(
            _dbHandle,          /* Database handle */
            sql,                /* SQL statement, UTF-8 encoded */
            qrySz,              /* Maximum length of zSql in bytes. */
            &pStmt,             /* OUT: Statement handle */
            &zTail              /* OUT: Pointer to unused portion of zSql */
        );

    // TODO: переиспользовать checkResForOk()
    if (_sqlLastCode != SQLITE_OK)
    {
        extractLastErrMsg();

        // здесь только код и расшифровку SQL-ошибки. концептуальную — перехватывать в вызывающем
        throw LdopaException::f("ExecSql (%d): %s", _sqlLastCode, _sqlLastErrMsg.c_str());
    } 

    // если все хорошо, создаем объект, который будет в дальнейшем управлять всей этой историей
    SQLiteStmt* s = new SQLiteStmt(this, pStmt, zTail);

    return s;
}


//==============================================================================
// class SQLiteStmt
//==============================================================================


SQLiteStmt::SQLiteStmt(SQLiteDB* owner, sqlite3_stmt* stmt, const char* zTail)
    : _owner(owner)
    , _stmt(stmt)
    , _zTail(zTail)
    , _lastRes(0)
    , _colsNum(-1)
{
    // к моменту создания объекта уже должно быть знание про число колонок
    // https://www.sqlite.org/c3ref/column_count.html
    // также важно сбрасывать при reset()
    // также есть метод sqlite3_data_count(), работающий для текущей запрошенной строки
    // https://www.sqlite.org/c3ref/data_count.html
    _colsNum = sqlite3_column_count(_stmt);
}

//------------------------------------------------------------------------------


SQLiteStmt::~SQLiteStmt()
{
    // лучше поздно, чем никогда (после профайлинга 16.02.2018)
    // https://www.sqlite.org/c3ref/finalize.html
    sqlite3_finalize(_stmt);
}

//------------------------------------------------------------------------------

bool SQLiteStmt::fetch()
{
    // если последний (предыдущий) результат сказал, что закончился перебор,
    // а его по-прежнему пытают, полетит эксцепция

    if (_lastRes == SQLITE_DONE)
        throw LdopaException::f("Can't fetch again with finished statement.");
        //LdopaException::throwException("Can't fetch again with finished statement.");

    _lastRes = sqlite3_step(_stmt);
    if (_lastRes == SQLITE_DONE)        // перебор закончился
        return false;                   

    if (_lastRes == SQLITE_ROW)         // очередная строка есть
    {
        //_colsNum = sqlite3_data_count(_stmt);
        return true;
    }

    // иначе какая-то проблема
    _owner->extractLastErrMsg();
    throw LdopaException::f("Fetch (%d): %s", _lastRes, _owner->_sqlLastErrMsg.c_str());
}

//------------------------------------------------------------------------------

void SQLiteStmt::reset()
{
    _owner->checkResForOk(
        sqlite3_reset(_stmt),
        "Resetting (%d): %s");
    _lastRes = 0;                   // важно, чтобы сбросить 101-код 
}


//------------------------------------------------------------------------------

const char* SQLiteStmt::getColumnName(int iCol) const
{
    checkColNum4Exception(iCol);
    return sqlite3_column_name(_stmt, iCol);
}

//------------------------------------------------------------------------------

const wchar_t* SQLiteStmt::getColumnWName(int iCol) const
{
    checkColNum4Exception(iCol);
    return (const wchar_t*)sqlite3_column_name16(_stmt, iCol);
}


//------------------------------------------------------------------------------

const char* SQLiteStmt::getOriginName(int iCol) const
{
    checkColNum4Exception(iCol);
    return sqlite3_column_origin_name(_stmt, iCol);
}

//------------------------------------------------------------------------------

const wchar_t* SQLiteStmt::getOriginWName(int iCol) const
{
    checkColNum4Exception(iCol);
    return (const wchar_t*)sqlite3_column_origin_name16(_stmt, iCol);
}

//------------------------------------------------------------------------------

const char* SQLiteStmt::getTableName(int iCol) const
{
    checkColNum4Exception(iCol);
    return sqlite3_column_table_name(_stmt, iCol);
}

//------------------------------------------------------------------------------

const wchar_t* SQLiteStmt::getTableWName(int iCol) const
{
    checkColNum4Exception(iCol);
    return (const wchar_t*)sqlite3_column_table_name16(_stmt, iCol);
}

//------------------------------------------------------------------------------

const char* SQLiteStmt::getDbName(int iCol) const
{
    checkColNum4Exception(iCol);
    return sqlite3_column_database_name(_stmt, iCol);
}

//------------------------------------------------------------------------------

const wchar_t* SQLiteStmt::getDbWName(int iCol) const
{
    checkColNum4Exception(iCol);
    return (const wchar_t*)sqlite3_column_database_name16(_stmt, iCol);
}

//------------------------------------------------------------------------------

SQLiteDB::SqlTypes SQLiteStmt::getType(int iCol) const
{
    checkColNum4Exception(iCol);
    return (SQLiteDB::SqlTypes)sqlite3_column_type(_stmt, iCol);
}

//------------------------------------------------------------------------------

const void* SQLiteStmt::getBlob(int iCol, int& size)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    // важно: рекомендуемая последовательность: сперва запросить тип, потом - размер!
    // см. https://www.sqlite.org/c3ref/column_blob.html

    const void* p = sqlite3_column_blob(_stmt, iCol);
    size = sqlite3_column_bytes(_stmt, iCol);
    return p;

}

//------------------------------------------------------------------------------

double SQLiteStmt::getDouble(int iCol)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    return sqlite3_column_double(_stmt, iCol);
}

//------------------------------------------------------------------------------

int SQLiteStmt::getInt(int iCol)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    return sqlite3_column_int(_stmt, iCol);
}

//------------------------------------------------------------------------------

types::TInt64 SQLiteStmt::getInt64(int iCol)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    return sqlite3_column_int64(_stmt, iCol);
}

//------------------------------------------------------------------------------

const unsigned char* SQLiteStmt::getCStr(int iCol, int& size)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    // важно: рекомендуемая последовательность: сперва запросить тип, потом - размер!
    // см. https://www.sqlite.org/c3ref/column_blob.html

    const unsigned char* s = sqlite3_column_text(_stmt, iCol);
    size = sqlite3_column_bytes(_stmt, iCol);
    return s;
}

//------------------------------------------------------------------------------

const wchar_t* SQLiteStmt::getWCStr(int iCol, int& size)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    // важно: рекомендуемая последовательность: сперва запросить тип, потом - размер!
    // см. https://www.sqlite.org/c3ref/column_blob.html

    const wchar_t* s = (const wchar_t*)sqlite3_column_text16(_stmt, iCol);
    size = sqlite3_column_bytes16(_stmt, iCol);
    return s;
}

//------------------------------------------------------------------------------

std::string SQLiteStmt::getStr(int iCol)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    // важно: рекомендуемая последовательность: сперва запросить тип, потом - размер!

    const char* cStr = (const char*)sqlite3_column_text(_stmt, iCol);
    int size = sqlite3_column_bytes(_stmt, iCol);
    
    std::string buf(cStr, size);

    return buf;                     // по идее, тут должна move-семантика работать
}

//------------------------------------------------------------------------------

std::wstring SQLiteStmt::getWStr(int iCol)
{
    checkColNum4Exception(iCol);        // тут можно полететь исключением

    // важно: рекомендуемая последовательность: сперва запросить тип, потом - размер!

    const wchar_t* cStr = (const wchar_t*)sqlite3_column_text16(_stmt, iCol);
    int size = sqlite3_column_bytes16(_stmt, iCol);    

    std::wstring buf(cStr, size / 2);

    return buf;                     // по идее, тут должна move-семантика работать
}

//------------------------------------------------------------------------------

void SQLiteStmt::bindText(int num, const char* str, int len /*= -1*/)
{
    _owner->checkResForOk(
        sqlite3_bind_text(_stmt, 
        num,                                // номер параметра
        str,                                // привязываемая строка
        len,                                // подсказка длины
        SQLITE_STATIC),                     // без деструктора (удаляет вызывающая сторона)
        "Binding (%d): %s");
}

//------------------------------------------------------------------------------

void SQLiteStmt::bindText(int num, const std::string& str)
{
    bindText(num, str.c_str(), str.length());
}

//------------------------------------------------------------------------------

void SQLiteStmt::bindTextCopy(int num, const char* str, int len /*= -1*/)
{
    _owner->checkResForOk(
        sqlite3_bind_text(_stmt,
        num,                                // номер параметра
        str,                                // привязываемая строка
        len,                                // подсказка длины
        SQLITE_TRANSIENT),                  // создает тут же копию и сама же ее удаляет (надеемся)
        "Binding w/ copy (%d): %s");
}

//------------------------------------------------------------------------------

void SQLiteStmt::bindInt(int num, int val)
{
    _owner->checkResForOk(
        sqlite3_bind_int(_stmt, num, val),                     
        "Binding (%d): %s");
}

//------------------------------------------------------------------------------

void SQLiteStmt::bindInt64(int num, types::TInt64 val)
{
    _owner->checkResForOk(
        sqlite3_bind_int64(_stmt, num, val),
        "Binding (%d): %s");
}

//------------------------------------------------------------------------------

void SQLiteStmt::bindDouble(int num, double val)
{
    _owner->checkResForOk(
        sqlite3_bind_double(_stmt, num, val),
        "Binding (%d): %s");
}

//------------------------------------------------------------------------------

void SQLiteStmt::clearBinding()
{
    _owner->checkResForOk(
        sqlite3_clear_bindings(_stmt),
        "Clearing bindings (%d): %s");
}

//------------------------------------------------------------------------------

const char* SQLiteStmt::getSql() const
{
    return sqlite3_sql(_stmt);
}

//------------------------------------------------------------------------------

void SQLiteStmt::checkColNum4Exception(int iCol) const
{
    if (iCol >= _colsNum)
        throw LdopaException("Wrong column number.");
}



//------------------------------------------------------------------------------

}} // namespace xi { namespace ldopa

