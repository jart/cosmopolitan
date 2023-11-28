#ifndef Py_MARSHAL_H
#define Py_MARSHAL_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#define Py_MARSHAL_VERSION 4

void PyMarshal_WriteLongToFile(long, FILE *, int);
void PyMarshal_WriteObjectToFile(PyObject *, FILE *, int);
PyObject * PyMarshal_WriteObjectToString(PyObject *, int);

#ifndef Py_LIMITED_API
long PyMarshal_ReadLongFromFile(FILE *);
int PyMarshal_ReadShortFromFile(FILE *);
PyObject * PyMarshal_ReadObjectFromFile(FILE *);
PyObject * PyMarshal_ReadLastObjectFromFile(FILE *);
#endif
PyObject * PyMarshal_ReadObjectFromString(const char *, Py_ssize_t);

COSMOPOLITAN_C_END_
#endif /* !Py_MARSHAL_H */
