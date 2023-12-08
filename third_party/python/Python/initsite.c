/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/yoink.h"

/* PYTHON_YOINK("site"); */
/* PYTHON_YOINK("_sysconfigdata_m_cosmo_x86_64_cosmo"); */

void
_Py_InitSite(void)
{
    PyObject *m;
    m = PyImport_ImportModule("site");
    if (m == NULL) {
        fputs("Failed to import the site module\n", stderr);
        PyErr_Print();
        Py_Finalize();
        exit(1);
    }
    else {
        Py_DECREF(m);
    }
}
