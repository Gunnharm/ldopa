////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     High-level objects for SQLite databased
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      24.08.2017
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Perhaps the classes need to be taken to a separate library.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_EVENTLOG_SQLITE_SQLITEHELPERS_H_
#define XI_LDOPA_EVENTLOG_SQLITE_SQLITEHELPERS_H_

#pragma once

// std
#include <bitset>
#include <string>


// ldopa dll
#include "xi/ldopa/ldopa_dll.h"

// xilib
#include "xi/types/aliases.h"

#include "sqlite3.h"

namespace xi { namespace ldopa {;   //

class LDOPA_API SQLiteStmt;

#pragma warning(push)
#pragma warning(disable: 4251)

/** \brief Adapter for sqlite-db object. */
class LDOPA_API SQLiteDB {
    friend class SQLiteStmt;
public:
    //-----<Types>-----

    /** \brief Defines SQLite fundamental types.
     *
     *  https://www.sqlite.org/c3ref/c_blob.html
     */
    enum SqlTypes {
        stInt = SQLITE_INTEGER,
        stDouble = SQLITE_FLOAT,
        stText = SQLITE3_TEXT,
        stBlob = SQLITE_BLOB,
        stNull = SQLITE_NULL
    }; 


    /** \brief Type for SQLite callback function. */
    typedef int(*SqlVoidCallback)(void*, int, char**, char**);


    /** \brief Declares an interface for SqlExec callback. 
     *
     *  See https://www.sqlite.org/c3ref/exec.html.
     */
    class ISqlExecCB {
    public:

        /** \brief Called back by sqlexec method. */
        virtual int sqlExecCallback(SQLiteDB* inv, int colsNum, char** colsData, char** colNames) = 0;
    protected:
        /** \brief Protected Destructor: It is not intended to delete an object through this interface! */
        ~ISqlExecCB() {};
    }; // class ISqlExecCallback


public:
    /** \brief Default constructor. */
    SQLiteDB();

    /** \brief Constructor with the filename of a DB. */
    SQLiteDB(const std::string& fn);

    /** \brief Destructor. */
    virtual ~SQLiteDB();

protected:
    // http ://stackoverflow.com/questions/6811037/whats-the-use-of-the-private-copy-constructor-in-c
    SQLiteDB(const SQLiteDB&);                    // Prevent copy-construction
    SQLiteDB& operator=(const SQLiteDB&);         // Prevent assignment

public:
    /** \brief Opens a db. If the db is already open, does nothing. */
    void open();
    
    /** \brief Closes the db. If the db is not open, does nothing. */
    void close();

    /** \brief Returns true if a db is open, false otherwise. */
    bool isOpen() const { return (_dbHandle != NULL); }

public:

    //-----<SQLite Operations>-----

    /** \brief Proxy for calling sqlite3_exec method.
     *
     *  Version with in-class callback handler (sqlExecCallback). If _sqlExecCB is set,
     *  it is called as a handler for this CB. Otherwise does nothing.
     *  If no DB open, throws an exception.
     *  If an error occured while executing, throws an execption.
     */
    void sqlExec(const char* sql);  // , SqlCallback cb);

    /** \brief Std::string adapter (1). */
    void sqlExec(const std::string& sql) { sqlExec(sql.c_str()); }

    /** \brief Variant with an object \a inv, whose callback \a cb is called. */
    void sqlExec(const char* sql, void* inv, SqlVoidCallback cb);

    /** \brief Std::string adapter (2). */
    void sqlExec(const std::string& sql, void* inv, SqlVoidCallback cb) { sqlExec(sql.c_str(), inv, cb); }


    /** \brief Prepare the given SQL statement \a qry and return an object
     *  for extracting resulting data.
     *
     *  Mostly wrapper for one of sqlite3_prepareXXX function.
     *  https://www.sqlite.org/c3ref/prepare.html
     *  If a database is not open, throws an exception.
     *  \returns SQLiteStmt* object, which must be cleared by the caller.
     */
    SQLiteStmt* execSql(const char* sql, int qrySz = -1);

    /** \brief std::string adapter (1). */
    SQLiteStmt* execSql(const std::string& sql) { return execSql(sql.c_str(), sql.length()); }

    /** \brief std::string adapter (1). */
    SQLiteStmt* execSql(const std::string* sql) { return execSql(sql->c_str(), sql->length()); }

public:
    /** \brief Sets a filename for a DB file. If a DB is already open, an exception is thrown. */
    void setFileName(const std::string& fn);

    /** \brief Returns a filename for a log */
    const std::string getFileName() const { return _fileName; }

    /** \brief Returns a callback for sqlExec if set. */
    ISqlExecCB* getSqlExecCB() { return _sqlExecCB; }

    /** \brief Const variant of getSqlExecCB(). */
    const ISqlExecCB* getSqlExecCB() const  { return _sqlExecCB; }

    /** \brief Sets a callback for sqlExec. */
    void getSqlExecCB(ISqlExecCB* cb) { _sqlExecCB = cb; }

protected:

    /** \brief Open an SQLite DB file.
     *
     *  If an error occured while opening the file, an exception is raised.
     */
    void openDB();

    /** \brief Closes the opened DB file internally. */
    void closeDB();

    /** \brief Prepares internal fields after close DB and before open a new one. */
    void reset();

    /** \brief Static callback function for dealing with sqlExec. */
    static int sqlExecCallback(void* sql, int colsNum, char** colsData, char** colNames);

    /** \brief "Objected" callback function for dealing with sqlExec. */
    int sqlExecCallback(int colsNum, char** colsData, char** colNames);

    /** \brief Retrieves a last error message from the SQLite connection to _sqlLastErrMsg. */
    void extractLastErrMsg();

    /** \brief Obtain an error code \a errCode (most often as a result of some method),
     *  and checks whether it is ok or not. If the code is not SQLITE_OK, retrieves the last
     *  error description and raises an exception with the given text \msg, which can contain
     *  placeholders for printf() formaters (1-st: %d errcode and 2-nd: %s message).
     */
    void checkResForOk(int errCode, const char* msg);

protected:
    /** \brief Database file name. */
    std::string _fileName;

    /** \brief Represent an internal handler of an opened SQLite connection. */
    sqlite3* _dbHandle;

    /** \brief Stores a last error code obtained after working with SQLite. */
    int _sqlLastCode;

    /** \brief Stores a last error message obtained after working with SQLite. */
    std::string _sqlLastErrMsg;

    /** \brief A handler for shorter version of sqlExec() method.  */
    ISqlExecCB* _sqlExecCB;

}; // class SQLiteDB

//==============================================================================

/** \brief Represent a SQLite statment object. */
class LDOPA_API SQLiteStmt {
    friend class SQLiteDB;
protected:
    /** \brief Protected constructor. Only SQLiteDB can create obect of this type. */
    SQLiteStmt(SQLiteDB* owner, sqlite3_stmt* stmt, const char* zTail);

public:
    /** \brief Destructor. */
    ~SQLiteStmt();
protected:
    SQLiteStmt(const SQLiteStmt&);                    // Prevent copy-construction
    SQLiteStmt& operator=(const SQLiteStmt&);         // Prevent assignment

public:
    //-----<Fetching>-----
    
    /** \brief Tries to fetch another string.
     *
     *  \returns true, if a row has been extracted, false at the very end (when no more rows
     *  to extract.
     *  Implements sqlite_step() function (https://www.sqlite.org/c3ref/step.html).
     */
    bool fetch();

    /** \brief Resets the statement, setting its state to the initial one.
     *
     *  https://www.sqlite.org/c3ref/reset.html
     */
    void reset();


    /** \brief Returns number of columns in the result set. 
     *  
     *  Will return -1 before first fetch(). Not null if fetch() returns true,
     *  return actual number of columns. If fetch() returns false, return 0.
     *  See https://www.sqlite.org/c3ref/data_count.html.
     */
    int getColsNum() { return _colsNum; }


    /** \brief Returns the name assigned to a particular column \a iCol 
     *  in the result set of a SELECT statement. 
     *
     *  https://www.sqlite.org/c3ref/column_name.html
     *  Checks column range and throws an exception if it is out of range.
     */
    const char* getColumnName(int iCol) const;

    /** \brief Wide-string version of SQLiteStmt::getColumnName(). */
    const wchar_t* getColumnWName(int iCol) const;


    /** \brief Returns the the original un-aliased names of the column 
     *  \a iCol.
     *
     *  https://www.sqlite.org/c3ref/column_name.html
     *  Checks column range and throws an exception if it is out of range.
     */
    const char* getOriginName(int iCol) const;

    /** \brief Wide-string version of SQLiteStmt::getOriginName(). */
    const wchar_t* getOriginWName(int iCol) const;


    /** \brief Returns the the original un-aliased names of the table of 
     *  the column \a iCol.
     *
     *  https://www.sqlite.org/c3ref/column_name.html
     *  Checks column range and throws an exception if it is out of range.
     */
    const char* getTableName(int iCol) const;

    /** \brief Wide-string version of SQLiteStmt::getTableName(). */
    const wchar_t* getTableWName(int iCol) const;


    /** \brief Returns the the original un-aliased names of the database of
     *  the column \a iCol.
     *
     *  https://www.sqlite.org/c3ref/column_name.html
     *  Checks column range and throws an exception if it is out of range.
     */
    const char* getDbName(int iCol) const;

    /** \brief Wide-string version of SQLiteStmt::getDbName(). */
    const wchar_t* getDbWName(int iCol) const;

    /** \brief Returns the SQLite fundamental datatype of a column \a iCol. */
    SQLiteDB::SqlTypes getType(int iCol) const;

    //-----<Getting Data>-----

    /** \brief Return data of a column \a iCol as BLOB. The size of BLOB
     *  object is determined by \a size.
     *
     *  If iCol > getColsNum(), throws an exception.
     *  https://www.sqlite.org/c3ref/column_blob.html
     */
    const void* getBlob(int iCol, int& size);

    /** \brief Return data of a column \a iCol as double.
     *
     *  If iCol > getColsNum(), throws an exception.
     */    
    double getDouble(int iCol);


    /** \brief Return data of a column \a iCol as Integer.
     *
     *  If iCol > getColsNum(), throws an exception.
     */
    int getInt(int iCol);


    /** \brief Return data of a column \a iCol as Integer-64.
     *
     *  If iCol > getColsNum(), throws an exception.
     */
    types::TInt64 getInt64(int iCol);


    /** \brief Return data of a column \a iCol as UTF-8 c-string.
     *  The number of bytes in the string is determined by \a size. It's not
     *  a number of cheracters, but a number of bytes!
     *
     *  If iCol > getColsNum(), throws an exception.
     */
    const unsigned char* getCStr(int iCol, int& size);

    /** \brief Return data of a column \a iCol as UTF-16 (wide) c-string. 
     *  The number of bytes in the string is determined by \a size. It's not
     *  a number of cheracters, but a number of bytes!
     *
     *  If iCol > getColsNum(), throws an exception.
     */
    const wchar_t* getWCStr(int iCol, int& size);

    /** \brief Return data of a column \a iCol as UTF-8 std::string.
     *
     *  If iCol > getColsNum(), throws an exception.
     */
    std::string getStr(int iCol);

    /** \brief Return data of a column \a iCol as UTF-16 (wide) std::string.
     *
     *  If iCol > getColsNum(), throws an exception.
     */
    std::wstring getWStr(int iCol);


    //-----<Binding Parameters>-----


    /** \brief Binds simple text parameter.
     *
     *  \param num is a number of SQL paramter to bind.
     *  \param str is an caller-managed string to bind as a parameter.
     *  \param len allow to give a hint for a length of a string. If it is equal to -1,
     *  the length is determined by the first occurst of \0 char.
     *  No encoding is given to account.
     */
    void bindText(int num, const char* str, int len = -1);

    /** \brief Binds simple text given as a std::string.
     *
     *  For details, see bindText(int, const char*, int).
     */
    void bindText(int num, const std::string& str);

    /** \brief Same as bindText, but asks SQLite to create its own managed copy of string. */
    void bindTextCopy(int num, const char* str, int len = -1);

    /** \brief Binds 32-int parameter.
     *
     *  \param num is a number of SQL paramter to bind.
     *  \param val is an integer value to bind.
     */
    void bindInt(int num, int val);


    /** \brief Binds 64-int parameter.
     *
     *  \param num is a number of SQL paramter to bind.
     *  \param val is an integer value to bind.
     */
    void bindInt64(int num, types::TInt64 val);


    /** \brief Binds double parameter.
     *
     *  \param num is a number of SQL paramter to bind.
     *  \param val is an integer value to bind.
     */
    void bindDouble(int num, double val);


    /** \brief Resets all bindings.
     *
     *  https://www.sqlite.org/c3ref/clear_bindings.html
     */
    void clearBinding();


    //-----<Utilities>-----

    /** \brief Returns a copy of the SQL query of the statement. 
     *  The string returned by the method is managed by SQLiteStmt and is automatically
     *  freed when the prepared statement is finalized.
     */
    const char* getSql() const;

    /** \brief Returns a unexecuted tail of the SQL query. */
    const char* getSqlTail() const { return _zTail; }

protected:
    
    /** \brief Checks if the given column \a iCol is in valid range of fetched columns. 
     *  If not, throws an exception.
     */
    void checkColNum4Exception(int iCol) const;

protected:
    
    /** \brief Owner DB. */
    SQLiteDB* _owner;
    
    /** \brief S raw SQLite statement object. */
    sqlite3_stmt* _stmt;

    /** \brief Points to the first byte past the end of the first SQL statement in qry. */
    const char* _zTail;

    /** \brief Stores last SQLite-result associated with this statement. */
    int _lastRes;

    /** \brief Number of columns. -1 is for undefined. */
    int _colsNum;

}; // class LDOPA_API SQLiteStmt


} //  namespace ldopa {
} // namespace xi {



#endif // XI_LDOPA_EVENTLOG_SQLITE_SQLITEHELPERS_H_
