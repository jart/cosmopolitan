/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/unicodeobject.h"

/*
    Return the file system path representation of the object.

    If the object is str or bytes, then allow it to pass through with
    an incremented refcount. If the object defines __fspath__(), then
    return the result of that method. All other types raise a TypeError.
*/
PyObject *
PyOS_FSPath(PyObject *path)
{
    /* For error message reasons, this function is manually inlined in
       path_converter(). */
    _Py_IDENTIFIER(__fspath__);
    PyObject *func = NULL;
    PyObject *path_repr = NULL;

    if (PyUnicode_Check(path) || PyBytes_Check(path)) {
        Py_INCREF(path);
        return path;
    }

    func = _PyObject_LookupSpecial(path, &PyId___fspath__);
    if (NULL == func) {
        return PyErr_Format(PyExc_TypeError,
                            "expected str, bytes or os.PathLike object, "
                            "not %.200s",
                            Py_TYPE(path)->tp_name);
    }

    path_repr = PyObject_CallFunctionObjArgs(func, NULL);
    Py_DECREF(func);
    if (NULL == path_repr) {
        return NULL;
    }

    if (!(PyUnicode_Check(path_repr) || PyBytes_Check(path_repr))) {
        PyErr_Format(PyExc_TypeError,
                     "expected %.200s.__fspath__() to return str or bytes, "
                     "not %.200s", Py_TYPE(path)->tp_name,
                     Py_TYPE(path_repr)->tp_name);
        Py_DECREF(path_repr);
        return NULL;
    }

    return path_repr;
}
