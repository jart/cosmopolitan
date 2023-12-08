/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyctype.h"

/* Cross platform case insensitive string compare functions
 */

int
PyOS_mystrnicmp(const char *s1, const char *s2, Py_ssize_t size)
{
    if (size == 0)
        return 0;
    while ((--size > 0) &&
           (Py_TOLOWER(*s1) == Py_TOLOWER(*s2))) {
        if (!*s1++ || !*s2++)
            break;
    }
    return Py_TOLOWER(*s1) - Py_TOLOWER(*s2);
}

int
PyOS_mystricmp(const char *s1, const char *s2)
{
    while (*s1 && (Py_TOLOWER((unsigned)*s1++) == Py_TOLOWER((unsigned)*s2++))) {
    }
    return (Py_TOLOWER((unsigned)*s1) - Py_TOLOWER((unsigned)*s2));
}
