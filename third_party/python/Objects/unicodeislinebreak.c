/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "third_party/python/Include/unicodeobject.h"
/* clang-format off */

/**
 * Returns 1 for Unicode characters having the line break
 * property 'BK', 'CR', 'LF' or 'NL' or having bidirectional
 * type 'B', 0 otherwise.
 */
int _PyUnicode_IsLinebreak(const Py_UCS4 ch)
{
    switch (ch) {
    case 0x000A:
    case 0x000B:
    case 0x000C:
    case 0x000D:
    case 0x001C:
    case 0x001D:
    case 0x001E:
    case 0x0085:
    case 0x2028:
    case 0x2029:
        return 1;
    default:
        return 0;
    }
}
