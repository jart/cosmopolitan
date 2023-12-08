/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Modules/bextra.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

void
_PyUnicode_GetDecompRecord(PyObject *self,
                           Py_UCS4 code,
                           int *index,
                           int *prefix,
                           int *count)
{
    unsigned decomp;
    if (code >= 0x110000) {
        *index = 0;
    } else if (self && UCD_Check(self) &&
               get_old_record(self, code)->category_changed==0) {
        /* unassigned in old version */
        *index = 0;
    }
    else {
        *index = _PyUnicode_DecompIndex1[(code>>_PyUnicode_DecompShift)];
        *index = _PyUnicode_DecompIndex2[(*index<<_PyUnicode_DecompShift)+
                                         (code&((1<<_PyUnicode_DecompShift)-1))];
    }
    /* high byte is number of hex bytes (usually one or two), low byte
       is prefix code (from*/
    decomp = BitFieldExtract(_PyUnicode_Decomp, *index, _PyUnicode_DecompBits);
    *count = decomp >> 8;
    *prefix = decomp & 255;
    (*index)++;
}
