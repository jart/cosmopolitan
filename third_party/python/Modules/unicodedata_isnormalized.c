/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Modules/unicodedata.h"

/**
 * Returns 1 if the input is certainly normalized, 0 if it might not be. 
 */
int
_PyUnicode_IsNormalized(PyObject *self, PyObject *input, int nfc, int k)
{
    int kind;
    void *data;
    Py_ssize_t i, len;
    unsigned char prev_combining = 0, quickcheck_mask;
    /* An older version of the database is requested, quickchecks must be
       disabled. */
    if (self && UCD_Check(self))
        return 0;
    /* The two quickcheck bits at this shift mean 0=Yes, 1=Maybe, 2=No,
       as described in http://unicode.org/reports/tr15/#Annex8. */
    quickcheck_mask = 3 << ((nfc ? 4 : 0) + (k ? 2 : 0));
    i = 0;
    kind = PyUnicode_KIND(input);
    data = PyUnicode_DATA(input);
    len = PyUnicode_GET_LENGTH(input);
    while (i < len) {
        Py_UCS4 ch = PyUnicode_READ(kind, data, i++);
        const _PyUnicode_Record *record = _PyUnicode_GetRecord(ch);
        unsigned char combining = record->combining;
        unsigned char quickcheck = record->normalization_quick_check;
        if (quickcheck & quickcheck_mask)
            return 0; /* this string might need normalization */
        if (combining && prev_combining > combining)
            return 0; /* non-canonical sort order, not normalized */
        prev_combining = combining;
    }
    return 1; /* certainly normalized */
}
