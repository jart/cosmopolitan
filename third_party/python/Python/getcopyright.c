/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pylifecycle.h"
/* clang-format off */

/* Return the copyright string.  This is updated manually. */

static const char cprt[] =
"\
Copyright (c) 2001-2021 Python Software Foundation.\n\
All Rights Reserved.\n\
\n\
Copyright (c) 2000 BeOpen.com.\n\
All Rights Reserved.\n\
\n\
Copyright (c) 1995-2001 Corporation for National Research Initiatives.\n\
All Rights Reserved.\n\
\n\
Copyright (c) 1991-1995 Stichting Mathematisch Centrum, Amsterdam.\n\
All Rights Reserved.";

const char *
Py_GetCopyright(void)
{
    return cprt;
}
