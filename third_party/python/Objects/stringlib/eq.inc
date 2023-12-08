/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/

/* Fast unicode equal function optimized for dictobject.c and setobject.c */

/* Return 1 if two unicode objects are equal, 0 if not.
 * unicode_eq() is called when the hash of two unicode objects is equal.
 */
Py_LOCAL_INLINE(int)
unicode_eq(PyObject *aa, PyObject *bb)
{
    PyUnicodeObject *a = (PyUnicodeObject *)aa;
    PyUnicodeObject *b = (PyUnicodeObject *)bb;
    if (UNLIKELY(PyUnicode_READY(a) == -1) ||
        UNLIKELY(PyUnicode_READY(b) == -1)) {
        assert(0 && "unicode_eq ready fail");
        return 0;
    }
    if (UNLIKELY(PyUnicode_GET_LENGTH(a) != PyUnicode_GET_LENGTH(b)))
        return 0;
    if (UNLIKELY(PyUnicode_GET_LENGTH(a) == 0))
        return 1;
    if (UNLIKELY(PyUnicode_KIND(a) != PyUnicode_KIND(b)))
        return 0;
    return bcmp(PyUnicode_1BYTE_DATA(a), PyUnicode_1BYTE_DATA(b),
                PyUnicode_GET_LENGTH(a) * PyUnicode_KIND(a)) == 0;
}
