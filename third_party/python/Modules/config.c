/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/object.h"

PyObject *PyInit_posix(void);
PyObject *PyInit__codecs(void);
PyObject *PyInit_itertools(void);
PyObject *PyInit__io(void);
PyObject *PyInit__weakref(void);
PyObject *PyMarshal_Init(void);
PyObject *PyInit__ast(void);
PyObject *PyInit_gc(void);
PyObject *_PyWarnings_Init(void);
PyObject *PyInit__string(void);
PyObject *PyInit__thread(void);

_Alignas(16) _Section(".rodata.pytab.0") const struct _inittab _PyImport_Inittab[0];
_Alignas(16) _Section(".rodata.pytab.2") const struct _inittab _PyImport_Inittab2[] = {
    {"posix", PyInit_posix},
    {"_codecs", PyInit__codecs},
    {"itertools", PyInit_itertools},
    {"_io", PyInit__io},
    {"_thread", PyInit__thread},
    {"_weakref", PyInit__weakref},
    {"marshal", PyMarshal_Init},
    {"_imp", PyInit_imp},
    {"_ast", PyInit__ast},
    {"builtins", NULL},
    {"sys", NULL},
    {"gc", PyInit_gc},
    {"_warnings", _PyWarnings_Init},
    {"_string", PyInit__string},
    {0}
};
