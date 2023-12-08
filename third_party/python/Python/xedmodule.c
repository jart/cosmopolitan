/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/xed/x86.h"

PYTHON_PROVIDE("xed");
PYTHON_PROVIDE("xed.ild");

PyDoc_STRVAR(xed_doc, "Xed Module\n\
\n\
This module exposes low-level utilities for x86 encoding.");

PyDoc_STRVAR(ild_doc,
"ild($module, bytes)\n\
--\n\n\
Decodes byte-length of first machine instruction in byte sequence.\n\
\n\
This function makes it possible to tokenize raw x86 binary instructions.\n\
Return value is negative on error, where -1 is defined as buffer being\n\
too short, and lower numbers represent other errors.");

static PyObject *
xed_ild(PyObject *self, PyObject *args)
{
    int e;
    Py_ssize_t n;
    const char *p;
    struct XedDecodedInst xedd;
    if (!PyArg_ParseTuple(args, "y#:ild", &p, &n)) return 0;
    xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
    e = xed_instruction_length_decode(&xedd, p, n);
    return PyLong_FromUnsignedLong(e ? -e : xedd.length);
}

static PyMethodDef xed_methods[] = {
    {"ild", xed_ild, METH_VARARGS, ild_doc},
    {0},
};

static struct PyModuleDef xedmodule = {
    PyModuleDef_HEAD_INIT,
    "xed",
    xed_doc,
    -1,
    xed_methods,
};

PyMODINIT_FUNC
PyInit_xed(void)
{
    return PyModule_Create(&xedmodule);
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_xed = {
    "xed",
    PyInit_xed,
};
