/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"

#define NEXITFUNCS 32

static void (*_Py_exitfunc)(void);
static void (*exitfuncs[NEXITFUNCS])(void);
static int nexitfuncs = 0;

void
_Py_PyAtExit(void (*func)(void))
{
    _Py_exitfunc = func;
}

void
_Py_CallExitFuncs(void)
{
    if (!_Py_exitfunc) return;
    _Py_exitfunc();
    PyErr_Clear();
}

int Py_AtExit(void (*func)(void))
{
    if (nexitfuncs >= NEXITFUNCS)
        return -1;
    exitfuncs[nexitfuncs++] = func;
    return 0;
}

void
_Py_CallLlExitFuncs(void)
{
    while (nexitfuncs > 0)
        (*exitfuncs[--nexitfuncs])();
    fflush(stdout);
    fflush(stderr);
}
