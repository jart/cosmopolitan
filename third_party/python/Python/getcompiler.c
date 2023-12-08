/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pylifecycle.h"

#ifndef COMPILER
#ifdef __llvm__
#define COMPILER "[LLVM " __VERSION__ "]"
#elif defined(__GNUC__)
#define COMPILER "[GCC " __VERSION__ "]"
#elif defined(__cplusplus)
#define COMPILER "[C++]"
#else
#define COMPILER "[C]"
#endif
#endif /* !COMPILER */

const char *
Py_GetCompiler(void)
{
    return COMPILER;
}
