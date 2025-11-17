/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/errors.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/methodobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/unicodeobject.h"
#include "libc/errno.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("errno");
PYTHON_PROVIDE("errno.EPERM");
PYTHON_PROVIDE("errno.ENOENT");
PYTHON_PROVIDE("errno.ESRCH");
PYTHON_PROVIDE("errno.EINTR");
PYTHON_PROVIDE("errno.EIO");
PYTHON_PROVIDE("errno.ENXIO");
PYTHON_PROVIDE("errno.E2BIG");
PYTHON_PROVIDE("errno.ENOEXEC");
PYTHON_PROVIDE("errno.EBADF");
PYTHON_PROVIDE("errno.ECHILD");
PYTHON_PROVIDE("errno.EAGAIN");
PYTHON_PROVIDE("errno.ENOMEM");
PYTHON_PROVIDE("errno.EACCES");
PYTHON_PROVIDE("errno.EFAULT");
PYTHON_PROVIDE("errno.ENOTBLK");
PYTHON_PROVIDE("errno.EBUSY");
PYTHON_PROVIDE("errno.EEXIST");
PYTHON_PROVIDE("errno.EXDEV");
PYTHON_PROVIDE("errno.ENODEV");
PYTHON_PROVIDE("errno.ENOTDIR");
PYTHON_PROVIDE("errno.EISDIR");
PYTHON_PROVIDE("errno.EINVAL");
PYTHON_PROVIDE("errno.ENFILE");
PYTHON_PROVIDE("errno.EMFILE");
PYTHON_PROVIDE("errno.ENOTTY");
PYTHON_PROVIDE("errno.ETXTBSY");
PYTHON_PROVIDE("errno.EFBIG");
PYTHON_PROVIDE("errno.ENOSPC");
PYTHON_PROVIDE("errno.ESPIPE");
PYTHON_PROVIDE("errno.EROFS");
PYTHON_PROVIDE("errno.EMLINK");
PYTHON_PROVIDE("errno.EPIPE");
PYTHON_PROVIDE("errno.EDOM");
PYTHON_PROVIDE("errno.ERANGE");
PYTHON_PROVIDE("errno.EDEADLK");
PYTHON_PROVIDE("errno.ENAMETOOLONG");
PYTHON_PROVIDE("errno.ENOLCK");
PYTHON_PROVIDE("errno.ENOSYS");
PYTHON_PROVIDE("errno.ENOTEMPTY");
PYTHON_PROVIDE("errno.ELOOP");
PYTHON_PROVIDE("errno.ENOMSG");
PYTHON_PROVIDE("errno.EIDRM");
PYTHON_PROVIDE("errno.ENOTSUP");
PYTHON_PROVIDE("errno.ENOSTR");
PYTHON_PROVIDE("errno.ENODATA");
PYTHON_PROVIDE("errno.ETIME");
PYTHON_PROVIDE("errno.ENOSR");
PYTHON_PROVIDE("errno.ENONET");
PYTHON_PROVIDE("errno.EREMOTE");
PYTHON_PROVIDE("errno.ENOLINK");
PYTHON_PROVIDE("errno.EPROTO");
PYTHON_PROVIDE("errno.EMULTIHOP");
PYTHON_PROVIDE("errno.EBADMSG");
PYTHON_PROVIDE("errno.EOVERFLOW");
PYTHON_PROVIDE("errno.EBADFD");
PYTHON_PROVIDE("errno.EFTYPE");
PYTHON_PROVIDE("errno.EILSEQ");
PYTHON_PROVIDE("errno.ERESTART");
PYTHON_PROVIDE("errno.EUSERS");
PYTHON_PROVIDE("errno.ENOTSOCK");
PYTHON_PROVIDE("errno.EDESTADDRREQ");
PYTHON_PROVIDE("errno.EMSGSIZE");
PYTHON_PROVIDE("errno.EPROTOTYPE");
PYTHON_PROVIDE("errno.ENOPROTOOPT");
PYTHON_PROVIDE("errno.EPROTONOSUPPORT");
PYTHON_PROVIDE("errno.ESOCKTNOSUPPORT");
PYTHON_PROVIDE("errno.EOPNOTSUPP");
PYTHON_PROVIDE("errno.EPFNOSUPPORT");
PYTHON_PROVIDE("errno.EAFNOSUPPORT");
PYTHON_PROVIDE("errno.EADDRINUSE");
PYTHON_PROVIDE("errno.EADDRNOTAVAIL");
PYTHON_PROVIDE("errno.ENETDOWN");
PYTHON_PROVIDE("errno.ENETUNREACH");
PYTHON_PROVIDE("errno.ENETRESET");
PYTHON_PROVIDE("errno.ECONNABORTED");
PYTHON_PROVIDE("errno.ECONNRESET");
PYTHON_PROVIDE("errno.ENOBUFS");
PYTHON_PROVIDE("errno.EISCONN");
PYTHON_PROVIDE("errno.ENOTCONN");
PYTHON_PROVIDE("errno.ESHUTDOWN");
PYTHON_PROVIDE("errno.ETOOMANYREFS");
PYTHON_PROVIDE("errno.ETIMEDOUT");
PYTHON_PROVIDE("errno.ECONNREFUSED");
PYTHON_PROVIDE("errno.EHOSTDOWN");
PYTHON_PROVIDE("errno.EHOSTUNREACH");
PYTHON_PROVIDE("errno.EALREADY");
PYTHON_PROVIDE("errno.EINPROGRESS");
PYTHON_PROVIDE("errno.ESTALE");
PYTHON_PROVIDE("errno.EDQUOT");
PYTHON_PROVIDE("errno.ENOMEDIUM");
PYTHON_PROVIDE("errno.EMEDIUMTYPE");
PYTHON_PROVIDE("errno.ECANCELED");
PYTHON_PROVIDE("errno.EOWNERDEAD");
PYTHON_PROVIDE("errno.ENOTRECOVERABLE");
PYTHON_PROVIDE("errno.ERFKILL");
PYTHON_PROVIDE("errno.EHWPOISON");
PYTHON_PROVIDE("errno.EWOULDBLOCK");
PYTHON_PROVIDE("errno.errorcode");

static PyMethodDef errno_methods[] = {
    {NULL,              NULL}
};

static void
_inscode(PyObject *d, PyObject *de, const char *name, int code)
{
    PyObject *u, *v;
    if (!code) return;
    u = PyUnicode_FromString(name);
    v = PyLong_FromLong((long)code);
    /* Don't bother checking for errors; they'll be caught at the end
     * of the module initialization function by the caller of
     * initerrno().
     */
    if (u && v) {
        /* insert in modules dict */
        PyDict_SetItem(d, u, v);
        /* insert in errorcode dict */
        PyDict_SetItem(de, v, u);
    }
    Py_XDECREF(u);
    Py_XDECREF(v);
}

PyDoc_STRVAR(errno__doc__,
"This module makes available standard errno system symbols.\n\
\n\
The value of each symbol is the corresponding integer value,\n\
e.g., on most systems, errno.ENOENT equals the integer 2.\n\
\n\
The dictionary errno.errorcode maps numeric codes to symbol names,\n\
e.g., errno.errorcode[2] could be the string 'ENOENT'.\n\
\n\
Symbols that are not relevant to the underlying system are not defined.\n\
\n\
To map error codes to error messages, use the function os.strerror(),\n\
e.g. os.strerror(2) could return 'No such file or directory'.");

static struct PyModuleDef errnomodule = {
    PyModuleDef_HEAD_INIT,
    "errno",
    errno__doc__,
    -1,
    errno_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_errno(void)
{
    PyObject *m, *d, *de;
    m = PyModule_Create(&errnomodule);
    if (m == NULL)
        return NULL;
    d = PyModule_GetDict(m);
    de = PyDict_New();
    if (!d || !de || PyDict_SetItemString(d, "errorcode", de) < 0)
        return NULL;

/* Macro so I don't have to edit each and every line below... */
#define inscode(d, ds, de, name, code, comment) _inscode(d, de, name, code)

    /*
     * The names and comments are borrowed from linux/include/errno.h,
     * which should be pretty all-inclusive.  However, the Solaris specific
     * names and comments are borrowed from sys/errno.h in Solaris.
     * MacOSX specific names and comments are borrowed from sys/errno.h in
     * MacOSX.
     */

    inscode(d, ds, de, "EPERM", EPERM, "Operation not permitted");
    inscode(d, ds, de, "ENOENT", ENOENT, "No such file or directory");
    inscode(d, ds, de, "ESRCH", ESRCH, "No such process");
    inscode(d, ds, de, "EINTR", EINTR, "The greatest of all errnos");
    inscode(d, ds, de, "EIO", EIO, "I/O error");
    inscode(d, ds, de, "ENXIO", ENXIO, "No such device or address");
    inscode(d, ds, de, "E2BIG", E2BIG, "Argument list too long");
    inscode(d, ds, de, "ENOEXEC", ENOEXEC, "Exec format error");
    inscode(d, ds, de, "EBADF", EBADF, "Bad file descriptor");
    inscode(d, ds, de, "ECHILD", ECHILD, "No child process");
    inscode(d, ds, de, "EAGAIN", EAGAIN, "Resource temporarily unavailable");
    inscode(d, ds, de, "ENOMEM", ENOMEM, "Out of memory");
    inscode(d, ds, de, "EACCES", EACCES, "Permission denied");
    inscode(d, ds, de, "EFAULT", EFAULT, "Invalid memory address");
    inscode(d, ds, de, "ENOTBLK", ENOTBLK, "Block device required");
    inscode(d, ds, de, "EBUSY", EBUSY, "Device or resource busy");
    inscode(d, ds, de, "EEXIST", EEXIST, "File exists");
    inscode(d, ds, de, "EXDEV", EXDEV, "Improper link");
    inscode(d, ds, de, "ENODEV", ENODEV, "No such device");
    inscode(d, ds, de, "ENOTDIR", ENOTDIR, "Not a directory");
    inscode(d, ds, de, "EISDIR", EISDIR, "Is a directory");
    inscode(d, ds, de, "EINVAL", EINVAL, "Invalid argument");
    inscode(d, ds, de, "ENFILE", ENFILE, "Too many open files in system");
    inscode(d, ds, de, "EMFILE", EMFILE, "Too many open files");
    inscode(d, ds, de, "ENOTTY", ENOTTY, "Inappropriate i/o control operation");
    inscode(d, ds, de, "ETXTBSY", ETXTBSY, "Won't execute open in write mode");
    inscode(d, ds, de, "EFBIG", EFBIG, "File too large");
    inscode(d, ds, de, "ENOSPC", ENOSPC, "No space left on device");
    inscode(d, ds, de, "ESPIPE", ESPIPE, "Invalid seek");
    inscode(d, ds, de, "EROFS", EROFS, "Read-only filesystem");
    inscode(d, ds, de, "EMLINK", EMLINK, "Too many links");
    inscode(d, ds, de, "EPIPE", EPIPE, "Broken pipe");
    inscode(d, ds, de, "EDOM", EDOM, "Argument outside function domain");
    inscode(d, ds, de, "ERANGE", ERANGE, "Result too large");
    inscode(d, ds, de, "EDEADLK", EDEADLK, "Resource deadlock avoided");
    inscode(d, ds, de, "ENAMETOOLONG", ENAMETOOLONG, "Filename too long");
    inscode(d, ds, de, "ENOLCK", ENOLCK, "No locks available");
    inscode(d, ds, de, "ENOSYS", ENOSYS, "System call unavailable");
    inscode(d, ds, de, "ENOTEMPTY", ENOTEMPTY, "Directory not empty");
    inscode(d, ds, de, "ELOOP", ELOOP, "Too many levels of symbolic links");
    inscode(d, ds, de, "ENOMSG", ENOMSG, "No message error");
    inscode(d, ds, de, "EIDRM", EIDRM, "Identifier removed");
    inscode(d, ds, de, "ENOTSUP", ENOTSUP, "Operation not supported");
    inscode(d, ds, de, "ENOSTR", ENOSTR, "No string");
    inscode(d, ds, de, "ENODATA", ENODATA, "No data");
    inscode(d, ds, de, "ETIME", ETIME, "Timer expired");
    inscode(d, ds, de, "ENOSR", ENOSR, "Out of streams resources");
    inscode(d, ds, de, "ENONET", ENONET, "No network");
    inscode(d, ds, de, "EREMOTE", EREMOTE, "Remote error");
    inscode(d, ds, de, "ENOLINK", ENOLINK, "Link severed");
    inscode(d, ds, de, "EPROTO", EPROTO, "Protocol error");
    inscode(d, ds, de, "EMULTIHOP", EMULTIHOP, "Multihop attempted");
    inscode(d, ds, de, "EBADMSG", EBADMSG, "Bad message");
    inscode(d, ds, de, "EOVERFLOW", EOVERFLOW, "Overflow error");
    inscode(d, ds, de, "EBADFD", EBADFD, "File descriptor in bad state");
    inscode(d, ds, de, "EFTYPE", EFTYPE, "Inappropriate file type or format");
    inscode(d, ds, de, "EILSEQ", EILSEQ, "Unicode decoding error");
    inscode(d, ds, de, "ERESTART", ERESTART, "Please restart syscall");
    inscode(d, ds, de, "EUSERS", EUSERS, "Too many users");
    inscode(d, ds, de, "ENOTSOCK", ENOTSOCK, "Not a socket");
    inscode(d, ds, de, "EDESTADDRREQ", EDESTADDRREQ, "Destination address required");
    inscode(d, ds, de, "EMSGSIZE", EMSGSIZE, "Message too long");
    inscode(d, ds, de, "EPROTOTYPE", EPROTOTYPE, "Protocol wrong type for socket");
    inscode(d, ds, de, "ENOPROTOOPT", ENOPROTOOPT, "Protocol not available");
    inscode(d, ds, de, "EPROTONOSUPPORT", EPROTONOSUPPORT, "Protocol not supported");
    inscode(d, ds, de, "ESOCKTNOSUPPORT", ESOCKTNOSUPPORT, "Socket type not supported");
    inscode(d, ds, de, "EOPNOTSUPP", EOPNOTSUPP, "Operation not supported on transport endpoint");
    inscode(d, ds, de, "EPFNOSUPPORT", EPFNOSUPPORT, "Protocol family not supported");
    inscode(d, ds, de, "EAFNOSUPPORT", EAFNOSUPPORT, "Address family not supported");
    inscode(d, ds, de, "EADDRINUSE", EADDRINUSE, "Address already in use");
    inscode(d, ds, de, "EADDRNOTAVAIL", EADDRNOTAVAIL, "Address not available");
    inscode(d, ds, de, "ENETDOWN", ENETDOWN, "Network is down");
    inscode(d, ds, de, "ENETUNREACH", ENETUNREACH, "Host is unreachable");
    inscode(d, ds, de, "ENETRESET", ENETRESET, "Connection reset by network");
    inscode(d, ds, de, "ECONNABORTED", ECONNABORTED, "Connection reset before accept");
    inscode(d, ds, de, "ECONNRESET", ECONNRESET, "Connection reset by client");
    inscode(d, ds, de, "ENOBUFS", ENOBUFS, "No buffer space available");
    inscode(d, ds, de, "EISCONN", EISCONN, "Socket is connected");
    inscode(d, ds, de, "ENOTCONN", ENOTCONN, "Socket is not connected");
    inscode(d, ds, de, "ESHUTDOWN", ESHUTDOWN, "Transport endpoint shutdown");
    inscode(d, ds, de, "ETOOMANYREFS", ETOOMANYREFS, "Too many references: cannot splice");
    inscode(d, ds, de, "ETIMEDOUT", ETIMEDOUT, "Connection timed out");
    inscode(d, ds, de, "ECONNREFUSED", ECONNREFUSED, "Connection refused error");
    inscode(d, ds, de, "EHOSTDOWN", EHOSTDOWN, "Host down error");
    inscode(d, ds, de, "EHOSTUNREACH", EHOSTUNREACH, "Host unreachable error");
    inscode(d, ds, de, "EALREADY", EALREADY, "Connection already in progress");
    inscode(d, ds, de, "EINPROGRESS", EINPROGRESS, "Operation already in progress");
    inscode(d, ds, de, "ESTALE", ESTALE, "Stale error");
    inscode(d, ds, de, "EDQUOT", EDQUOT, "Disk quota exceeded");
    inscode(d, ds, de, "ENOMEDIUM", ENOMEDIUM, "No medium found");
    inscode(d, ds, de, "EMEDIUMTYPE", EMEDIUMTYPE, "Wrong medium type");
    inscode(d, ds, de, "ECANCELED", ECANCELED, "Operation canceled");
    inscode(d, ds, de, "EOWNERDEAD", EOWNERDEAD, "Owner died");
    inscode(d, ds, de, "ENOTRECOVERABLE", ENOTRECOVERABLE, "State not recoverable");
    inscode(d, ds, de, "ERFKILL", ERFKILL, "Operation not possible due to RF-kill");
    inscode(d, ds, de, "EHWPOISON", EHWPOISON, "Memory page has hardware error");
    inscode(d, ds, de, "EWOULDBLOCK", EWOULDBLOCK, "Operation would block");

    Py_DECREF(de);
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_errno = {
    "errno",
    PyInit_errno,
};
