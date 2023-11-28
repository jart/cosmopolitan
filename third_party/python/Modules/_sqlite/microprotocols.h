#ifndef PSYCOPG_MICROPROTOCOLS_H
#define PSYCOPG_MICROPROTOCOLS_H 1
#include "third_party/python/Include/Python.h"
#include "third_party/python/Modules/_sqlite/cursor.h"

/** adapters registry **/

extern PyObject *psyco_adapters;

/** the names of the three mandatory methods **/

#define MICROPROTOCOLS_GETQUOTED_NAME "getquoted"
#define MICROPROTOCOLS_GETSTRING_NAME "getstring"
#define MICROPROTOCOLS_GETBINARY_NAME "getbinary"

/** exported functions **/

/* used by module.c to init the microprotocols system */
extern int pysqlite_microprotocols_init(PyObject *dict);
extern int pysqlite_microprotocols_add(
    PyTypeObject *type, PyObject *proto, PyObject *cast);
extern PyObject *pysqlite_microprotocols_adapt(
    PyObject *obj, PyObject *proto, PyObject *alt);

extern PyObject *
    pysqlite_adapt(pysqlite_Cursor* self, PyObject *args);
#define pysqlite_adapt_doc \
    "adapt(obj, protocol, alternate) -> adapt obj to given protocol. Non-standard."

#endif /* !defined(PSYCOPG_MICROPROTOCOLS_H) */
