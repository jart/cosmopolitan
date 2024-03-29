#ifndef Py_STRTOD_H
#define Py_STRTOD_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

double PyOS_string_to_double(const char *str,
                                         char **endptr,
                                         PyObject *overflow_exception);

/* The caller is responsible for calling PyMem_Free to free the buffer
   that's is returned. */
char * PyOS_double_to_string(double val,
                                         char format_code,
                                         int precision,
                                         int flags,
                                         int *type);

#ifndef Py_LIMITED_API
PyObject * _Py_string_to_number_with_underscores(
    const char *str, Py_ssize_t len, const char *what, PyObject *obj, void *arg,
    PyObject *(*innerfunc)(const char *, Py_ssize_t, void *));

double _Py_parse_inf_or_nan(const char *p, char **endptr);
#endif


/* PyOS_double_to_string's "flags" parameter can be set to 0 or more of: */
#define Py_DTSF_SIGN      0x01 /* always add the sign */
#define Py_DTSF_ADD_DOT_0 0x02 /* if the result is an integer add ".0" */
#define Py_DTSF_ALT       0x04 /* "alternate" formatting. it's format_code
                                  specific */

/* PyOS_double_to_string's "type", if non-NULL, will be set to one of: */
#define Py_DTST_FINITE 0
#define Py_DTST_INFINITE 1
#define Py_DTST_NAN 2

COSMOPOLITAN_C_END_
#endif /* !Py_STRTOD_H */
