/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pylifecycle.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/locale.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/python/Include/Python-ast.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/ast.h"
#include "third_party/python/Include/boolobject.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/code.h"
#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/cosmo.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/errcode.h"
#include "third_party/python/Include/fileobject.h"
#include "third_party/python/Include/grammar.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/intrcheck.h"
#include "third_party/python/Include/marshal.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/node.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/osdefs.h"
#include "third_party/python/Include/parsetok.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pystrcmp.h"
#include "third_party/python/Include/pytime.h"
#include "third_party/python/Include/symtable.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/token.h"
#include "third_party/python/Include/traceback.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/warnings.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/pyconfig.h"

/* Python interpreter top-level routines, including init/exit */

_Py_IDENTIFIER(stderr);

/* Forward */
void wait_for_thread_shutdown(void);
#ifdef WITH_THREAD
extern void _PyGILState_Init(PyInterpreterState *, PyThreadState *);
extern void _PyGILState_Fini(void);
#endif /* WITH_THREAD */

/* Hack to force loading of object files */
int (*_PyOS_mystrnicmp_hack)(const char *, const char *, Py_ssize_t) = \
    PyOS_mystrnicmp; /* Python/pystrcmp.o */

/* Helper to allow an embedding application to override the normal
 * mechanism that attempts to figure out an appropriate IO encoding
 */

int
Py_SetStandardStreamEncoding(const char *encoding, const char *errors)
{
    if (Py_IsInitialized()) {
        /* This is too late to have any effect */
        return -1;
    }
    /* Can't call PyErr_NoMemory() on errors, as Python hasn't been
     * initialised yet.
     *
     * However, the raw memory allocators are initialised appropriately
     * as C static variables, so _PyMem_RawStrdup is OK even though
     * Py_Initialize hasn't been called yet.
     */
    if (encoding) {
        _Py_StandardStreamEncoding = _PyMem_RawStrdup(encoding);
        if (!_Py_StandardStreamEncoding) {
            return -2;
        }
    }
    if (errors) {
        _Py_StandardStreamErrors = _PyMem_RawStrdup(errors);
        if (!_Py_StandardStreamErrors) {
            if (_Py_StandardStreamEncoding) {
                PyMem_RawFree(_Py_StandardStreamEncoding);
            }
            return -3;
        }
    }
#ifdef MS_WINDOWS
    if (_Py_StandardStreamEncoding) {
        /* Overriding the stream encoding implies legacy streams */
        Py_LegacyWindowsStdioFlag = 1;
    }
#endif
    return 0;
}

/* Global initializations.  Can be undone by Py_FinalizeEx().  Don't
   call this twice without an intervening Py_FinalizeEx() call.  When
   initializations fail, a fatal error is issued and the function does
   not return.  On return, the first thread and interpreter state have
   been created.

   Locking: you must hold the interpreter lock while calling this.
   (If the lock has not yet been initialized, that's equivalent to
   having the lock, but you cannot use multiple threads.)

*/

static int
add_flag(int flag, const char *envs)
{
    int env = atoi(envs);
    if (flag < env)
        flag = env;
    if (flag < 1)
        flag = 1;
    return flag;
}

void
_Py_InitializeEx_Private(int install_sigs, int install_importlib)
{
    PyInterpreterState *interp;
    PyThreadState *tstate;
    PyObject *bimod, *sysmod, *pstderr;
    char *p;

    if (_Py_initialized)
        return;
    _Py_initialized = 1;
    _Py_Finalizing = NULL;

#ifdef HAVE_SETLOCALE
    /* Set up the LC_CTYPE locale, so we can obtain
       the locale's charset without having to switch
       locales. */
    setlocale(LC_CTYPE, "");
#endif

    if ((p = Py_GETENV("PYTHONDEBUG")) && *p != '\0')
        Py_DebugFlag = add_flag(Py_DebugFlag, p);
    if ((p = Py_GETENV("PYTHONVERBOSE")) && *p != '\0')
        Py_VerboseFlag = add_flag(Py_VerboseFlag, p);
    if ((p = Py_GETENV("PYTHONOPTIMIZE")) && *p != '\0')
        Py_OptimizeFlag = add_flag(Py_OptimizeFlag, p);
    if ((p = Py_GETENV("PYTHONDONTWRITEBYTECODE")) && *p != '\0')
        Py_DontWriteBytecodeFlag = add_flag(Py_DontWriteBytecodeFlag, p);
#ifdef MS_WINDOWS
    if ((p = Py_GETENV("PYTHONLEGACYWINDOWSFSENCODING")) && *p != '\0')
        Py_LegacyWindowsFSEncodingFlag = add_flag(Py_LegacyWindowsFSEncodingFlag, p);
    if ((p = Py_GETENV("PYTHONLEGACYWINDOWSSTDIO")) && *p != '\0')
        Py_LegacyWindowsStdioFlag = add_flag(Py_LegacyWindowsStdioFlag, p);
#endif

    _PyRandom_Init();

    interp = PyInterpreterState_New();
    if (interp == NULL)
        Py_FatalError("Py_Initialize: can't make first interpreter");

    tstate = PyThreadState_New(interp);
    if (tstate == NULL)
        Py_FatalError("Py_Initialize: can't make first thread");
    (void) PyThreadState_Swap(tstate);

#ifdef WITH_THREAD
    /* We can't call _PyEval_FiniThreads() in Py_FinalizeEx because
       destroying the GIL might fail when it is being referenced from
       another running thread (see issue #9901).
       Instead we destroy the previously created GIL here, which ensures
       that we can call Py_Initialize / Py_FinalizeEx multiple times. */
    _PyEval_FiniThreads();

    /* Auto-thread-state API */
    _PyGILState_Init(interp, tstate);
#endif /* WITH_THREAD */

    _Py_ReadyTypes();

    if (!_PyFrame_Init())
        Py_FatalError("Py_Initialize: can't init frames");

    if (!_PyLong_Init())
        Py_FatalError("Py_Initialize: can't init longs");

    if (!PyByteArray_Init())
        Py_FatalError("Py_Initialize: can't init bytearray");

    if (!_PyFloat_Init())
        Py_FatalError("Py_Initialize: can't init float");

    interp->modules = PyDict_New();
    if (interp->modules == NULL)
        Py_FatalError("Py_Initialize: can't make modules dictionary");

    /* Init Unicode implementation; relies on the codec registry */
    if (_PyUnicode_Init() < 0)
        Py_FatalError("Py_Initialize: can't initialize unicode");
    if (_PyStructSequence_Init() < 0)
        Py_FatalError("Py_Initialize: can't initialize structseq");

    bimod = _PyBuiltin_Init();
    if (bimod == NULL)
        Py_FatalError("Py_Initialize: can't initialize builtins modules");
    _PyImport_FixupBuiltin(bimod, "builtins");
    interp->builtins = PyModule_GetDict(bimod);
    if (interp->builtins == NULL)
        Py_FatalError("Py_Initialize: can't initialize builtins dict");
    Py_INCREF(interp->builtins);

    /* initialize builtin exceptions */
    _PyExc_Init(bimod);

    sysmod = _PySys_Init();
    if (sysmod == NULL)
        Py_FatalError("Py_Initialize: can't initialize sys");
    interp->sysdict = PyModule_GetDict(sysmod);
    if (interp->sysdict == NULL)
        Py_FatalError("Py_Initialize: can't initialize sys dict");
    Py_INCREF(interp->sysdict);
    _PyImport_FixupBuiltin(sysmod, "sys");
    PySys_SetPath(Py_GetPath());
    PyDict_SetItemString(interp->sysdict, "modules",
                         interp->modules);

    /* Set up a preliminary stderr printer until we have enough
       infrastructure for the io module in place. */
    pstderr = PyFile_NewStdPrinter(fileno(stderr));
    if (pstderr == NULL)
        Py_FatalError("Py_Initialize: can't set preliminary stderr");
    _PySys_SetObjectId(&PyId_stderr, pstderr);
    PySys_SetObject("__stderr__", pstderr);
    Py_DECREF(pstderr);

    _PyImportLookupTables_Init();
    _PyImport_Init();

    _PyImportHooks_Init();

    /* Initialize _warnings. */
    _PyWarnings_Init();

    if (!install_importlib)
        return;

    if (_PyTime_Init() < 0)
        Py_FatalError("Py_Initialize: can't initialize time");

    _Py_InitImport(interp, sysmod);

    /* initialize the faulthandler module */
    if (_PyFaulthandler_Init())
        Py_FatalError("Py_Initialize: can't initialize faulthandler");

    if (_Py_InitFsEncoding(interp) < 0)
        Py_FatalError("Py_Initialize: unable to load the file system codec");

    if (install_sigs)
        _Py_InitSigs(); /* Signal handling stuff, including initintr() */

#if IsModeDbg()
    if (_PyTraceMalloc_Init() < 0)
        Py_FatalError("Py_Initialize: can't initialize tracemalloc");
#endif

    _Py_InitMain(interp); /* Module __main__ */
    if (_Py_InitStdio() < 0) {
        Py_FatalError(
            "Py_Initialize: can't initialize sys standard streams");
    }

    /* Initialize warnings. */
    if (PySys_HasWarnOptions()) {
        PyObject *warnings_module = PyImport_ImportModule("warnings");
        if (warnings_module == NULL) {
            fputs("'import warnings' failed; traceback:\n", stderr);
            PyErr_Print();
        }
        Py_XDECREF(warnings_module);
    }

    _PyImportZip_Init();

    if (!Py_NoSiteFlag)
        _Py_InitSite(); /* Module site */
}

void
Py_InitializeEx(int install_sigs)
{
    _Py_InitializeEx_Private(install_sigs, 1);
}

void
Py_Initialize(void)
{
    Py_InitializeEx(1);
}


#ifdef COUNT_ALLOCS
extern void dump_counts(FILE*);
#endif

/* Create and initialize a new interpreter and thread, and return the
   new thread.  This requires that Py_Initialize() has been called
   first.

   Unsuccessful initialization yields a NULL pointer.  Note that *no*
   exception information is available even in this case -- the
   exception information is held in the thread, and there is no
   thread.

   Locking: as above.

*/

PyThreadState *
Py_NewInterpreter(void)
{
    PyInterpreterState *interp;
    PyThreadState *tstate, *save_tstate;
    PyObject *bimod, *sysmod;

    if (!_Py_initialized)
        Py_FatalError("Py_NewInterpreter: call Py_Initialize first");

#ifdef WITH_THREAD
    /* Issue #10915, #15751: The GIL API doesn't work with multiple
       interpreters: disable PyGILState_Check(). */
    _PyGILState_check_enabled = 0;
#endif

    interp = PyInterpreterState_New();
    if (interp == NULL)
        return NULL;

    tstate = PyThreadState_New(interp);
    if (tstate == NULL) {
        PyInterpreterState_Delete(interp);
        return NULL;
    }

    save_tstate = PyThreadState_Swap(tstate);

    /* XXX The following is lax in error checking */

    interp->modules = PyDict_New();

    bimod = _PyImport_FindBuiltin("builtins");
    if (bimod != NULL) {
        interp->builtins = PyModule_GetDict(bimod);
        if (interp->builtins == NULL)
            goto handle_error;
        Py_INCREF(interp->builtins);
    }
    else if (PyErr_Occurred()) {
        goto handle_error;
    }

    /* initialize builtin exceptions */
    _PyExc_Init(bimod);

    sysmod = _PyImport_FindBuiltin("sys");
    if (bimod != NULL && sysmod != NULL) {
        PyObject *pstderr;

        interp->sysdict = PyModule_GetDict(sysmod);
        if (interp->sysdict == NULL)
            goto handle_error;
        Py_INCREF(interp->sysdict);
        PySys_SetPath(Py_GetPath());
        PyDict_SetItemString(interp->sysdict, "modules",
                             interp->modules);
        /* Set up a preliminary stderr printer until we have enough
           infrastructure for the io module in place. */
        pstderr = PyFile_NewStdPrinter(fileno(stderr));
        if (pstderr == NULL)
            Py_FatalError("Py_Initialize: can't set preliminary stderr");
        _PySys_SetObjectId(&PyId_stderr, pstderr);
        PySys_SetObject("__stderr__", pstderr);
        Py_DECREF(pstderr);

        _PyImportHooks_Init();

        _Py_InitImport(interp, sysmod);

        if (_Py_InitFsEncoding(interp) < 0)
            goto handle_error;

        if (_Py_InitStdio() < 0)
            Py_FatalError(
                "Py_Initialize: can't initialize sys standard streams");
        _Py_InitMain(interp);
        if (!Py_NoSiteFlag)
            _Py_InitSite();
    }

    if (!PyErr_Occurred())
        return tstate;

handle_error:
    /* Oops, it didn't work.  Undo it all. */

    PyErr_PrintEx(0);
    PyThreadState_Clear(tstate);
    PyThreadState_Swap(save_tstate);
    PyThreadState_Delete(tstate);
    PyInterpreterState_Delete(interp);

    return NULL;
}

/* Delete an interpreter and its last thread.  This requires that the
   given thread state is current, that the thread has no remaining
   frames, and that it is its interpreter's only remaining thread.
   It is a fatal error to violate these constraints.

   (Py_FinalizeEx() doesn't have these constraints -- it zaps
   everything, regardless.)

   Locking: as above.

*/

void
Py_EndInterpreter(PyThreadState *tstate)
{
    PyInterpreterState *interp = tstate->interp;

    if (tstate != PyThreadState_GET())
        Py_FatalError("Py_EndInterpreter: thread is not current");
    if (tstate->frame != NULL)
        Py_FatalError("Py_EndInterpreter: thread still has a frame");

    wait_for_thread_shutdown();

    if (tstate != interp->tstate_head || tstate->next != NULL)
        Py_FatalError("Py_EndInterpreter: not the last thread");

    PyImport_Cleanup();
    PyInterpreterState_Clear(interp);
    PyThreadState_Swap(NULL);
    PyInterpreterState_Delete(interp);
}

/* Clean up and exit */

#ifdef WITH_THREAD
#include "third_party/python/Include/pythread.h"
#endif

/* Wait until threading._shutdown completes, provided
   the threading module was imported in the first place.
   The shutdown routine will wait until all non-daemon
   "threading" threads have completed. */
void
wait_for_thread_shutdown(void)
{
#ifdef WITH_THREAD
    _Py_IDENTIFIER(_shutdown);
    PyObject *result;
    PyThreadState *tstate = PyThreadState_GET();
    PyObject *threading = PyMapping_GetItemString(tstate->interp->modules,
                                                  "threading");
    if (threading == NULL) {
        /* threading not imported */
        PyErr_Clear();
        return;
    }
    result = _PyObject_CallMethodId(threading, &PyId__shutdown, NULL);
    if (result == NULL) {
        PyErr_WriteUnraisable(threading);
    }
    else {
        Py_DECREF(result);
    }
    Py_DECREF(threading);
#endif
}

void
Py_Exit(int sts)
{
    if (Py_FinalizeEx() < 0) {
        sts = 120;
    }
    exit(sts);
}
