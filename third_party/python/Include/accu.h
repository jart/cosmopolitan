#ifndef Py_LIMITED_API
#ifndef Py_ACCU_H
#define Py_ACCU_H
COSMOPOLITAN_C_START_
/* clang-format off */

#undef small /* defined by some Windows headers */

typedef struct {
    PyObject *large;  /* A list of previously accumulated large strings */
    PyObject *small;  /* Pending small strings */
} _PyAccu;

PyAPI_FUNC(int) _PyAccu_Init(_PyAccu *acc);
PyAPI_FUNC(int) _PyAccu_Accumulate(_PyAccu *acc, PyObject *unicode);
PyAPI_FUNC(PyObject *) _PyAccu_FinishAsList(_PyAccu *acc);
PyAPI_FUNC(PyObject *) _PyAccu_Finish(_PyAccu *acc);
PyAPI_FUNC(void) _PyAccu_Destroy(_PyAccu *acc);

COSMOPOLITAN_C_END_
#endif /* Py_ACCU_H */
#endif /* Py_LIMITED_API */
