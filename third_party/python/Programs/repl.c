/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"
#include "libc/unicode/locale.h"
#include "libc/x/x.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/funcobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"
/* clang-format off */

const struct _frozen *PyImport_FrozenModules = _PyImport_FrozenModules;
struct _inittab *PyImport_Inittab = _PyImport_Inittab;
static jmp_buf jbuf;

static void
OnKeyboardInterrupt(int sig)
{
    gclongjmp(jbuf, 1);
}

static void
AddCompletion(linenoiseCompletions *c, char *s)
{
    c->cvec = realloc(c->cvec, ++c->len * sizeof(*c->cvec));
    c->cvec[c->len - 1] = s;
}

static void
CompleteDict(const char *b, const char *q, const char *p,
             linenoiseCompletions *c, PyObject *o)
{
    const char *s;
    PyObject *k, *v;
    Py_ssize_t i, m;
    for (i = 0; PyDict_Next(o, &i, &k, &v);) {
        if ((v != Py_None && PyUnicode_Check(k) &&
             (s = PyUnicode_AsUTF8AndSize(k, &m)) &&
             m >= q - p && !memcmp(s, p, q - p))) {
            AddCompletion(c, xasprintf("%.*s%.*s", p - b, b, m, s));
        }
    }
}

static void
CompleteDir(const char *b, const char *q, const char *p,
            linenoiseCompletions *c, PyObject *o)
{
    Py_ssize_t m;
    const char *s;
    PyObject *d, *i, *k;
    if (!(d = PyObject_Dir(o))) return;
    if ((i = PyObject_GetIter(d))) {
        while ((k = PyIter_Next(i))) {
            if (((s = PyUnicode_AsUTF8AndSize(k, &m)) &&
                 m >= q - p && !memcmp(s, p, q - p))) {
                AddCompletion(c, xasprintf("%.*s%.*s", p - b, b, m, s));
            }
            Py_DECREF(k);
        }
        Py_DECREF(i);
    }
    Py_DECREF(d);
}

static void
TerminalCompletion(const char *p, linenoiseCompletions *c)
{
    PyObject *o, *t, *i;
    const char *q, *s, *b;
    for (b = p, p += strlen(p); p > b; --p) {
        if (!isalnum(p[-1]) && p[-1] != '.' && p[-1] != '_') {
            break;
        }
    }
    o = PyModule_GetDict(PyImport_AddModule("__main__"));
    if (!*(q = strchrnul(p, '.'))) {
        CompleteDict(b, q, p, c, o);
        CompleteDir(b, q, p, c, PyDict_GetItemString(o, "__builtins__"));
    } else {
        s = strndup(p, q - p);
        if ((t = PyDict_GetItemString(o, s))) {
            Py_INCREF(t);
        } else {
            o = PyDict_GetItemString(o, "__builtins__");
            if (PyObject_HasAttrString(o, s)) {
                t = PyObject_GetAttrString(o, s);
            }
        }
        while ((p = q + 1), (o = t)) {
            if (*(q = strchrnul(p, '.'))) {
                t = PyObject_GetAttrString(o, gc(strndup(p, q - p)));
                Py_DECREF(o);
            } else {
                CompleteDir(b, q, p, c, o);
                Py_DECREF(o);
                break;
            }
        }
        free(s);
    }
}

static char *
TerminalHint(const char *p, const char **ansi1, const char **ansi2)
{
    char *h = 0;
    linenoiseCompletions c = {0};
    TerminalCompletion(p, &c);
    if (c.len == 1) {
        h = strdup(c.cvec[0] + strlen(p));
    }
    linenoiseFreeCompletions(&c);
    return h;
}

static char *
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
    p = ezlinenoise(prompt, "python");
    PyOS_setsig(SIGINT, saint);
    if (p) {
        n = strlen(p);
        q = PyMem_RawMalloc(n + 2);
        strcpy(mempcpy(q, p, n), "\n");
        free(p);
        clearerr(sys_stdin);
    } else {
        q = PyMem_RawMalloc(1);
        if (q) *q = 0;
    }
    return q;
}

int
main(int argc, char **argv)
{
    wchar_t **argv_copy;
    /* We need a second copy, as Python might modify the first one. */
    wchar_t **argv_copy2;
    int i, res;
    char *oldloc;

    /* if (FindDebugBinary()) { */
    /*     ShowCrashReports(); */
    /* } */

    PyOS_ReadlineFunctionPointer = TerminalReadline;
    linenoiseSetCompletionCallback(TerminalCompletion);
    linenoiseSetHintsCallback(TerminalHint);
    linenoiseSetFreeHintsCallback(free);

    /* Force malloc() allocator to bootstrap Python */
    _PyMem_SetupAllocators("malloc");

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
    _PyMem_SetupAllocators("malloc");

    for (i = 0; i < argc; i++) {
        PyMem_RawFree(argv_copy2[i]);
    }
    PyMem_RawFree(argv_copy);
    PyMem_RawFree(argv_copy2);
    return res;
}
