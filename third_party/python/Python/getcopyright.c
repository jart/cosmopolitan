/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/sections.internal.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "third_party/python/Include/pylifecycle.h"

asm(".ident\t\"\\n\\n\
Python 3.6 (https://docs.python.org/3/license.html)\\n\
Copyright (c) 2001-2021 Python Software Foundation.\\n\
All Rights Reserved.\\n\
Copyright (c) 2000 BeOpen.com.\\n\
All Rights Reserved.\\n\
Copyright (c) 1995-2001 Corporation for National Research Initiatives.\\n\
All Rights Reserved.\\n\
Copyright (c) 1991-1995 Stichting Mathematisch Centrum, Amsterdam.\\n\
All Rights Reserved.\"");

const char *
Py_GetCopyright(void)
{
    static char *res;
    if (!res) {
        char *r = 0;
        const char *p;
        appends(&r, "");
        for (p = __comment_start; *p; p += strlen(p) + 1) {
            appends(&r, p);
        }
        res = r;
    }
    return res;
}
