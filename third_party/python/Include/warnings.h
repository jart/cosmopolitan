#ifndef Py_WARNINGS_H
#define Py_WARNINGS_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
PyObject *_PyWarnings_Init(void);
#endif

int PyErr_WarnEx(PyObject *, const char *, Py_ssize_t);
int PyErr_WarnFormat(PyObject *, Py_ssize_t, const char *, ...);

#if !defined(Py_LIMITED_API) || Py_LIMITED_API + 0 >= 0x03060000
int PyErr_ResourceWarning(PyObject *, Py_ssize_t, const char *, ...);
#endif

#ifndef Py_LIMITED_API
int PyErr_WarnExplicitObject(PyObject *, PyObject *, PyObject *, int,
                             PyObject *, PyObject *);
#endif

int PyErr_WarnExplicit(PyObject *, const char *, const char *, int,
                       const char *, PyObject *);

#ifndef Py_LIMITED_API
int PyErr_WarnExplicitFormat(PyObject *, const char *, int, const char *,
                             PyObject *, const char *, ...);
#endif

#ifndef Py_LIMITED_API
#define PyErr_Warn(category, msg) PyErr_WarnEx(category, msg, 1)
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_WARNINGS_H */
