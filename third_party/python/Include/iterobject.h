#ifndef Py_ITEROBJECT_H
#define Py_ITEROBJECT_H
COSMOPOLITAN_C_START_
/* clang-format off */

PyAPI_DATA(PyTypeObject) PySeqIter_Type;
PyAPI_DATA(PyTypeObject) PyCallIter_Type;
PyAPI_DATA(PyTypeObject) PyCmpWrapper_Type;

#define PySeqIter_Check(op) (Py_TYPE(op) == &PySeqIter_Type)

PyAPI_FUNC(PyObject *) PySeqIter_New(PyObject *);


#define PyCallIter_Check(op) (Py_TYPE(op) == &PyCallIter_Type)

PyAPI_FUNC(PyObject *) PyCallIter_New(PyObject *, PyObject *);

COSMOPOLITAN_C_END_
#endif /* !Py_ITEROBJECT_H */

