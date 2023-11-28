#ifndef Py_OSMODULE_H
#define Py_OSMODULE_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
PyObject * PyOS_FSPath(PyObject *path);
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_OSMODULE_H */
