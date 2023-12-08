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
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("errno");
PYTHON_PROVIDE("errno.E2BIG");
PYTHON_PROVIDE("errno.EACCES");
PYTHON_PROVIDE("errno.EADDRINUSE");
PYTHON_PROVIDE("errno.EADDRNOTAVAIL");
PYTHON_PROVIDE("errno.EADV");
PYTHON_PROVIDE("errno.EAFNOSUPPORT");
PYTHON_PROVIDE("errno.EAGAIN");
PYTHON_PROVIDE("errno.EALREADY");
PYTHON_PROVIDE("errno.EBADE");
PYTHON_PROVIDE("errno.EBADF");
PYTHON_PROVIDE("errno.EBADFD");
PYTHON_PROVIDE("errno.EBADMSG");
PYTHON_PROVIDE("errno.EBADR");
PYTHON_PROVIDE("errno.EBADRQC");
PYTHON_PROVIDE("errno.EBADSLT");
PYTHON_PROVIDE("errno.EBUSY");
PYTHON_PROVIDE("errno.ECANCELED");
PYTHON_PROVIDE("errno.ECHILD");
PYTHON_PROVIDE("errno.ECHRNG");
PYTHON_PROVIDE("errno.ECOMM");
PYTHON_PROVIDE("errno.ECONNABORTED");
PYTHON_PROVIDE("errno.ECONNREFUSED");
PYTHON_PROVIDE("errno.ECONNRESET");
PYTHON_PROVIDE("errno.EDEADLK");
PYTHON_PROVIDE("errno.EDESTADDRREQ");
PYTHON_PROVIDE("errno.EDOM");
PYTHON_PROVIDE("errno.EDOTDOT");
PYTHON_PROVIDE("errno.EDQUOT");
PYTHON_PROVIDE("errno.EEXIST");
PYTHON_PROVIDE("errno.EFAULT");
PYTHON_PROVIDE("errno.EFBIG");
PYTHON_PROVIDE("errno.EHOSTDOWN");
PYTHON_PROVIDE("errno.EHOSTUNREACH");
PYTHON_PROVIDE("errno.EIDRM");
PYTHON_PROVIDE("errno.EILSEQ");
PYTHON_PROVIDE("errno.EINPROGRESS");
PYTHON_PROVIDE("errno.EINTR");
PYTHON_PROVIDE("errno.EINVAL");
PYTHON_PROVIDE("errno.EIO");
PYTHON_PROVIDE("errno.EISCONN");
PYTHON_PROVIDE("errno.EISDIR");
PYTHON_PROVIDE("errno.EISNAM");
PYTHON_PROVIDE("errno.EKEYEXPIRED");
PYTHON_PROVIDE("errno.EKEYREJECTED");
PYTHON_PROVIDE("errno.EKEYREVOKED");
PYTHON_PROVIDE("errno.EL2HLT");
PYTHON_PROVIDE("errno.EL2NSYNC");
PYTHON_PROVIDE("errno.EL3HLT");
PYTHON_PROVIDE("errno.EL3RST");
PYTHON_PROVIDE("errno.ELIBACC");
PYTHON_PROVIDE("errno.ELIBBAD");
PYTHON_PROVIDE("errno.ELIBEXEC");
PYTHON_PROVIDE("errno.ELIBMAX");
PYTHON_PROVIDE("errno.ELIBSCN");
PYTHON_PROVIDE("errno.ELNRNG");
PYTHON_PROVIDE("errno.ELOOP");
PYTHON_PROVIDE("errno.EMEDIUMTYPE");
PYTHON_PROVIDE("errno.EMFILE");
PYTHON_PROVIDE("errno.EMLINK");
PYTHON_PROVIDE("errno.EMSGSIZE");
PYTHON_PROVIDE("errno.EMULTIHOP");
PYTHON_PROVIDE("errno.ENAMETOOLONG");
PYTHON_PROVIDE("errno.ENAVAIL");
PYTHON_PROVIDE("errno.ENETDOWN");
PYTHON_PROVIDE("errno.ENETRESET");
PYTHON_PROVIDE("errno.ENETUNREACH");
PYTHON_PROVIDE("errno.ENFILE");
PYTHON_PROVIDE("errno.ENOANO");
PYTHON_PROVIDE("errno.ENOBUFS");
PYTHON_PROVIDE("errno.ENOCSI");
PYTHON_PROVIDE("errno.ENODATA");
PYTHON_PROVIDE("errno.ENODEV");
PYTHON_PROVIDE("errno.ENOENT");
PYTHON_PROVIDE("errno.ENOEXEC");
PYTHON_PROVIDE("errno.ENOKEY");
PYTHON_PROVIDE("errno.ENOLCK");
PYTHON_PROVIDE("errno.ENOLINK");
PYTHON_PROVIDE("errno.ENOMEDIUM");
PYTHON_PROVIDE("errno.ENOMEM");
PYTHON_PROVIDE("errno.ENOMSG");
PYTHON_PROVIDE("errno.ENONET");
PYTHON_PROVIDE("errno.ENOPKG");
PYTHON_PROVIDE("errno.ENOPROTOOPT");
PYTHON_PROVIDE("errno.ENOSPC");
PYTHON_PROVIDE("errno.ENOSR");
PYTHON_PROVIDE("errno.ENOSTR");
PYTHON_PROVIDE("errno.ENOSYS");
PYTHON_PROVIDE("errno.ENOTBLK");
PYTHON_PROVIDE("errno.ENOTCONN");
PYTHON_PROVIDE("errno.ENOTDIR");
PYTHON_PROVIDE("errno.ENOTEMPTY");
PYTHON_PROVIDE("errno.ENOTNAM");
PYTHON_PROVIDE("errno.ENOTRECOVERABLE");
PYTHON_PROVIDE("errno.ENOTSOCK");
PYTHON_PROVIDE("errno.ENOTSUP");
PYTHON_PROVIDE("errno.ENOTTY");
PYTHON_PROVIDE("errno.ENOTUNIQ");
PYTHON_PROVIDE("errno.ENXIO");
PYTHON_PROVIDE("errno.EOPNOTSUPP");
PYTHON_PROVIDE("errno.EOVERFLOW");
PYTHON_PROVIDE("errno.EOWNERDEAD");
PYTHON_PROVIDE("errno.EPERM");
PYTHON_PROVIDE("errno.EPFNOSUPPORT");
PYTHON_PROVIDE("errno.EPIPE");
PYTHON_PROVIDE("errno.EPROTO");
PYTHON_PROVIDE("errno.EPROTONOSUPPORT");
PYTHON_PROVIDE("errno.EPROTOTYPE");
PYTHON_PROVIDE("errno.ERANGE");
PYTHON_PROVIDE("errno.EREMCHG");
PYTHON_PROVIDE("errno.EREMOTE");
PYTHON_PROVIDE("errno.EREMOTEIO");
PYTHON_PROVIDE("errno.ERESTART");
PYTHON_PROVIDE("errno.ERFKILL");
PYTHON_PROVIDE("errno.EROFS");
PYTHON_PROVIDE("errno.ESHUTDOWN");
PYTHON_PROVIDE("errno.ESOCKTNOSUPPORT");
PYTHON_PROVIDE("errno.ESPIPE");
PYTHON_PROVIDE("errno.ESRCH");
PYTHON_PROVIDE("errno.ESRMNT");
PYTHON_PROVIDE("errno.ESTALE");
PYTHON_PROVIDE("errno.ESTRPIPE");
PYTHON_PROVIDE("errno.ETIME");
PYTHON_PROVIDE("errno.ETIMEDOUT");
PYTHON_PROVIDE("errno.ETOOMANYREFS");
PYTHON_PROVIDE("errno.ETXTBSY");
PYTHON_PROVIDE("errno.EUCLEAN");
PYTHON_PROVIDE("errno.EUNATCH");
PYTHON_PROVIDE("errno.EUSERS");
PYTHON_PROVIDE("errno.EWOULDBLOCK");
PYTHON_PROVIDE("errno.EXDEV");
PYTHON_PROVIDE("errno.EXFULL");
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

    inscode(d, ds, de, "EPIPE", EPIPE, "Broken pipe");
    inscode(d, ds, de, "ENODEV", ENODEV, "No such device");
    inscode(d, ds, de, "EINVAL", EINVAL, "Invalid argument");
    inscode(d, ds, de, "EINTR", EINTR, "Interrupted system call");
    inscode(d, ds, de, "ENOTBLK", ENOTBLK, "Block device required");
    inscode(d, ds, de, "ENOSYS", ENOSYS, "Function not implemented");
    inscode(d, ds, de, "EHOSTUNREACH", EHOSTUNREACH, "No route to host");
    inscode(d, ds, de, "ESRCH", ESRCH, "No such process");
    inscode(d, ds, de, "EUSERS", EUSERS, "Too many users");
    inscode(d, ds, de, "EXDEV", EXDEV, "Cross-device link");
    inscode(d, ds, de, "E2BIG", E2BIG, "Arg list too long");
    inscode(d, ds, de, "EREMOTE", EREMOTE, "Object is remote");
    inscode(d, ds, de, "ECHILD", ECHILD, "No child processes");
    inscode(d, ds, de, "EMSGSIZE", EMSGSIZE, "Message too long");
    inscode(d, ds, de, "ENOTEMPTY", ENOTEMPTY, "Directory not empty");
    inscode(d, ds, de, "ENOBUFS", ENOBUFS, "No buffer space available");
    inscode(d, ds, de, "ELOOP", ELOOP, "Too many symbolic links encountered");
    inscode(d, ds, de, "EAFNOSUPPORT", EAFNOSUPPORT, "Address family not supported by protocol");
    inscode(d, ds, de, "EHOSTDOWN", EHOSTDOWN, "Host is down");
    inscode(d, ds, de, "EPFNOSUPPORT", EPFNOSUPPORT, "Protocol family not supported");
    inscode(d, ds, de, "ENOPROTOOPT", ENOPROTOOPT, "Protocol not available");
    inscode(d, ds, de, "EBUSY", EBUSY, "Device or resource busy");
    inscode(d, ds, de, "EWOULDBLOCK", EWOULDBLOCK, "Operation would block");
    inscode(d, ds, de, "EBADFD", EBADFD, "File descriptor in bad state");
    inscode(d, ds, de, "EISCONN", EISCONN, "Transport endpoint is already connected");
    inscode(d, ds, de, "ESHUTDOWN", ESHUTDOWN, "Cannot send after transport endpoint shutdown");
    inscode(d, ds, de, "ENONET", ENONET, "Machine is not on the network");
    inscode(d, ds, de, "EBADE", EBADE, "Invalid exchange");
    inscode(d, ds, de, "EBADF", EBADF, "Bad file number");
    inscode(d, ds, de, "EMULTIHOP", EMULTIHOP, "Multihop attempted");
    inscode(d, ds, de, "EIO", EIO, "I/O error");
    inscode(d, ds, de, "EUNATCH", EUNATCH, "Protocol driver not attached");
    inscode(d, ds, de, "EPROTOTYPE", EPROTOTYPE, "Protocol wrong type for socket");
    inscode(d, ds, de, "ENOSPC", ENOSPC, "No space left on device");
    inscode(d, ds, de, "ENOEXEC", ENOEXEC, "Exec format error");
    inscode(d, ds, de, "EALREADY", EALREADY, "Operation already in progress");
    inscode(d, ds, de, "ENETDOWN", ENETDOWN, "Network is down");
    inscode(d, ds, de, "ENOTNAM", ENOTNAM, "Not a XENIX named type file");
    inscode(d, ds, de, "EACCES", EACCES, "Permission denied");
    inscode(d, ds, de, "ELNRNG", ELNRNG, "Link number out of range");
    inscode(d, ds, de, "EILSEQ", EILSEQ, "Illegal byte sequence");
    inscode(d, ds, de, "ENOTDIR", ENOTDIR, "Not a directory");
    inscode(d, ds, de, "ENOTUNIQ", ENOTUNIQ, "Name not unique on network");
    inscode(d, ds, de, "EPERM", EPERM, "Operation not permitted");
    inscode(d, ds, de, "EDOM", EDOM, "Math argument out of domain of func");
    inscode(d, ds, de, "EXFULL", EXFULL, "Exchange full");
    inscode(d, ds, de, "ECONNREFUSED", ECONNREFUSED, "Connection refused");
    inscode(d, ds, de, "EISDIR", EISDIR, "Is a directory");
    inscode(d, ds, de, "EPROTONOSUPPORT", EPROTONOSUPPORT, "Protocol not supported");
    inscode(d, ds, de, "EROFS", EROFS, "Read-only file system");
    inscode(d, ds, de, "EADDRNOTAVAIL", EADDRNOTAVAIL, "Cannot assign requested address");
    inscode(d, ds, de, "EIDRM", EIDRM, "Identifier removed");
    inscode(d, ds, de, "ECOMM", ECOMM, "Communication error on send");
    inscode(d, ds, de, "ESRMNT", ESRMNT, "Srmount error");
    inscode(d, ds, de, "EREMOTEIO", EREMOTEIO, "Remote I/O error");
    inscode(d, ds, de, "EL3RST", EL3RST, "Level 3 reset");
    inscode(d, ds, de, "EBADMSG", EBADMSG, "Not a data message");
    inscode(d, ds, de, "ENFILE", ENFILE, "File table overflow");
    inscode(d, ds, de, "ELIBMAX", ELIBMAX, "Attempting to link in too many shared libraries");
    inscode(d, ds, de, "ESPIPE", ESPIPE, "Illegal seek");
    inscode(d, ds, de, "ENOLINK", ENOLINK, "Link has been severed");
    inscode(d, ds, de, "ENETRESET", ENETRESET, "Network dropped connection because of reset");
    inscode(d, ds, de, "ETIMEDOUT", ETIMEDOUT, "Connection timed out");
    inscode(d, ds, de, "ENOENT", ENOENT, "No such file or directory");
    inscode(d, ds, de, "EEXIST", EEXIST, "File exists");
    inscode(d, ds, de, "EDQUOT", EDQUOT, "Quota exceeded");
    inscode(d, ds, de, "ENOSTR", ENOSTR, "Device not a stream");
    inscode(d, ds, de, "EBADSLT", EBADSLT, "Invalid slot");
    inscode(d, ds, de, "EBADRQC", EBADRQC, "Invalid request code");
    inscode(d, ds, de, "ELIBACC", ELIBACC, "Can not access a needed shared library");
    inscode(d, ds, de, "EFAULT", EFAULT, "Bad address");
    inscode(d, ds, de, "EFBIG", EFBIG, "File too large");
    inscode(d, ds, de, "EDEADLK", EDEADLK, "Resource deadlock would occur");
    inscode(d, ds, de, "ENOTCONN", ENOTCONN, "Transport endpoint is not connected");
    inscode(d, ds, de, "EDESTADDRREQ", EDESTADDRREQ, "Destination address required");
    inscode(d, ds, de, "ELIBSCN", ELIBSCN, ".lib section in a.out corrupted");
    inscode(d, ds, de, "ENOLCK", ENOLCK, "No record locks available");
    inscode(d, ds, de, "EISNAM", EISNAM, "Is a named type file");
    inscode(d, ds, de, "ECONNABORTED", ECONNABORTED, "Software caused connection abort");
    inscode(d, ds, de, "ENETUNREACH", ENETUNREACH, "Network is unreachable");
    inscode(d, ds, de, "ESTALE", ESTALE, "Stale NFS file handle");
    inscode(d, ds, de, "ENOSR", ENOSR, "Out of streams resources");
    inscode(d, ds, de, "ENOMEM", ENOMEM, "Out of memory");
    inscode(d, ds, de, "ENOTSOCK", ENOTSOCK, "Socket operation on non-socket");
    inscode(d, ds, de, "ESTRPIPE", ESTRPIPE, "Streams pipe error");
    inscode(d, ds, de, "EMLINK", EMLINK, "Too many links");
    inscode(d, ds, de, "ERANGE", ERANGE, "Math result not representable");
    inscode(d, ds, de, "ELIBEXEC", ELIBEXEC, "Cannot exec a shared library directly");
    inscode(d, ds, de, "EL3HLT", EL3HLT, "Level 3 halted");
    inscode(d, ds, de, "ECONNRESET", ECONNRESET, "Connection reset by peer");
    inscode(d, ds, de, "EADDRINUSE", EADDRINUSE, "Address already in use");
    inscode(d, ds, de, "EOPNOTSUPP", EOPNOTSUPP, "Operation not supported on transport endpoint");
    inscode(d, ds, de, "EREMCHG", EREMCHG, "Remote address changed");
    inscode(d, ds, de, "EAGAIN", EAGAIN, "Try again");
    inscode(d, ds, de, "ENAMETOOLONG", ENAMETOOLONG, "File name too long");
    inscode(d, ds, de, "ENOTTY", ENOTTY, "Not a typewriter");
    inscode(d, ds, de, "ERESTART", ERESTART, "Interrupted system call should be restarted");
    inscode(d, ds, de, "ESOCKTNOSUPPORT", ESOCKTNOSUPPORT, "Socket type not supported");
    inscode(d, ds, de, "ETIME", ETIME, "Timer expired");
    inscode(d, ds, de, "ETOOMANYREFS", ETOOMANYREFS, "Too many references: cannot splice");
    inscode(d, ds, de, "EMFILE", EMFILE, "Too many open files");
    inscode(d, ds, de, "ETXTBSY", ETXTBSY, "Text file busy");
    inscode(d, ds, de, "EINPROGRESS", EINPROGRESS, "Operation now in progress");
    inscode(d, ds, de, "ENXIO", ENXIO, "No such device or address");
    inscode(d, ds, de, "ENOTSUP", ENOTSUP, "Operation not supported");

    /* might not be available */
    inscode(d, ds, de, "EPROTO", EPROTO, "Protocol error");
    inscode(d, ds, de, "ENOMSG", ENOMSG, "No message of desired type");
    inscode(d, ds, de, "ENODATA", ENODATA, "No data available");
    inscode(d, ds, de, "EOVERFLOW", EOVERFLOW, "Value too large for defined data type");
    inscode(d, ds, de, "ENOMEDIUM", ENOMEDIUM, "No medium found");
    inscode(d, ds, de, "EMEDIUMTYPE", EMEDIUMTYPE, "Wrong medium type");
    inscode(d, ds, de, "ECANCELED", ECANCELED, "Operation Canceled");
    inscode(d, ds, de, "EOWNERDEAD", EOWNERDEAD, "Owner died");
    inscode(d, ds, de, "ENOTRECOVERABLE", ENOTRECOVERABLE, "State not recoverable");
    inscode(d, ds, de, "EOWNERDEAD", EOWNERDEAD, "Process died with the lock");
    inscode(d, ds, de, "ENOTRECOVERABLE", ENOTRECOVERABLE, "Lock is not recoverable");

    /* bsd only */
    inscode(d, ds, de, "EFTYPE", EFTYPE, "Inappropriate file type or format");
    inscode(d, ds, de, "EAUTH", EAUTH, "Authentication error");
    inscode(d, ds, de, "EBADRPC", EBADRPC, "RPC struct is bad");
    inscode(d, ds, de, "ENEEDAUTH", ENEEDAUTH, "Need authenticator");
    inscode(d, ds, de, "ENOATTR", ENOATTR, "Attribute not found");
    inscode(d, ds, de, "EPROCUNAVAIL", EPROCUNAVAIL, "Bad procedure for program");
    inscode(d, ds, de, "EPROGMISMATCH", EPROGMISMATCH, "Program version wrong");
    inscode(d, ds, de, "EPROGUNAVAIL", EPROGUNAVAIL, "RPC prog. not avail");
    inscode(d, ds, de, "ERPCMISMATCH", ERPCMISMATCH, "RPC version wrong");

    /* bsd and windows literally */
    inscode(d, ds, de, "EPROCLIM", EPROCLIM, "Too many processes");

    /* xnu only */
    inscode(d, ds, de, "EBADARCH", EBADARCH, "Bad CPU type in executable");
    inscode(d, ds, de, "EBADEXEC", EBADEXEC, "Bad executable (or shared library)");
    inscode(d, ds, de, "EBADMACHO", EBADMACHO, "Malformed Mach-o file");
    inscode(d, ds, de, "EDEVERR", EDEVERR, "Device error");
    inscode(d, ds, de, "ENOPOLICY", ENOPOLICY, "Policy not found");
    inscode(d, ds, de, "EPWROFF", EPWROFF, "Device power is off");
    inscode(d, ds, de, "ESHLIBVERS", ESHLIBVERS, "Shared library version mismatch");

    /* linux undocumented errnos */
    inscode(d, ds, de, "ENOANO", ENOANO, "No anode");
    inscode(d, ds, de, "EADV", EADV, "Advertise error");
    inscode(d, ds, de, "EL2HLT", EL2HLT, "Level 2 halted");
    inscode(d, ds, de, "EDOTDOT", EDOTDOT, "RFS specific error");
    inscode(d, ds, de, "ENOPKG", ENOPKG, "Package not installed");
    inscode(d, ds, de, "EBADR", EBADR, "Invalid request descriptor");
    inscode(d, ds, de, "ENOCSI", ENOCSI, "No CSI structure available");
    inscode(d, ds, de, "ENOKEY", ENOKEY, "Required key not available");
    inscode(d, ds, de, "EUCLEAN", EUCLEAN, "Structure needs cleaning");
    inscode(d, ds, de, "ECHRNG", ECHRNG, "Channel number out of range");
    inscode(d, ds, de, "EL2NSYNC", EL2NSYNC, "Level 2 not synchronized");
    inscode(d, ds, de, "EKEYEXPIRED", EKEYEXPIRED, "Key has expired");
    inscode(d, ds, de, "ENAVAIL", ENAVAIL, "No XENIX semaphores available");
    inscode(d, ds, de, "EKEYREVOKED", EKEYREVOKED, "Key has been revoked");
    inscode(d, ds, de, "ELIBBAD", ELIBBAD, "Accessing a corrupted shared library");
    inscode(d, ds, de, "EKEYREJECTED", EKEYREJECTED, "Key was rejected by service");
    inscode(d, ds, de, "ERFKILL", ERFKILL, "Operation not possible due to RF-kill");

    /* solaris only */
#ifdef ELOCKUNMAPPED
    inscode(d, ds, de, "ELOCKUNMAPPED", ELOCKUNMAPPED, "Locked lock was unmapped");
#endif
#ifdef ENOTACTIVE
    inscode(d, ds, de, "ENOTACTIVE", ENOTACTIVE, "Facility is not active");
#endif

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
