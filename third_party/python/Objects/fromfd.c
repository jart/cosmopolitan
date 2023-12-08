/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/yoink.h"

PYTHON_YOINK("io");

PyObject *
PyFile_FromFd(int fd, const char *name, const char *mode, int buffering,
              const char *encoding, const char *errors, const char *newline,
              int closefd)
{
    PyObject *io, *stream;
    _Py_IDENTIFIER(open);
    io = PyImport_ImportModule("io");
    if (io == NULL)
        return NULL;
    stream = _PyObject_CallMethodId(io, &PyId_open, "isisssi", fd, mode,
                                    buffering, encoding, errors,
                                    newline, closefd);
    Py_DECREF(io);
    if (stream == NULL)
        return NULL;
    /* ignore name attribute because the name attribute of _BufferedIOMixin
       and TextIOWrapper is read only */
    return stream;
}
