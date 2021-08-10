#ifndef NAMESPACEOBJECT_H
#define NAMESPACEOBJECT_H
COSMOPOLITAN_C_START_
/* clang-format off */

#ifndef Py_LIMITED_API
PyAPI_DATA(PyTypeObject) _PyNamespace_Type;

PyAPI_FUNC(PyObject *) _PyNamespace_New(PyObject *kwds);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !NAMESPACEOBJECT_H */
