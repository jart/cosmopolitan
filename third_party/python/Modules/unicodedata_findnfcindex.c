/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Modules/unicodedata.h"

int
_PyUnicode_FindNfcIndex(const _PyUnicode_Reindex *nfc, Py_UCS4 code)
{
    unsigned int index;
    for (index = 0; nfc[index].start; index++) {
        unsigned int start = nfc[index].start;
        if (code < start)
            return -1;
        if (code <= start + nfc[index].count) {
            unsigned int delta = code - start;
            return nfc[index].index + delta;
        }
    }
    return -1;
}
