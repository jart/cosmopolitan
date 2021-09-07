/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"
#include "libc/unicode/locale.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/funcobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"
/* clang-format off */

#define _L(x) L##x
#define L(x) _L(x)

PYTHON_YOINK(LAUNCH);

PYTHON_YOINK("_bootlocale");
PYTHON_YOINK("_locale");
PYTHON_YOINK("encodings.aliases");
PYTHON_YOINK("encodings.latin_1");
PYTHON_YOINK("encodings.utf_8");
PYTHON_YOINK("launchpy");

const struct _frozen *PyImport_FrozenModules = _PyImport_FrozenModules;
struct _inittab *PyImport_Inittab = _PyImport_Inittab;

static int LaunchModule(wchar_t *modname)
{
    PyObject *module, *runpy, *runmodule, *runargs, *result;
    runpy = PyImport_ImportModule("launchpy");
    if (runpy == NULL) {
        fprintf(stderr, "Could not import launchpy module\n");
        PyErr_Print();
        return -1;
    }
    runmodule = PyObject_GetAttrString(runpy, "run_module_as_main");
    if (runmodule == NULL) {
        fprintf(stderr, "Could not access launchpy.run_module_as_main\n");
        PyErr_Print();
        Py_DECREF(runpy);
        return -1;
    }
    module = PyUnicode_FromWideChar(modname, wcslen(modname));
    if (module == NULL) {
        fprintf(stderr, "Could not convert module name to unicode\n");
        PyErr_Print();
        Py_DECREF(runpy);
        Py_DECREF(runmodule);
        return -1;
    }
    runargs = Py_BuildValue("(O)", module);
    if (runargs == NULL) {
        fprintf(stderr,
            "Could not create arguments for runpy._run_module_as_main\n");
        PyErr_Print();
        Py_DECREF(runpy);
        Py_DECREF(runmodule);
        Py_DECREF(module);
        return -1;
    }
    result = PyObject_Call(runmodule, runargs, NULL);
    if (result == NULL) {
        PyErr_Print();
    }
    Py_DECREF(runpy);
    Py_DECREF(runmodule);
    Py_DECREF(module);
    Py_DECREF(runargs);
    if (result == NULL) {
        return -1;
    }
    Py_DECREF(result);
    return 0;
}

int
main(int argc, char **argv)
{
    int i, res;
    char *oldloc;
    wchar_t **argv_copy;
    wchar_t **argv_copy2;
    _PyMem_SetupAllocators("malloc");
    argv_copy = gc(malloc(sizeof(wchar_t*) * (argc+1)));
    argv_copy2 = gc(malloc(sizeof(wchar_t*) * (argc+1)));
    oldloc = _PyMem_RawStrdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "");
    for (i = 0; i < argc; i++) {
        argv_copy2[i] = argv_copy[i] = gc(utf8toutf32(argv[i], -1, 0));
    }
    argv_copy2[argc] = argv_copy[argc] = NULL;
    setlocale(LC_ALL, oldloc);
    PyMem_RawFree(oldloc);
    _PyRandom_Init();
    Py_FrozenFlag++;
    Py_NoSiteFlag++;
    /* Py_VerboseFlag++; */
    Py_NoUserSiteDirectory++;
    Py_IgnoreEnvironmentFlag++;
    Py_DontWriteBytecodeFlag++;
    Py_Initialize();
    Py_LimitedPath();
    PySys_SetArgvEx(argc, argv_copy, 0);
    res = LaunchModule(L(LAUNCH)) != 0;
    _PyMem_SetupAllocators("malloc");
    return res;
}
