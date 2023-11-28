#ifndef PYSQLITE_MODULE_H
#define PYSQLITE_MODULE_H
#include "third_party/python/Include/Python.h"

#define PYSQLITE_VERSION "2.6.0"

extern PyObject* pysqlite_Error;
extern PyObject* pysqlite_Warning;
extern PyObject* pysqlite_InterfaceError;
extern PyObject* pysqlite_DatabaseError;
extern PyObject* pysqlite_InternalError;
extern PyObject* pysqlite_OperationalError;
extern PyObject* pysqlite_ProgrammingError;
extern PyObject* pysqlite_IntegrityError;
extern PyObject* pysqlite_DataError;
extern PyObject* pysqlite_NotSupportedError;

/* A dictionary, mapping column types (INTEGER, VARCHAR, etc.) to converter
 * functions, that convert the SQL value to the appropriate Python value.
 * The key is uppercase.
 */
extern PyObject* _pysqlite_converters;

extern int _pysqlite_enable_callback_tracebacks;
extern int pysqlite_BaseTypeAdapted;

#define PARSE_DECLTYPES 1
#define PARSE_COLNAMES 2
#endif
