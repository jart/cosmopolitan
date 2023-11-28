#ifndef PYSQLITE_ROW_H
#define PYSQLITE_ROW_H
#include "third_party/python/Include/Python.h"

typedef struct _Row
{
    PyObject_HEAD
    PyObject* data;
    PyObject* description;
} pysqlite_Row;

extern PyTypeObject pysqlite_RowType;

int pysqlite_row_setup_types(void);

#endif
