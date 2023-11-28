#ifndef Py_LIMITED_API
#ifndef Py_CELLOBJECT_H
#define Py_CELLOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

typedef struct {
	PyObject_HEAD
	PyObject *ob_ref;	/* Content of the cell or NULL when empty */
} PyCellObject;

extern PyTypeObject PyCell_Type;

#define PyCell_Check(op) (Py_TYPE(op) == &PyCell_Type)

PyObject * PyCell_New(PyObject *);
PyObject * PyCell_Get(PyObject *);
int PyCell_Set(PyObject *, PyObject *);

#define PyCell_GET(op) (((PyCellObject *)(op))->ob_ref)
#define PyCell_SET(op, v) (((PyCellObject *)(op))->ob_ref = v)

COSMOPOLITAN_C_END_
#endif /* !Py_TUPLEOBJECT_H */
#endif /* Py_LIMITED_API */
