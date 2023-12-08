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

PYTHON_PROVIDE("xterm");
PYTHON_PROVIDE("xterm.rgb2xterm256");

PyDoc_STRVAR(xterm_doc, "Xterm Module\n\
\n\
This module exposes low-level utilities for xterm ansi encoding.");

PyDoc_STRVAR(rgb2xterm256_doc,
"rgb2xterm256($module, r, g, b)\n\
--\n\n\
Quantizes RGB to color to XTERM256 ANSI terminal code.\n\
\n\
This helps you print colors in the terminal faster. For example:\n\
\n\
    print(\"\\x1b[38;5;%dmhello\\x1b[0m\" % (xterm.rgb2xterm256(255,0,0)))\n\
\n\
Will print red text to the terminal.");

static PyObject *
xterm_rgb2xterm256(PyObject *self, PyObject *args)
{
    unsigned char r, g, b;
    int res, cerr, gerr, ir, ig, ib, gray, grai, cr, cg, cb, gv;
    const unsigned char kXtermCube[6] = {0, 0137, 0207, 0257, 0327, 0377};
    if (!PyArg_ParseTuple(args, "BBB:rgb2xterm256", &r, &g, &b)) return 0;
    gray = round(r * .299 + g * .587 + b * .114);
    grai = gray > 238 ? 23 : (gray - 3) / 10;
    ir = r < 48 ? 0 : r < 115 ? 1 : (r - 35) / 40;
    ig = g < 48 ? 0 : g < 115 ? 1 : (g - 35) / 40;
    ib = b < 48 ? 0 : b < 115 ? 1 : (b - 35) / 40;
    cr = kXtermCube[ir];
    cg = kXtermCube[ig];
    cb = kXtermCube[ib];
    gv = 8 + 10 * grai;
    cerr = (cr-r)*(cr-r) + (cg-g)*(cg-g) + (cb-b)*(cb-b);
    gerr = (gv-r)*(gv-r) + (gv-g)*(gv-g) + (gv-b)*(gv-b);
    if (cerr <= gerr) {
        res = 16 + 36 * ir + 6 * ig + ib;
    } else {
        res = 232 + grai;
    }
    return PyLong_FromUnsignedLong(res);
}

static PyMethodDef xterm_methods[] = {
    {"rgb2xterm256", xterm_rgb2xterm256, METH_VARARGS, rgb2xterm256_doc},
    {0},
};

static struct PyModuleDef xtermmodule = {
    PyModuleDef_HEAD_INIT,
    "xterm",
    xterm_doc,
    -1,
    xterm_methods,
};

PyMODINIT_FUNC
PyInit_xterm(void)
{
    return PyModule_Create(&xtermmodule);
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_xterm = {
    "xterm",
    PyInit_xterm,
};
