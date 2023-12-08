/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "libc/assert.h"
#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/warnings.h"

#define _PyUnicode_STATE(op)                            \
    (((PyASCIIObject *)(op))->state)

int ensure_unicode(PyObject *);
PyObject *unicode_result(PyObject *);
int unicode_check_modifiable(PyObject *);
PyObject *unicode_encode_ucs1(PyObject *, const char *, const Py_UCS4);
PyObject *_PyUnicode_TranslateCharmap(PyObject *, PyObject *, const char *);

/* The max unicode value is always 0x10FFFF while using the PEP-393 API.
   This function is kept for backward compatibility with the old API. */
Py_UNICODE
PyUnicode_GetMax(void)
{
#ifdef Py_UNICODE_WIDE
    return 0x10FFFF;
#else
    /* This is actually an illegal character, so it should
       not be passed to unichr. */
    return 0xFFFF;
#endif
}

PyObject *
PyUnicode_AsDecodedObject(PyObject *unicode,
                          const char *encoding,
                          const char *errors)
{
    if (!PyUnicode_Check(unicode)) {
        PyErr_BadArgument();
        return NULL;
    }
    if (PyErr_WarnEx(PyExc_DeprecationWarning,
                     "PyUnicode_AsDecodedObject() is deprecated; "
                     "use PyCodec_Decode() to decode from str", 1) < 0)
        return NULL;
    if (encoding == NULL)
        encoding = PyUnicode_GetDefaultEncoding();
    /* Decode via the codec registry */
    return PyCodec_Decode(unicode, encoding, errors);
}

PyObject *
PyUnicode_AsDecodedUnicode(PyObject *unicode,
                           const char *encoding,
                           const char *errors)
{
    PyObject *v;
    if (!PyUnicode_Check(unicode)) {
        PyErr_BadArgument();
        goto onError;
    }
    if (PyErr_WarnEx(PyExc_DeprecationWarning,
                     "PyUnicode_AsDecodedUnicode() is deprecated; "
                     "use PyCodec_Decode() to decode from str to str", 1) < 0)
        return NULL;
    if (encoding == NULL)
        encoding = PyUnicode_GetDefaultEncoding();
    /* Decode via the codec registry */
    v = PyCodec_Decode(unicode, encoding, errors);
    if (v == NULL)
        goto onError;
    if (!PyUnicode_Check(v)) {
        PyErr_Format(PyExc_TypeError,
                     "'%.400s' decoder returned '%.400s' instead of 'str'; "
                     "use codecs.decode() to decode to arbitrary types",
                     encoding,
                     Py_TYPE(unicode)->tp_name);
        Py_DECREF(v);
        goto onError;
    }
    return unicode_result(v);
  onError:
    return NULL;
}

PyObject *
PyUnicode_AsEncodedObject(PyObject *unicode,
                          const char *encoding,
                          const char *errors)
{
    PyObject *v;
    if (!PyUnicode_Check(unicode)) {
        PyErr_BadArgument();
        goto onError;
    }
    if (PyErr_WarnEx(PyExc_DeprecationWarning,
                     "PyUnicode_AsEncodedObject() is deprecated; "
                     "use PyUnicode_AsEncodedString() to encode from str to bytes "
                     "or PyCodec_Encode() for generic encoding", 1) < 0)
        return NULL;
    if (encoding == NULL)
        encoding = PyUnicode_GetDefaultEncoding();
    /* Encode via the codec registry */
    v = PyCodec_Encode(unicode, encoding, errors);
    if (v == NULL)
        goto onError;
    return v;
  onError:
    return NULL;
}

PyObject *
PyUnicode_AsEncodedUnicode(PyObject *unicode,
                           const char *encoding,
                           const char *errors)
{
    PyObject *v;
    if (!PyUnicode_Check(unicode)) {
        PyErr_BadArgument();
        goto onError;
    }
    if (PyErr_WarnEx(PyExc_DeprecationWarning,
                     "PyUnicode_AsEncodedUnicode() is deprecated; "
                     "use PyCodec_Encode() to encode from str to str", 1) < 0)
        return NULL;
    if (encoding == NULL)
        encoding = PyUnicode_GetDefaultEncoding();
    /* Encode via the codec registry */
    v = PyCodec_Encode(unicode, encoding, errors);
    if (v == NULL)
        goto onError;
    if (!PyUnicode_Check(v)) {
        PyErr_Format(PyExc_TypeError,
                     "'%.400s' encoder returned '%.400s' instead of 'str'; "
                     "use codecs.encode() to encode to arbitrary types",
                     encoding,
                     Py_TYPE(v)->tp_name);
        Py_DECREF(v);
        goto onError;
    }
    return v;
  onError:
    return NULL;
}

wchar_t *
_PyUnicode_AsWideCharString(PyObject *unicode)
{
    const wchar_t *wstr;
    wchar_t *buffer;
    Py_ssize_t buflen;
    if (unicode == NULL) {
        PyErr_BadInternalCall();
        return NULL;
    }
    wstr = PyUnicode_AsUnicodeAndSize(unicode, &buflen);
    if (wstr == NULL) {
        return NULL;
    }
    if (wcslen(wstr) != (size_t)buflen) {
        PyErr_SetString(PyExc_ValueError,
                        "embedded null character");
        return NULL;
    }
    buffer = PyMem_NEW(wchar_t, buflen + 1);
    if (buffer == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    memcpy(buffer, wstr, (buflen + 1) * sizeof(wchar_t));
    return buffer;
}

const Py_UNICODE *
_PyUnicode_AsUnicode(PyObject *unicode)
{
    Py_ssize_t size;
    const Py_UNICODE *wstr;
    wstr = PyUnicode_AsUnicodeAndSize(unicode, &size);
    if (wstr && wcslen(wstr) != (size_t)size) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        return NULL;
    }
    return wstr;
}

Py_ssize_t
PyUnicode_GetSize(PyObject *unicode)
{
    if (!PyUnicode_Check(unicode)) {
        PyErr_BadArgument();
        goto onError;
    }
    return PyUnicode_GET_SIZE(unicode);
  onError:
    return -1;
}

int
PyUnicode_WriteChar(PyObject *unicode, Py_ssize_t index, Py_UCS4 ch)
{
    if (!PyUnicode_Check(unicode) || !PyUnicode_IS_COMPACT(unicode)) {
        PyErr_BadArgument();
        return -1;
    }
    assert(PyUnicode_IS_READY(unicode));
    if (index < 0 || index >= PyUnicode_GET_LENGTH(unicode)) {
        PyErr_SetString(PyExc_IndexError, "string index out of range");
        return -1;
    }
    if (unicode_check_modifiable(unicode))
        return -1;
    if (ch > PyUnicode_MAX_CHAR_VALUE(unicode)) {
        PyErr_SetString(PyExc_ValueError, "character out of range");
        return -1;
    }
    PyUnicode_WRITE(PyUnicode_KIND(unicode), PyUnicode_DATA(unicode),
                    index, ch);
    return 0;
}

/* Deprecated */
PyObject *
PyUnicode_EncodeLatin1(const Py_UNICODE *p,
                       Py_ssize_t size,
                       const char *errors)
{
    PyObject *result;
    PyObject *unicode = PyUnicode_FromUnicode(p, size);
    if (unicode == NULL)
        return NULL;
    result = unicode_encode_ucs1(unicode, errors, 256);
    Py_DECREF(unicode);
    return result;
}

/* Deprecated */
PyObject *
PyUnicode_EncodeASCII(const Py_UNICODE *p,
                      Py_ssize_t size,
                      const char *errors)
{
    PyObject *result;
    PyObject *unicode = PyUnicode_FromUnicode(p, size);
    if (unicode == NULL)
        return NULL;
    result = unicode_encode_ucs1(unicode, errors, 128);
    Py_DECREF(unicode);
    return result;
}

PyObject *
PyUnicode_Encode(const Py_UNICODE *s,
                 Py_ssize_t size,
                 const char *encoding,
                 const char *errors)
{
    PyObject *v, *unicode;
    unicode = PyUnicode_FromUnicode(s, size);
    if (unicode == NULL)
        return NULL;
    v = PyUnicode_AsEncodedString(unicode, encoding, errors);
    Py_DECREF(unicode);
    return v;
}

/* Deprecated */
PyObject *
PyUnicode_EncodeCharmap(const Py_UNICODE *p,
                        Py_ssize_t size,
                        PyObject *mapping,
                        const char *errors)
{
    PyObject *result;
    PyObject *unicode = PyUnicode_FromUnicode(p, size);
    if (unicode == NULL)
        return NULL;
    result = _PyUnicode_EncodeCharmap(unicode, mapping, errors);
    Py_DECREF(unicode);
    return result;
}

/* Deprecated. Use PyUnicode_Translate instead. */
PyObject *
PyUnicode_TranslateCharmap(const Py_UNICODE *p,
                           Py_ssize_t size,
                           PyObject *mapping,
                           const char *errors)
{
    PyObject *result;
    PyObject *unicode = PyUnicode_FromUnicode(p, size);
    if (!unicode)
        return NULL;
    result = _PyUnicode_TranslateCharmap(unicode, mapping, errors);
    Py_DECREF(unicode);
    return result;
}

void
PyUnicode_InternImmortal(PyObject **p)
{
    PyUnicode_InternInPlace(p);
    if (PyUnicode_CHECK_INTERNED(*p) != SSTATE_INTERNED_IMMORTAL) {
        _PyUnicode_STATE(*p).interned = SSTATE_INTERNED_IMMORTAL;
        Py_INCREF(*p);
    }
}

Py_UNICODE*
Py_UNICODE_strcpy(Py_UNICODE *s1, const Py_UNICODE *s2)
{
    Py_UNICODE *u = s1;
    while ((*u++ = *s2++));
    return s1;
}

Py_UNICODE*
Py_UNICODE_strncpy(Py_UNICODE *s1, const Py_UNICODE *s2, size_t n)
{
    Py_UNICODE *u = s1;
    while ((*u++ = *s2++))
        if (n-- == 0)
            break;
    return s1;
}

Py_UNICODE*
Py_UNICODE_strcat(Py_UNICODE *s1, const Py_UNICODE *s2)
{
    Py_UNICODE *u1 = s1;
    u1 += Py_UNICODE_strlen(u1);
    Py_UNICODE_strcpy(u1, s2);
    return s1;
}

int
Py_UNICODE_strcmp(const Py_UNICODE *s1, const Py_UNICODE *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
        s1++, s2++;
    if (*s1 && *s2)
        return (*s1 < *s2) ? -1 : +1;
    if (*s1)
        return 1;
    if (*s2)
        return -1;
    return 0;
}

int
Py_UNICODE_strncmp(const Py_UNICODE *s1, const Py_UNICODE *s2, size_t n)
{
    Py_UNICODE u1, u2;
    for (; n != 0; n--) {
        u1 = *s1;
        u2 = *s2;
        if (u1 != u2)
            return (u1 < u2) ? -1 : +1;
        if (u1 == '\0')
            return 0;
        s1++;
        s2++;
    }
    return 0;
}

Py_UNICODE*
Py_UNICODE_strchr(const Py_UNICODE *s, Py_UNICODE c)
{
    const Py_UNICODE *p;
    for (p = s; *p; p++)
        if (*p == c)
            return (Py_UNICODE*)p;
    return NULL;
}

Py_UNICODE*
Py_UNICODE_strrchr(const Py_UNICODE *s, Py_UNICODE c)
{
    const Py_UNICODE *p;
    p = s + Py_UNICODE_strlen(s);
    while (p != s) {
        p--;
        if (*p == c)
            return (Py_UNICODE*)p;
    }
    return NULL;
}

size_t
Py_UNICODE_strlen(const Py_UNICODE *u)
{
    int res = 0;
    while(*u++)
        res++;
    return res;
}

Py_UNICODE*
PyUnicode_AsUnicodeCopy(PyObject *unicode)
{
    Py_UNICODE *u, *copy;
    Py_ssize_t len, size;
    if (!PyUnicode_Check(unicode)) {
        PyErr_BadArgument();
        return NULL;
    }
    u = PyUnicode_AsUnicodeAndSize(unicode, &len);
    if (u == NULL)
        return NULL;
    /* Ensure we won't overflow the size. */
    if (len > ((PY_SSIZE_T_MAX / (Py_ssize_t)sizeof(Py_UNICODE)) - 1)) {
        PyErr_NoMemory();
        return NULL;
    }
    size = len + 1; /* copy the null character */
    size *= sizeof(Py_UNICODE);
    copy = PyMem_Malloc(size);
    if (copy == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    memcpy(copy, u, size);
    return copy;
}
