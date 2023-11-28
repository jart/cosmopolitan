#ifndef PYSQLITE_CURSOR_H
#define PYSQLITE_CURSOR_H
#include "third_party/python/Include/Python.h"

#include "third_party/python/Modules/_sqlite/statement.h"
#include "third_party/python/Modules/_sqlite/connection.h"
#include "third_party/python/Modules/_sqlite/module.h"
#include "third_party/python/Modules/_sqlite/connection.h"

typedef struct
{
    PyObject_HEAD
    pysqlite_Connection* connection;
    PyObject* description;
    PyObject* row_cast_map;
    int arraysize;
    PyObject* lastrowid;
    long rowcount;
    PyObject* row_factory;
    pysqlite_Statement* statement;
    int closed;
    int reset;
    int locked;
    int initialized;

    /* the next row to be returned, NULL if no next row available */
    PyObject* next_row;

    PyObject* in_weakreflist; /* List of weak references */
} pysqlite_Cursor;

extern PyTypeObject pysqlite_CursorType;

PyObject* pysqlite_cursor_execute(pysqlite_Cursor* self, PyObject* args);
PyObject* pysqlite_cursor_executemany(pysqlite_Cursor* self, PyObject* args);
PyObject* pysqlite_cursor_getiter(pysqlite_Cursor *self);
PyObject* pysqlite_cursor_iternext(pysqlite_Cursor *self);
PyObject* pysqlite_cursor_fetchone(pysqlite_Cursor* self, PyObject* args);
PyObject* pysqlite_cursor_fetchmany(pysqlite_Cursor* self, PyObject* args, PyObject* kwargs);
PyObject* pysqlite_cursor_fetchall(pysqlite_Cursor* self, PyObject* args);
PyObject* pysqlite_noop(pysqlite_Connection* self, PyObject* args);
PyObject* pysqlite_cursor_close(pysqlite_Cursor* self, PyObject* args);

int pysqlite_cursor_setup_types(void);

#define UNKNOWN (-1)
#endif
