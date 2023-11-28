#ifndef Py_BOOLOBJECT_H
#define Py_BOOLOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

extern PyTypeObject PyBool_Type;

#define PyBool_Check(x) (Py_TYPE(x) == &PyBool_Type)

/* Py_False and Py_True are the only two bools in existence.
Don't forget to apply Py_INCREF() when returning either!!! */

/* Don't use these directly */
extern struct _longobject _Py_FalseStruct, _Py_TrueStruct;

/* Use these macros */
#define Py_False ((PyObject *)&_Py_FalseStruct)
#define Py_True  ((PyObject *)&_Py_TrueStruct)

/* Macros for returning Py_True or Py_False, respectively */
#define Py_RETURN_TRUE  return Py_INCREF(Py_True), Py_True
#define Py_RETURN_FALSE return Py_INCREF(Py_False), Py_False

/* Function to return a bool from a C long */
PyObject * PyBool_FromLong(long);

COSMOPOLITAN_C_END_
#endif /* !Py_BOOLOBJECT_H */
