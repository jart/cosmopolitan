#ifndef Py_ITEROBJECT_H
#define Py_ITEROBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

extern PyTypeObject PySeqIter_Type;
extern PyTypeObject PyCallIter_Type;
extern PyTypeObject PyCmpWrapper_Type;

#define PySeqIter_Check(op) (Py_TYPE(op) == &PySeqIter_Type)

PyObject * PySeqIter_New(PyObject *);


#define PyCallIter_Check(op) (Py_TYPE(op) == &PyCallIter_Type)

PyObject * PyCallIter_New(PyObject *, PyObject *);

COSMOPOLITAN_C_END_
#endif /* !Py_ITEROBJECT_H */

