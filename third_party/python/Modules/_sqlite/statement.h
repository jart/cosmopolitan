#ifndef PYSQLITE_STATEMENT_H
#define PYSQLITE_STATEMENT_H
#include "third_party/python/Include/Python.h"
#include "third_party/python/Modules/_sqlite/connection.h"
#include "third_party/sqlite3/sqlite3.h"

#define PYSQLITE_TOO_MUCH_SQL (-100)
#define PYSQLITE_SQL_WRONG_TYPE (-101)

typedef struct
{
    PyObject_HEAD
    sqlite3* db;
    sqlite3_stmt* st;
    PyObject* sql;
    int in_use;
    int is_dml;
    PyObject* in_weakreflist; /* List of weak references */
} pysqlite_Statement;

extern PyTypeObject pysqlite_StatementType;

int pysqlite_statement_create(pysqlite_Statement* self, pysqlite_Connection* connection, PyObject* sql);
void pysqlite_statement_dealloc(pysqlite_Statement* self);

int pysqlite_statement_bind_parameter(pysqlite_Statement* self, int pos, PyObject* parameter);
void pysqlite_statement_bind_parameters(pysqlite_Statement* self, PyObject* parameters);

int pysqlite_statement_recompile(pysqlite_Statement* self, PyObject* parameters);
int pysqlite_statement_finalize(pysqlite_Statement* self);
int pysqlite_statement_reset(pysqlite_Statement* self);
void pysqlite_statement_mark_dirty(pysqlite_Statement* self);

int pysqlite_statement_setup_types(void);

#endif
