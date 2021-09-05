/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/Python-ast.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/boolobject.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/code.h"
#include "third_party/python/Include/cosmo.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/errcode.h"
#include "third_party/python/Include/eval.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/frameobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/marshal.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/osdefs.h"
#include "third_party/python/Include/pgenheaders.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/traceback.h"
#include "third_party/python/Include/tupleobject.h"
#include "third_party/python/Include/warnings.h"
#include "third_party/python/Include/weakrefobject.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Python/importdl.h"
/* clang-format off */

PYTHON_PROVIDE("_cosmo");

static int cosmo_constants(PyObject *m)
{
  if(PyModule_AddStringMacro(m, MODE)) return -1;
  return 0;
}


PyDoc_STRVAR(doc_cosmo,
"additional information and special functions provided by Cosmopolitan Libc.");

static PyMethodDef cosmo_methods[] = {
    {NULL, NULL}  /* sentinel */
};


static struct PyModuleDef cosmomodule = {
    PyModuleDef_HEAD_INIT,
    "_cosmo",
    doc_cosmo,
    -1,
    cosmo_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_cosmo(void)
{
    PyObject *m, *d;

    m = PyModule_Create(&cosmomodule);
    if (m == NULL)
        goto failure;

    if(cosmo_constants(m))
        goto failure;

    return m;
  failure:
    Py_XDECREF(m);
    return NULL;
}
