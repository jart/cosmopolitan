#ifndef Py_STRHEX_H
#define Py_STRHEX_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
/* Returns a str() containing the hex representation of argbuf. */
PyObject* _Py_strhex(const char* argbuf, const Py_ssize_t arglen);
/* Returns a bytes() containing the ASCII hex representation of argbuf. */
PyObject* _Py_strhex_bytes(const char* argbuf, const Py_ssize_t arglen);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_STRHEX_H */
