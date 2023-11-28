#ifndef NAMESPACEOBJECT_H
#define NAMESPACEOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
extern PyTypeObject _PyNamespace_Type;

PyObject * _PyNamespace_New(PyObject *kwds);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !NAMESPACEOBJECT_H */
