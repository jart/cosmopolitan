/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Modules/bextra.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

PyObject *
_PyUnicode_NfdNfkd(PyObject *self, PyObject *input, int k)
{
    PyObject *result;
    Py_UCS4 *output;
    Py_ssize_t i, o, osize;
    int kind;
    void *data;
    /* Longest decomposition in Unicode 3.2: U+FDFA */
    Py_UCS4 stack[20];
    Py_ssize_t space, isize;
    int index, prefix, count, stackptr;
    unsigned char prev, cur;
    stackptr = 0;
    isize = PyUnicode_GET_LENGTH(input);
    space = isize;
    /* Overallocate at most 10 characters. */
    if (space > 10) {
        if (space <= PY_SSIZE_T_MAX - 10)
            space += 10;
    }
    else {
        space *= 2;
    }
    osize = space;
    output = PyMem_NEW(Py_UCS4, space);
    if (!output) {
        PyErr_NoMemory();
        return NULL;
    }
    i = o = 0;
    kind = PyUnicode_KIND(input);
    data = PyUnicode_DATA(input);
    while (i < isize) {
        stack[stackptr++] = PyUnicode_READ(kind, data, i++);
        while(stackptr) {
            Py_UCS4 code = stack[--stackptr];
            /* Hangul Decomposition adds three characters in
               a single step, so we need at least that much room. */
            if (space < 3) {
                Py_UCS4 *new_output;
                osize += 10;
                space += 10;
                new_output = PyMem_Realloc(output, osize*sizeof(Py_UCS4));
                if (new_output == NULL) {
                    PyMem_Free(output);
                    PyErr_NoMemory();
                    return NULL;
                }
                output = new_output;
            }
            /* Hangul Decomposition. */
            if (_Hanghoul_SBase <= code && code < (_Hanghoul_SBase + _Hanghoul_SCount)) {
                int SIndex = code - _Hanghoul_SBase;
                int L = _Hanghoul_LBase + SIndex / _Hanghoul_NCount;
                int V = _Hanghoul_VBase + (SIndex % _Hanghoul_NCount) / _Hanghoul_TCount;
                int T = _Hanghoul_TBase + SIndex % _Hanghoul_TCount;
                output[o++] = L;
                output[o++] = V;
                space -= 2;
                if (T != _Hanghoul_TBase) {
                    output[o++] = T;
                    space --;
                }
                continue;
            }
            /* normalization changes */
            if (self && UCD_Check(self)) {
                Py_UCS4 value = ((PreviousDBVersion*)self)->normalization(code);
                if (value != 0) {
                    stack[stackptr++] = value;
                    continue;
                }
            }
            /* Other decompositions. */
            _PyUnicode_GetDecompRecord(self, code, &index, &prefix, &count);
            /* Copy character if it is not decomposable, or has a
               compatibility decomposition, but we do NFD. */
            if (!count || (prefix && !k)) {
                output[o++] = code;
                space--;
                continue;
            }
            /* Copy decomposition onto the stack, in reverse
               order.  */
            while(count) {
                code = BitFieldExtract(_PyUnicode_Decomp,
                                       index + (--count),
                                       _PyUnicode_DecompBits);
                stack[stackptr++] = code;
            }
        }
    }
    result = PyUnicode_FromKindAndData(PyUnicode_4BYTE_KIND,
                                       output, o);
    PyMem_Free(output);
    if (!result)
        return NULL;
    /* result is guaranteed to be ready, as it is compact. */
    kind = PyUnicode_KIND(result);
    data = PyUnicode_DATA(result);
    /* Sort canonically. */
    i = 0;
    prev = _PyUnicode_GetRecord(PyUnicode_READ(kind, data, i))->combining;
    for (i++; i < PyUnicode_GET_LENGTH(result); i++) {
        cur = _PyUnicode_GetRecord(PyUnicode_READ(kind, data, i))->combining;
        if (prev == 0 || cur == 0 || prev <= cur) {
            prev = cur;
            continue;
        }
        /* Non-canonical order. Need to switch *i with previous. */
        o = i - 1;
        while (1) {
            Py_UCS4 tmp = PyUnicode_READ(kind, data, o+1);
            PyUnicode_WRITE(kind, data, o+1,
                            PyUnicode_READ(kind, data, o));
            PyUnicode_WRITE(kind, data, o, tmp);
            o--;
            if (o < 0)
                break;
            prev = _PyUnicode_GetRecord(PyUnicode_READ(kind, data, o))->combining;
            if (prev == 0 || prev <= cur)
                break;
        }
        prev = _PyUnicode_GetRecord(PyUnicode_READ(kind, data, i))->combining;
    }
    return result;
}
