#ifndef Py_MARSHAL_H
#define Py_MARSHAL_H
COSMOPOLITAN_C_START_
/* clang-format off */

#define Py_MARSHAL_VERSION 4

PyAPI_FUNC(void) PyMarshal_WriteLongToFile(long, FILE *, int);
PyAPI_FUNC(void) PyMarshal_WriteObjectToFile(PyObject *, FILE *, int);
PyAPI_FUNC(PyObject *) PyMarshal_WriteObjectToString(PyObject *, int);

#ifndef Py_LIMITED_API
PyAPI_FUNC(long) PyMarshal_ReadLongFromFile(FILE *);
PyAPI_FUNC(int) PyMarshal_ReadShortFromFile(FILE *);
PyAPI_FUNC(PyObject *) PyMarshal_ReadObjectFromFile(FILE *);
PyAPI_FUNC(PyObject *) PyMarshal_ReadLastObjectFromFile(FILE *);
#endif
PyAPI_FUNC(PyObject *) PyMarshal_ReadObjectFromString(const char *, Py_ssize_t);

COSMOPOLITAN_C_END_
#endif /* !Py_MARSHAL_H */
