#ifndef PYSQLITE_CONNECTION_H
#define PYSQLITE_CONNECTION_H
#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/pythread.h"
#include "third_party/python/Include/structmember.h"
#include "third_party/python/Modules/_sqlite/cache.h"
#include "third_party/python/Modules/_sqlite/module.h"
#include "third_party/sqlite3/sqlite3.h"

typedef struct
{
    PyObject_HEAD
    sqlite3* db;

    /* the type detection mode. Only 0, PARSE_DECLTYPES, PARSE_COLNAMES or a
     * bitwise combination thereof makes sense */
    int detect_types;

    /* the timeout value in seconds for database locks */
    double timeout;

    /* for internal use in the timeout handler: when did the timeout handler
     * first get called with count=0? */
    double timeout_started;

    /* None for autocommit, otherwise a PyUnicode with the isolation level */
    PyObject* isolation_level;

    /* NULL for autocommit, otherwise a string with the BEGIN statement */
    const char* begin_statement;

    /* 1 if a check should be performed for each API call if the connection is
     * used from the same thread it was created in */
    int check_same_thread;

    int initialized;

    /* thread identification of the thread the connection was created in */
    long thread_ident;

    pysqlite_Cache* statement_cache;

    /* Lists of weak references to statements and cursors used within this connection */
    PyObject* statements;
    PyObject* cursors;

    /* Counters for how many statements/cursors were created in the connection. May be
     * reset to 0 at certain intervals */
    int created_statements;
    int created_cursors;

    PyObject* row_factory;

    /* Determines how bytestrings from SQLite are converted to Python objects:
     * - PyUnicode_Type:        Python Unicode objects are constructed from UTF-8 bytestrings
     * - PyBytes_Type:          The bytestrings are returned as-is.
     * - Any custom callable:   Any object returned from the callable called with the bytestring
     *                          as single parameter.
     */
    PyObject* text_factory;

    /* remember references to functions/classes used in
     * create_function/create/aggregate, use these as dictionary keys, so we
     * can keep the total system refcount constant by clearing that dictionary
     * in connection_dealloc */
    PyObject* function_pinboard;

    /* a dictionary of registered collation name => collation callable mappings */
    PyObject* collations;

    /* Exception objects */
    PyObject* Warning;
    PyObject* Error;
    PyObject* InterfaceError;
    PyObject* DatabaseError;
    PyObject* DataError;
    PyObject* OperationalError;
    PyObject* IntegrityError;
    PyObject* InternalError;
    PyObject* ProgrammingError;
    PyObject* NotSupportedError;
} pysqlite_Connection;

extern PyTypeObject pysqlite_ConnectionType;

PyObject* pysqlite_connection_alloc(PyTypeObject* type, int aware);
void pysqlite_connection_dealloc(pysqlite_Connection* self);
PyObject* pysqlite_connection_cursor(pysqlite_Connection* self, PyObject* args, PyObject* kwargs);
PyObject* pysqlite_connection_close(pysqlite_Connection* self, PyObject* args);
PyObject* _pysqlite_connection_begin(pysqlite_Connection* self);
PyObject* pysqlite_connection_commit(pysqlite_Connection* self, PyObject* args);
PyObject* pysqlite_connection_rollback(pysqlite_Connection* self, PyObject* args);
PyObject* pysqlite_connection_new(PyTypeObject* type, PyObject* args, PyObject* kw);
int pysqlite_connection_init(pysqlite_Connection* self, PyObject* args, PyObject* kwargs);

int pysqlite_connection_register_cursor(pysqlite_Connection* connection, PyObject* cursor);
int pysqlite_check_thread(pysqlite_Connection* self);
int pysqlite_check_connection(pysqlite_Connection* con);

int pysqlite_connection_setup_types(void);

#endif
