#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_EZPRINT_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_EZPRINT_H_
#include "libc/calls/calls.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/unicodeobject.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

static void EzPrint(PyObject *x, const char *s) {
  PyObject *u;
  if (!s) {
    dprintf(2, "%s = NULL\n", s);
  } else if (PyBytes_Check(x)) {
    dprintf(2, "%s = b%`'.*s\n", s, PyBytes_GET_SIZE(x), PyBytes_AS_STRING(x));
  } else if ((u = PyUnicode_AsUTF8String(x))) {
    dprintf(2, "%s = u%`'.*s\n", s, PyBytes_GET_SIZE(u), PyBytes_AS_STRING(u));
    Py_DECREF(u);
  } else {
    PyErr_Clear();
    dprintf(2, "%s = !!!\n", s);
  }
}

#define EZPRINT(x) EzPrint(x, #x)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_EZPRINT_H_ */
