#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_EZPRINT_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_EZPRINT_H_
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/unicodeobject.h"
COSMOPOLITAN_C_START_

static void EzPrint(PyObject *x, const char *s) {
  PyObject *u, *r, *t;
  kprintf("%s = ", s);
  if (!s) {
    kprintf("NULL");
  } else {
    t = PyObject_Type(x);
    r = PyObject_Repr(t);
    u = PyUnicode_AsUTF8String(r);
    kprintf("%.*s ", PyBytes_GET_SIZE(u), PyBytes_AS_STRING(u));
    Py_DECREF(u);
    Py_DECREF(r);
    Py_DECREF(t);
    r = PyObject_Repr(x);
    u = PyUnicode_AsUTF8String(r);
    kprintf("%.*s", PyBytes_GET_SIZE(u), PyBytes_AS_STRING(u));
    Py_DECREF(u);
    Py_DECREF(r);
  }
  kprintf("\n");
}

#define EZPRINT(x) EzPrint(x, #x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_INCLUDE_EZPRINT_H_ */
