/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/unicodeobject.h"

/* bytes to hex implementation */

static PyObject *_Py_strhex_impl(const char* argbuf, const Py_ssize_t arglen,
                                 int return_bytes)
{
    PyObject *retval;
    Py_UCS1* retbuf;
    Py_ssize_t i, j;

    assert(arglen >= 0);
    if (arglen > PY_SSIZE_T_MAX / 2)
        return PyErr_NoMemory();

    if (return_bytes) {
        /* If _PyBytes_FromSize() were public we could avoid malloc+copy. */
        retbuf = (Py_UCS1*) PyMem_Malloc(arglen*2);
	if (!retbuf)
	    return PyErr_NoMemory();
        retval = NULL;  /* silence a compiler warning, assigned later. */
    } else {
	retval = PyUnicode_New(arglen*2, 127);
	if (!retval)
	    return NULL;
	retbuf = PyUnicode_1BYTE_DATA(retval);
    }

    /* make hex version of string, taken from shamodule.c */
    for (i=j=0; i < arglen; i++) {
        unsigned char c;
        c = (argbuf[i] >> 4) & 0xf;
        retbuf[j++] = Py_hexdigits[c];
        c = argbuf[i] & 0xf;
        retbuf[j++] = Py_hexdigits[c];
    }

    if (return_bytes) {
        retval = PyBytes_FromStringAndSize((const char *)retbuf, arglen*2);
        PyMem_Free(retbuf);
    }
#ifdef Py_DEBUG
    else {
        assert(_PyUnicode_CheckConsistency(retval, 1));
    }
#endif

    return retval;
}

PyObject * _Py_strhex(const char* argbuf, const Py_ssize_t arglen)
{
    return _Py_strhex_impl(argbuf, arglen, 0);
}

/* Same as above but returns a bytes() instead of str() to avoid the
 * need to decode the str() when bytes are needed. */
PyObject * _Py_strhex_bytes(const char* argbuf, const Py_ssize_t arglen)
{
    return _Py_strhex_impl(argbuf, arglen, 1);
}
