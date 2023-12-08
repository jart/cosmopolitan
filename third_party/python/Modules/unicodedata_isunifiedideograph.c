/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Modules/unicodedata.h"

/* These ranges need to match makeunicodedata.py:cjk_ranges. */
int
_PyUnicode_IsUnifiedIdeograph(Py_UCS4 code)
{
    return
        (0x3400 <= code && code <= 0x4DB5)   || /* CJK Ideograph Extension A */
        (0x4E00 <= code && code <= 0x9FD5)   || /* CJK Ideograph */
        (0x20000 <= code && code <= 0x2A6D6) || /* CJK Ideograph Extension B */
        (0x2A700 <= code && code <= 0x2B734) || /* CJK Ideograph Extension C */
        (0x2B740 <= code && code <= 0x2B81D) || /* CJK Ideograph Extension D */
        (0x2B820 <= code && code <= 0x2CEA1);   /* CJK Ideograph Extension E */
}
