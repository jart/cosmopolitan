/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/str/locale.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/boolobject.h"
#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/cosmo.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pystate.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("_frozen_importlib");
PYTHON_PROVIDE("_frozen_importlib_external");

void
_Py_InitImport(PyInterpreterState *interp, PyObject *sysmod)
{
    PyObject *importlib;
    PyObject *impmod;
    PyObject *sys_modules;
    PyObject *value;

    /* Import _importlib through its frozen version, _frozen_importlib. */
    if (PyImport_ImportFrozenModule("_frozen_importlib") <= 0) {
        Py_FatalError("Py_Initialize: can't import _frozen_importlib");
    }
    else if (Py_VerboseFlag) {
        PySys_FormatStderr("import _frozen_importlib # frozen\n");
    }
    importlib = PyImport_AddModule("_frozen_importlib");
    if (importlib == NULL) {
        Py_FatalError("Py_Initialize: couldn't get _frozen_importlib from "
                      "sys.modules");
    }
    interp->importlib = importlib;
    Py_INCREF(interp->importlib);

    interp->import_func = PyDict_GetItemString(interp->builtins, "__import__");
    if (interp->import_func == NULL)
        Py_FatalError("Py_Initialize: __import__ not found");
    Py_INCREF(interp->import_func);

    /* Import the _imp module */
    impmod = PyInit_imp();
    if (impmod == NULL) {
        Py_FatalError("Py_Initialize: can't import _imp");
    }
    else if (Py_VerboseFlag) {
        PySys_FormatStderr("import _imp # builtin\n");
    }
    sys_modules = PyImport_GetModuleDict();
    if (Py_VerboseFlag) {
        PySys_FormatStderr("import sys # builtin\n");
    }
    if (PyDict_SetItemString(sys_modules, "_imp", impmod) < 0) {
        Py_FatalError("Py_Initialize: can't save _imp to sys.modules");
    }

    /* Install importlib as the implementation of import */
    value = PyObject_CallMethod(importlib, "_install", "OO", sysmod, impmod);
    if (value == NULL) {
        PyErr_Print();
        Py_FatalError("Py_Initialize: importlib install failed");
    }
    Py_DECREF(value);
    Py_DECREF(impmod); 
}
