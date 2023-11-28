#ifndef Py__SOCKET_H
#define Py__SOCKET_H
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pytime.h"
COSMOPOLITAN_C_START_

/* Python module and C API name */
#define PySocket_MODULE_NAME    "_socket"
#define PySocket_CAPI_NAME      "CAPI"
#define PySocket_CAPSULE_NAME   PySocket_MODULE_NAME "." PySocket_CAPI_NAME

typedef int SOCKET_T;
#define SIZEOF_SOCKET_T __SIZEOF_INT__

#if SIZEOF_SOCKET_T <= SIZEOF_LONG
#define PyLong_FromSocket_t(fd) PyLong_FromLong((SOCKET_T)(fd))
#define PyLong_AsSocket_t(fd) (SOCKET_T)PyLong_AsLong(fd)
#else
#define PyLong_FromSocket_t(fd) PyLong_FromLongLong((SOCKET_T)(fd))
#define PyLong_AsSocket_t(fd) (SOCKET_T)PyLong_AsLongLong(fd)
#endif
 
/* Socket address */
typedef union sock_addr {
    struct sockaddr_in in;
    struct sockaddr sa;
    struct sockaddr_un un;
#ifdef ENABLE_IPV6
    struct sockaddr_in6 in6;
    struct sockaddr_storage storage;
#endif
#ifdef HAVE_BLUETOOTH_BLUETOOTH_H
    struct sockaddr_l2 bt_l2;
    struct sockaddr_rc bt_rc;
    struct sockaddr_sco bt_sco;
    struct sockaddr_hci bt_hci;
#endif
#ifdef HAVE_NETPACKET_PACKET_H
    struct sockaddr_ll ll;
#endif
#ifdef HAVE_LINUX_CAN_H
    struct sockaddr_can can;
#endif
#ifdef HAVE_SYS_KERN_CONTROL_H
    struct sockaddr_ctl ctl;
#endif
#ifdef HAVE_SOCKADDR_ALG
    struct sockaddr_alg alg;
#endif
} sock_addr_t;

/* The object holding a socket.  It holds some extra information,
   like the address family, which is used to decode socket address
   arguments properly. */

typedef struct {
    PyObject_HEAD
    SOCKET_T sock_fd;           /* Socket file descriptor */
    int sock_family;            /* Address family, e.g., AF_INET */
    int sock_type;              /* Socket type, e.g., SOCK_STREAM */
    int sock_proto;             /* Protocol type, usually 0 */
    PyObject *(*errorhandler)(void); /* Error handler; checks
                                        errno, returns NULL and
                                        sets a Python exception */
    _PyTime_t sock_timeout;     /* Operation timeout in seconds;
                                        0.0 means non-blocking */
} PySocketSockObject;

/* --- C API ----------------------------------------------------*/

/* Short explanation of what this C API export mechanism does
   and how it works:

    The _ssl module needs access to the type object defined in
    the _socket module. Since cross-DLL linking introduces a lot of
    problems on many platforms, the "trick" is to wrap the
    C API of a module in a struct which then gets exported to
    other modules via a PyCapsule.

    The code in socketmodule.c defines this struct (which currently
    only contains the type object reference, but could very
    well also include other C APIs needed by other modules)
    and exports it as PyCapsule via the module dictionary
    under the name "CAPI".

    Other modules can now include the socketmodule.h file
    which defines the needed C APIs to import and set up
    a static copy of this struct in the importing module.

    After initialization, the importing module can then
    access the C APIs from the _socket module by simply
    referring to the static struct, e.g.

    Load _socket module and its C API; this sets up the global
    PySocketModule:

    if (PySocketModule_ImportModuleAndAPI())
        return;


    Now use the C API as if it were defined in the using
    module:

    if (!PyArg_ParseTuple(args, "O!|zz:ssl",

                          PySocketModule.Sock_Type,

                          (PyObject*)&Sock,
                          &key_file, &cert_file))
        return NULL;

    Support could easily be extended to export more C APIs/symbols
    this way. Currently, only the type object is exported,
    other candidates would be socket constructors and socket
    access functions.

*/

/* C API for usage by other Python modules */
typedef struct {
    PyTypeObject *Sock_Type;
    PyObject *error;
    PyObject *timeout_error;
} PySocketModule_APIObject;

#define PySocketModule_ImportModuleAndAPI() PyCapsule_Import(PySocket_CAPSULE_NAME, 1)

COSMOPOLITAN_C_END_
#endif /* !Py__SOCKET_H */
