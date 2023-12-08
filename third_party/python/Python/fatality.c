/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/sysmodule.h"
#include "third_party/python/Include/traceback.h"

_Py_IDENTIFIER(flush);
_Py_IDENTIFIER(stderr);

/* Import the site module (not into __main__ though) */

static void
_Py_FatalError_DumpTracebacks(int fd)
{
    fputc('\n', stderr);
    fflush(stderr);
    /* display the current Python stack */
    _Py_DumpTracebackThreads(fd, NULL, NULL);
}

/* Print the current exception (if an exception is set) with its traceback,
   or display the current Python stack.

   Don't call PyErr_PrintEx() and the except hook, because Py_FatalError() is
   called on catastrophic cases.

   Return 1 if the traceback was displayed, 0 otherwise. */

static int
_Py_FatalError_PrintExc(int fd)
{
    PyObject *ferr, *res;
    PyObject *exception, *v, *tb;
    int has_tb;

    PyErr_Fetch(&exception, &v, &tb);
    if (exception == NULL) {
        /* No current exception */
        return 0;
    }

    ferr = _PySys_GetObjectId(&PyId_stderr);
    if (ferr == NULL || ferr == Py_None) {
        /* sys.stderr is not set yet or set to None,
           no need to try to display the exception */
        return 0;
    }

    PyErr_NormalizeException(&exception, &v, &tb);
    if (tb == NULL) {
        tb = Py_None;
        Py_INCREF(tb);
    }
    PyException_SetTraceback(v, tb);
    if (exception == NULL) {
        /* PyErr_NormalizeException() failed */
        return 0;
    }

    has_tb = (tb != Py_None);
    PyErr_Display(exception, v, tb);
    Py_XDECREF(exception);
    Py_XDECREF(v);
    Py_XDECREF(tb);

    /* sys.stderr may be buffered: call sys.stderr.flush() */
    res = _PyObject_CallMethodId(ferr, &PyId_flush, NULL);
    if (res == NULL)
        PyErr_Clear();
    else
        Py_DECREF(res);

    return has_tb;
}

/* Print fatal error message and abort */

void
Py_FatalError(const char *msg)
{
    const int fd = fileno(stderr);
    static int reentrant = 0;
#ifdef MS_WINDOWS
    size_t len;
    WCHAR* buffer;
    size_t i;
#endif

    if (reentrant) {
        /* Py_FatalError() caused a second fatal error.
           Example: _Py_FlushStdFiles() raises a recursion error. */
        goto exit;
    }
    reentrant = 1;

    fprintf(stderr, "Fatal Python error: %s\n", msg);
    fflush(stderr); /* it helps in Windows debug build */

    /* Check if the current thread has a Python thread state
       and holds the GIL */
    PyThreadState *tss_tstate = NULL; // PyGILState_GetThisThreadState();
    if (tss_tstate != NULL) {
        PyThreadState *tstate = PyThreadState_GET();
        if (tss_tstate != tstate) {
            /* The Python thread does not hold the GIL */
            tss_tstate = NULL;
        }
    }
    else {
        /* Py_FatalError() has been called from a C thread
           which has no Python thread state. */
    }
    int has_tstate_and_gil = (tss_tstate != NULL);

    /* If an exception is set, print the exception with its traceback */
    if (!_Py_FatalError_PrintExc(fd)) {
        /* No exception is set, or an exception is set without traceback */
        _Py_FatalError_DumpTracebacks(fd);
    }

    /* The main purpose of faulthandler is to display the traceback. We already
     * did our best to display it. So faulthandler can now be disabled.
     * (Don't trigger it on abort().) */
    _PyFaulthandler_Fini();

    /* Check if the current Python thread hold the GIL */
    if (has_tstate_and_gil) {
        /* Flush sys.stdout and sys.stderr */
        _Py_FlushStdFiles();
    }

#ifdef MS_WINDOWS
    len = strlen(msg);

    /* Convert the message to wchar_t. This uses a simple one-to-one
    conversion, assuming that the this error message actually uses ASCII
    only. If this ceases to be true, we will have to convert. */
    buffer = alloca( (len+1) * (sizeof *buffer));
    for( i=0; i<=len; ++i)
        buffer[i] = msg[i];
    OutputDebugStringW(L"Fatal Python error: ");
    OutputDebugStringW(buffer);
    OutputDebugStringW(L"\n");
#endif /* MS_WINDOWS */

exit:
#if defined(MS_WINDOWS) && defined(_DEBUG)
    DebugBreak();
#endif
    if (_weaken(__die)) _weaken(__die)();
    abort();
}
