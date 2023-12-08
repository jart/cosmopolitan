/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/object.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

const _PyUnicode_Record *
_PyUnicode_GetRecord(Py_UCS4 c)
{
    int k, i;
    if (c >= 0x110000) {
        i = 0;
    } else {
        k = _PyUnicode_RecordsShift;
        i = _PyUnicode_RecordsIndex1[(c >> k)];
        i = _PyUnicode_RecordsIndex2[(i << k) + (c & ((1 << k) - 1))];
    }
    return _PyUnicode_Records + i;
}
