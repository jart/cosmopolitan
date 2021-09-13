/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pylifecycle.h"
/* clang-format off */

#ifdef MS_WINDOWS
static wchar_t *progname = L"python";
#else
static wchar_t *progname = L"python3";
#endif

void
Py_SetProgramName(wchar_t *pn)
{
    if (pn && *pn)
        progname = pn;
}

wchar_t *
Py_GetProgramName(void)
{
    return progname;
}
