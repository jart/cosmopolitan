/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/

#if STRINGLIB_IS_UNICODE
# error "ctype.h only compatible with byte-wise strings"
#endif

#include "third_party/python/Include/bytes_methods.h"

static PyObject*
stringlib_isspace(PyObject *self)
{
    return _Py_bytes_isspace(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}

static PyObject*
stringlib_isalpha(PyObject *self)
{
    return _Py_bytes_isalpha(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}

static PyObject*
stringlib_isalnum(PyObject *self)
{
    return _Py_bytes_isalnum(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}

static PyObject*
stringlib_isdigit(PyObject *self)
{
    return _Py_bytes_isdigit(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}

static PyObject*
stringlib_islower(PyObject *self)
{
    return _Py_bytes_islower(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}

static PyObject*
stringlib_isupper(PyObject *self)
{
    return _Py_bytes_isupper(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}

static PyObject*
stringlib_istitle(PyObject *self)
{
    return _Py_bytes_istitle(STRINGLIB_STR(self), STRINGLIB_LEN(self));
}


/* functions that return a new object partially translated by ctype funcs: */

static PyObject*
stringlib_lower(PyObject *self)
{
    PyObject* newobj;
    newobj = STRINGLIB_NEW(NULL, STRINGLIB_LEN(self));
    if (!newobj)
            return NULL;
    _Py_bytes_lower(STRINGLIB_STR(newobj), STRINGLIB_STR(self),
                 STRINGLIB_LEN(self));
    return newobj;
}

static PyObject*
stringlib_upper(PyObject *self)
{
    PyObject* newobj;
    newobj = STRINGLIB_NEW(NULL, STRINGLIB_LEN(self));
    if (!newobj)
            return NULL;
    _Py_bytes_upper(STRINGLIB_STR(newobj), STRINGLIB_STR(self),
                 STRINGLIB_LEN(self));
    return newobj;
}

static PyObject*
stringlib_title(PyObject *self)
{
    PyObject* newobj;
    newobj = STRINGLIB_NEW(NULL, STRINGLIB_LEN(self));
    if (!newobj)
            return NULL;
    _Py_bytes_title(STRINGLIB_STR(newobj), STRINGLIB_STR(self),
                 STRINGLIB_LEN(self));
    return newobj;
}

static PyObject*
stringlib_capitalize(PyObject *self)
{
    PyObject* newobj;
    newobj = STRINGLIB_NEW(NULL, STRINGLIB_LEN(self));
    if (!newobj)
            return NULL;
    _Py_bytes_capitalize(STRINGLIB_STR(newobj), STRINGLIB_STR(self),
                      STRINGLIB_LEN(self));
    return newobj;
}

static PyObject*
stringlib_swapcase(PyObject *self)
{
    PyObject* newobj;
    newobj = STRINGLIB_NEW(NULL, STRINGLIB_LEN(self));
    if (!newobj)
            return NULL;
    _Py_bytes_swapcase(STRINGLIB_STR(newobj), STRINGLIB_STR(self),
                    STRINGLIB_LEN(self));
    return newobj;
}
