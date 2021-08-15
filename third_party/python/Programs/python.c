/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"
#include "libc/unicode/locale.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/unicodeobject.h"
/* clang-format off */

static jmp_buf jbuf;

static void
OnKeyboardInterrupt(int sig)
{
    longjmp(jbuf, 1);
}

static PyObject *
GetMember(const char *s, Py_ssize_t n, PyObject *o)
{
    const char *t;
    PyObject *k, *v;
    Py_ssize_t i, m;
    if (!o) return 0;
    for (i = 0; PyDict_Next(o, &i, &k, &v);) {
        if (v != Py_None && PyUnicode_Check(k)) {
            t = PyUnicode_AsUTF8AndSize(k, &m);
            printf("\r%`'.*s vs. %`'.*s\n", n, s, m, t);
            if (n == m && !memcmp(s, t, n)) {
                Py_INCREF(v);
                return v;
            }
        }
    }
    return 0;
}

static PyObject *
GetVar(const char *s, Py_ssize_t n)
{
    PyObject *o;
    /*
     * TODO: Why doesn't PyEval_GetLocals() work?
     */
    if ((o = GetMember(s, n, PyEval_GetLocals()))) return o;
    if ((o = GetMember(s, n, PyEval_GetGlobals()))) return o;
    if ((o = GetMember(s, n, PyEval_GetBuiltins()))) return o;
    return 0;
}

static void
TerminalComplete(const char *s, linenoiseCompletions *c, PyObject *o)
{
    const char *t;
    PyObject *k, *v;
    Py_ssize_t i, n, m;
    if (!o) return;
    for (n = strlen(s), i = 0; PyDict_Next(o, &i, &k, &v);) {
        if (v != Py_None && PyUnicode_Check(k)) {
            t = PyUnicode_AsUTF8AndSize(k, &m);
            if (m > n && !memcmp(t, s, n)) {
                c->cvec = realloc(c->cvec, ++c->len * sizeof(*c->cvec));
                c->cvec[c->len - 1] = strdup(t);
            }
        }
    }
}

static void
TerminalCompletion(const char *s, linenoiseCompletions *c)
{
    const char *p;
    PyObject *o, *q;
    if ((p = strchr(s, '.'))) {
        if (!(o = GetVar(s, p - s))) return;
        for (s = p + 1; (p = strchr(s, '.')); o = q) {
            if ((q = GetMember(s, p - s, o))) return;
            Py_DECREF(o);
        }
        TerminalComplete(s, c, o);
        Py_DECREF(o);
    } else {
        TerminalComplete(s, c, PyEval_GetLocals());
        TerminalComplete(s, c, PyEval_GetGlobals());
        TerminalComplete(s, c, PyEval_GetBuiltins());
    }
}

char *
TerminalReadline(FILE *sys_stdin, FILE *sys_stdout, const char *prompt)
{
    size_t n;
    char *p, *q;
    PyOS_sighandler_t saint;
    saint = PyOS_setsig(SIGINT, OnKeyboardInterrupt);
    if (setjmp(jbuf)) {
        linenoiseDisableRawMode(STDIN_FILENO);
        PyOS_setsig(SIGINT, saint);
        return NULL;
    }
    p = linenoise(prompt);
    PyOS_setsig(SIGINT, saint);
    if (p) {
        if (*p) linenoiseHistoryAdd(p);
        n = strlen(p);
        q = PyMem_RawMalloc(n + 2);
        strcpy(mempcpy(q, p, n), "\n");
        free(p);
        clearerr(sys_stdin);
        return q;
    } else {
        q = PyMem_RawMalloc(1);
        if (q) *q = 0;
        return q;
    }
}

int
main(int argc, char **argv)
{
    wchar_t **argv_copy;
    /* We need a second copy, as Python might modify the first one. */
    wchar_t **argv_copy2;
    int i, res;
    char *oldloc;

    showcrashreports();
    linenoiseSetCompletionCallback(TerminalCompletion);
    PyOS_ReadlineFunctionPointer = TerminalReadline;

    /* Force malloc() allocator to bootstrap Python */
    (void)_PyMem_SetupAllocators("malloc");

    argv_copy = (wchar_t **)PyMem_RawMalloc(sizeof(wchar_t*) * (argc+1));
    argv_copy2 = (wchar_t **)PyMem_RawMalloc(sizeof(wchar_t*) * (argc+1));
    if (!argv_copy || !argv_copy2) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    /* 754 requires that FP exceptions run in "no stop" mode by default,
     * and until C vendors implement C99's ways to control FP exceptions,
     * Python requires non-stop mode.  Alas, some platforms enable FP
     * exceptions by default.  Here we disable them.
     */
#ifdef __FreeBSD__
    fedisableexcept(FE_OVERFLOW);
#endif

    oldloc = _PyMem_RawStrdup(setlocale(LC_ALL, NULL));
    if (!oldloc) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    setlocale(LC_ALL, "");
    for (i = 0; i < argc; i++) {
        argv_copy[i] = Py_DecodeLocale(argv[i], NULL);
        if (!argv_copy[i]) {
            PyMem_RawFree(oldloc);
            fprintf(stderr, "Fatal Python error: "
                            "unable to decode the command line argument #%i\n",
                            i + 1);
            return 1;
        }
        argv_copy2[i] = argv_copy[i];
    }
    argv_copy2[argc] = argv_copy[argc] = NULL;

    setlocale(LC_ALL, oldloc);
    PyMem_RawFree(oldloc);

    res = Py_Main(argc, argv_copy);

    /* Force again malloc() allocator to release memory blocks allocated
       before Py_Main() */
    (void)_PyMem_SetupAllocators("malloc");

    for (i = 0; i < argc; i++) {
        PyMem_RawFree(argv_copy2[i]);
    }
    PyMem_RawFree(argv_copy);
    PyMem_RawFree(argv_copy2);
    return res;
}
