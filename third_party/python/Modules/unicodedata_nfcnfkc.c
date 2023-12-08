/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/likely.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Modules/bextra.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

PyObject *
_PyUnicode_NfcNfkc(PyObject *self, PyObject *input, int k)
{
    int kind;
    void *data;
    Py_UCS4 code;
    Py_UCS4 *output;
    PyObject *result;
    int cskipped = 0;
    Py_ssize_t skipped[20];
    Py_ssize_t i, i1, o, len;
    int f,l,index,index1,comb;
    result = _PyUnicode_NfdNfkd(self, input, k);
    if (!result)
        return NULL;
    /* result will be "ready". */
    kind = PyUnicode_KIND(result);
    data = PyUnicode_DATA(result);
    len = PyUnicode_GET_LENGTH(result);
    /* We allocate a buffer for the output.
       If we find that we made no changes, we still return
       the NFD result. */
    output = PyMem_NEW(Py_UCS4, len);
    if (!output) {
        PyErr_NoMemory();
        Py_DECREF(result);
        return 0;
    }
    i = o = 0;
  again:
    while (i < len) {
      for (index = 0; index < cskipped; index++) {
          if (skipped[index] == i) {
              /* *i character is skipped.
                 Remove from list. */
              skipped[index] = skipped[cskipped-1];
              cskipped--;
              i++;
              goto again; /* continue while */
          }
      }
      /* Hangul Composition. We don't need to check for <LV,T>
         pairs, since we always have decomposed data. */
      code = PyUnicode_READ(kind, data, i);
      if ((UNLIKELY(_Hanghoul_LBase <= code && code < _Hanghoul_LBase + _Hanghoul_LCount) &&
           i + 1 < len && _Hanghoul_VBase <= PyUnicode_READ(kind, data, i+1) &&
           PyUnicode_READ(kind, data, i+1) < _Hanghoul_VBase + _Hanghoul_VCount)) {
          /* check L character is a modern leading consonant (0x1100 ~ 0x1112)
             and V character is a modern vowel (0x1161 ~ 0x1175). */
          int LIndex, VIndex;
          LIndex = code - _Hanghoul_LBase;
          VIndex = PyUnicode_READ(kind, data, i+1) - _Hanghoul_VBase;
          code = _Hanghoul_SBase + (LIndex * _Hanghoul_VCount + VIndex) * _Hanghoul_TCount;
          i+=2;
          if ((i < len &&
               _Hanghoul_TBase < PyUnicode_READ(kind, data, i) &&
               PyUnicode_READ(kind, data, i) < (_Hanghoul_TBase + _Hanghoul_TCount))) {
              /* check T character is a modern trailing consonant
                 (0x11A8 ~ 0x11C2). */
              code += PyUnicode_READ(kind, data, i) - _Hanghoul_TBase;
              i++;
          }
          output[o++] = code;
          continue;
      }
      /* code is still input[i] here */
      f = _PyUnicode_FindNfcIndex(_PyUnicode_NfcFirst, code);
      if (f == -1) {
          output[o++] = code;
          i++;
          continue;
      }
      /* Find next unblocked character. */
      i1 = i+1;
      comb = 0;
      /* output base character for now; might be updated later. */
      output[o] = PyUnicode_READ(kind, data, i);
      while (i1 < len) {
          Py_UCS4 code1 = PyUnicode_READ(kind, data, i1);
          int comb1 = _PyUnicode_GetRecord(code1)->combining;
          if (comb) {
              if (comb1 == 0)
                  break;
              if (comb >= comb1) {
                  /* Character is blocked. */
                  i1++;
                  continue;
              }
          }
          l = _PyUnicode_FindNfcIndex(_PyUnicode_NfcLast, code1);
          /* i1 cannot be combined with i. If i1
             is a starter, we don't need to look further.
             Otherwise, record the combining class. */
          if (l == -1) {
            not_combinable:
              if (comb1 == 0)
                  break;
              comb = comb1;
              i1++;
              continue;
          }
          index = f * UNIDATA_TOTAL_LAST + l;
          index1 = _PyUnicode_CompIndex[index >> _PyUnicode_CompShift];
          code = BitFieldExtract(_PyUnicode_CompData,
                                 (index1 << _PyUnicode_CompShift)+
                                 (index & ((1 << _PyUnicode_CompShift) - 1)),
                                 _PyUnicode_CompDataBits);
          if (code == 0)
              goto not_combinable;
          /* Replace the original character. */
          output[o] = code;
          /* Mark the second character unused. */
          assert(cskipped < 20);
          skipped[cskipped++] = i1;
          i1++;
          f = _PyUnicode_FindNfcIndex(_PyUnicode_NfcFirst, output[o]);
          if (f == -1)
              break;
      }
      /* Output character was already written.
         Just advance the indices. */
      o++; i++;
    }
    if (o == len) {
        /* No changes. Return original string. */
        PyMem_Free(output);
        return result;
    }
    Py_DECREF(result);
    result = PyUnicode_FromKindAndData(PyUnicode_4BYTE_KIND,
                                       output, o);
    PyMem_Free(output);
    return result;
}
