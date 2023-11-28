#ifndef PYSQLITE_UTIL_H
#define PYSQLITE_UTIL_H
#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/pythread.h"
#include "third_party/python/Modules/_sqlite/connection.h"
#include "third_party/sqlite3/sqlite3.h"

int pysqlite_step(sqlite3_stmt* statement, pysqlite_Connection* connection);

/**
 * Checks the SQLite error code and sets the appropriate DB-API exception.
 * Returns the error code (0 means no error occurred).
 */
int _pysqlite_seterror(sqlite3* db, sqlite3_stmt* st);

PyObject * _pysqlite_long_from_int64(sqlite_int64 value);
sqlite_int64 _pysqlite_long_as_int64(PyObject * value);

#endif
