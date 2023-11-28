#ifndef PYSQLITE_PREPARE_PROTOCOL_H
#define PYSQLITE_PREPARE_PROTOCOL_H
#include "third_party/python/Include/Python.h"

typedef struct
{
    PyObject_HEAD
} pysqlite_PrepareProtocol;

extern PyTypeObject pysqlite_PrepareProtocolType;

int pysqlite_prepare_protocol_init(pysqlite_PrepareProtocol* self, PyObject* args, PyObject* kwargs);
void pysqlite_prepare_protocol_dealloc(pysqlite_PrepareProtocol* self);

int pysqlite_prepare_protocol_setup_types(void);

#define UNKNOWN (-1)
#endif
