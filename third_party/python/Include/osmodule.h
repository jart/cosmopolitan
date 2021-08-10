#ifndef Py_OSMODULE_H
#define Py_OSMODULE_H
COSMOPOLITAN_C_START_
/* clang-format off */

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
PyAPI_FUNC(PyObject *) PyOS_FSPath(PyObject *path);
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_OSMODULE_H */
