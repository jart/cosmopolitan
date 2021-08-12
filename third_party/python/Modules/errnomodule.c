#include "libc/dce.h"
#include "libc/errno.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/methodobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/unicodeobject.h"
/* clang-format off */

/*
 * Pull in the system error definitions
 */

static PyMethodDef errno_methods[] = {
    {NULL,              NULL}
};

/* Helper function doing the dictionary inserting */

static void
_inscode(PyObject *d, PyObject *de, const char *name, int code)
{
    PyObject *u = PyUnicode_FromString(name);
    PyObject *v = PyLong_FromLong((long) code);

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

    if (EPROTO) inscode(d, ds, de, "EPROTO", EPROTO, "Protocol error");
    if (ENOMSG) inscode(d, ds, de, "ENOMSG", ENOMSG, "No message of desired type");
    if (ENODATA) inscode(d, ds, de, "ENODATA", ENODATA, "No data available");
    if (EOVERFLOW) inscode(d, ds, de, "EOVERFLOW", EOVERFLOW, "Value too large for defined data type");

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

    if (ENOMEDIUM) inscode(d, ds, de, "ENOMEDIUM", ENOMEDIUM, "No medium found");
    if (EMEDIUMTYPE) inscode(d, ds, de, "EMEDIUMTYPE", EMEDIUMTYPE, "Wrong medium type");
    if (ECANCELED) inscode(d, ds, de, "ECANCELED", ECANCELED, "Operation Canceled");
    if (EOWNERDEAD) inscode(d, ds, de, "EOWNERDEAD", EOWNERDEAD, "Owner died");
    if (ENOTRECOVERABLE) inscode(d, ds, de, "ENOTRECOVERABLE", ENOTRECOVERABLE, "State not recoverable");

#if !IsTiny()
    /* Linux junk errors */
    if (ENOANO) inscode(d, ds, de, "ENOANO", ENOANO, "No anode");
    if (EADV) inscode(d, ds, de, "EADV", EADV, "Advertise error");
    if (EL2HLT) inscode(d, ds, de, "EL2HLT", EL2HLT, "Level 2 halted");
    if (EDOTDOT) inscode(d, ds, de, "EDOTDOT", EDOTDOT, "RFS specific error");
    if (ENOPKG) inscode(d, ds, de, "ENOPKG", ENOPKG, "Package not installed");
    if (EBADR) inscode(d, ds, de, "EBADR", EBADR, "Invalid request descriptor");
    if (ENOCSI) inscode(d, ds, de, "ENOCSI", ENOCSI, "No CSI structure available");
    if (ENOKEY) inscode(d, ds, de, "ENOKEY", ENOKEY, "Required key not available");
    if (EUCLEAN) inscode(d, ds, de, "EUCLEAN", EUCLEAN, "Structure needs cleaning");
    if (ECHRNG) inscode(d, ds, de, "ECHRNG", ECHRNG, "Channel number out of range");
    if (EL2NSYNC) inscode(d, ds, de, "EL2NSYNC", EL2NSYNC, "Level 2 not synchronized");
    if (EKEYEXPIRED) inscode(d, ds, de, "EKEYEXPIRED", EKEYEXPIRED, "Key has expired");
    if (ENAVAIL) inscode(d, ds, de, "ENAVAIL", ENAVAIL, "No XENIX semaphores available");
    if (EKEYREVOKED) inscode(d, ds, de, "EKEYREVOKED", EKEYREVOKED, "Key has been revoked");
    if (ELIBBAD) inscode(d, ds, de, "ELIBBAD", ELIBBAD, "Accessing a corrupted shared library");
    if (EKEYREJECTED) inscode(d, ds, de, "EKEYREJECTED", EKEYREJECTED, "Key was rejected by service");
    if (ERFKILL) inscode(d, ds, de, "ERFKILL", ERFKILL, "Operation not possible due to RF-kill");
#endif

    /* Solaris-specific errnos */
#ifdef ECANCELED
    inscode(d, ds, de, "ECANCELED", ECANCELED, "Operation canceled");
#endif
#ifdef ENOTSUP
    inscode(d, ds, de, "ENOTSUP", ENOTSUP, "Operation not supported");
#endif
#ifdef EOWNERDEAD
    inscode(d, ds, de, "EOWNERDEAD", EOWNERDEAD, "Process died with the lock");
#endif
#ifdef ENOTRECOVERABLE
    inscode(d, ds, de, "ENOTRECOVERABLE", ENOTRECOVERABLE, "Lock is not recoverable");
#endif
#ifdef ELOCKUNMAPPED
    inscode(d, ds, de, "ELOCKUNMAPPED", ELOCKUNMAPPED, "Locked lock was unmapped");
#endif
#ifdef ENOTACTIVE
    inscode(d, ds, de, "ENOTACTIVE", ENOTACTIVE, "Facility is not active");
#endif

    /* MacOSX specific errnos */
#ifdef EAUTH
    inscode(d, ds, de, "EAUTH", EAUTH, "Authentication error");
#endif
#ifdef EBADARCH
    inscode(d, ds, de, "EBADARCH", EBADARCH, "Bad CPU type in executable");
#endif
#ifdef EBADEXEC
    inscode(d, ds, de, "EBADEXEC", EBADEXEC, "Bad executable (or shared library)");
#endif
#ifdef EBADMACHO
    inscode(d, ds, de, "EBADMACHO", EBADMACHO, "Malformed Mach-o file");
#endif
#ifdef EBADRPC
    inscode(d, ds, de, "EBADRPC", EBADRPC, "RPC struct is bad");
#endif
#ifdef EDEVERR
    inscode(d, ds, de, "EDEVERR", EDEVERR, "Device error");
#endif
#ifdef EFTYPE
    inscode(d, ds, de, "EFTYPE", EFTYPE, "Inappropriate file type or format");
#endif
#ifdef ENEEDAUTH
    inscode(d, ds, de, "ENEEDAUTH", ENEEDAUTH, "Need authenticator");
#endif
#ifdef ENOATTR
    inscode(d, ds, de, "ENOATTR", ENOATTR, "Attribute not found");
#endif
#ifdef ENOPOLICY
    inscode(d, ds, de, "ENOPOLICY", ENOPOLICY, "Policy not found");
#endif
#ifdef EPROCLIM
    inscode(d, ds, de, "EPROCLIM", EPROCLIM, "Too many processes");
#endif
#ifdef EPROCUNAVAIL
    inscode(d, ds, de, "EPROCUNAVAIL", EPROCUNAVAIL, "Bad procedure for program");
#endif
#ifdef EPROGMISMATCH
    inscode(d, ds, de, "EPROGMISMATCH", EPROGMISMATCH, "Program version wrong");
#endif
#ifdef EPROGUNAVAIL
    inscode(d, ds, de, "EPROGUNAVAIL", EPROGUNAVAIL, "RPC prog. not avail");
#endif
#ifdef EPWROFF
    inscode(d, ds, de, "EPWROFF", EPWROFF, "Device power is off");
#endif
#ifdef ERPCMISMATCH
    inscode(d, ds, de, "ERPCMISMATCH", ERPCMISMATCH, "RPC version wrong");
#endif
#ifdef ESHLIBVERS
    inscode(d, ds, de, "ESHLIBVERS", ESHLIBVERS, "Shared library version mismatch");
#endif

    Py_DECREF(de);
    return m;
}
