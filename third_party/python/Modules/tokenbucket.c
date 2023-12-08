/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "net/http/tokenbucket.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/timer.h"
#include "net/http/tokenbucket.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/intrcheck.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "libc/serialize.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("tokenbucket");
PYTHON_PROVIDE("tokenbucket.acquire");
PYTHON_PROVIDE("tokenbucket.blackhole");
PYTHON_PROVIDE("tokenbucket.count");
PYTHON_PROVIDE("tokenbucket.program");

struct TokenBucket {
    int fd;
    int pid;
    signed char cidr;
    struct timespec replenish;
    union {
        atomic_schar *b;
        atomic_uint_fast64_t *w;
    };
} g_tokenbucket;

PyDoc_STRVAR(tokenbucket_doc,
"Token Bucket Module\n\
\n\
This module implements the token bucket algorithm, which can help\n\
keep you safe from denial of service attacks.");

wontreturn static void
tokenbucket_onsignal(int sig)
{
    _Exit(0);
}

static void
tokenbucket_atexit(void)
{
    kill(g_tokenbucket.pid, SIGTERM);
}

wontreturn static void
tokenbucket_replenisher(void)
{
    struct timespec ts;
    signal(SIGINT, tokenbucket_onsignal);
    signal(SIGHUP, tokenbucket_onsignal);
    signal(SIGTERM, tokenbucket_onsignal);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    ts = timespec_real();
    for (;;) {
        if (!clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, 0)) {
            ReplenishTokens(g_tokenbucket.w, (1ul << g_tokenbucket.cidr) / 8);
            ts = timespec_add(ts, g_tokenbucket.replenish);
        }
    }
}

PyDoc_STRVAR(config_doc,
"config($module, replenish=1.0, cidr=24)\n\
--\n\n\
Configures global token bucket.\n\
\n\
This function launches a background process. This function can only be\n\
called once. The replenish interval is in seconds and specifies how\n\
often the background process will add a token to each bucket. The\n\
cidr specifies the network bit granularity.");

static PyObject *
tokenbucket_config(PyObject *self, PyObject *args)
{
    double replenish = 1.0;
    unsigned cidr = 24;
    if (g_tokenbucket.pid > 0) {
        PyErr_SetString(PyExc_ValueError,
                        "tokenbucket.config() can only be called once");
        return 0;
    }
    if (!PyArg_ParseTuple(args, "|dI:config", &replenish, &cidr)) {
        return 0;
    }
    if (!(8 <= cidr && cidr <= 32)) {
        PyErr_SetString(PyExc_ValueError, "require 8 <= cidr <= 32");
        return 0;
    }
    if (!(0 < replenish && replenish <= LONG_MAX)) {
        PyErr_SetString(PyExc_ValueError, "need 0 < replenish <= LONG_MAX");
        return 0;
    }
    if (!(g_tokenbucket.b = _mapshared(1ul << cidr))) {
        PyErr_NoMemory();
        return 0;
    }
    memset(g_tokenbucket.b, 127, 1ul << cidr);
    g_tokenbucket.cidr = cidr;
    g_tokenbucket.replenish = timespec_frommicros(replenish * 1e6);
    _PyImport_AcquireLock();
    g_tokenbucket.pid = fork();
    if (!g_tokenbucket.pid) {
        PyOS_AfterFork();
    } else {
        _PyImport_ReleaseLock();
    }
    if (g_tokenbucket.pid == -1) {
        munmap(g_tokenbucket.b, 1ul << cidr);
        return PyErr_SetFromErrno(PyExc_OSError);
    }
    atexit(tokenbucket_atexit);
    if (!g_tokenbucket.pid) {
        tokenbucket_replenisher();
        __builtin_unreachable();
    }
    Py_RETURN_NONE;
}

PyDoc_STRVAR(acquire_doc,
"acquire($module, ip)\n\
--\n\n\
Acquires token for IP address.\n\
\n\
This removes a token from the bucket associated with `ip` and then\n\
returns the number of tokens that were in the bucket beforehand.\n\
\n\
Return values greater than zero mean a token was atomically acquired.\n\
Values less than or equal zero means the bucket is empty.");

static PyObject *
tokenbucket_acquire(PyObject *self, PyObject *args)
{
    uint32_t ip;
    const char *ipstr;
    if (g_tokenbucket.pid <= 0) {
        PyErr_SetString(PyExc_ValueError,
                        "tokenbucket.config() needs to be called first");
        return 0;
    }
    if (!PyArg_ParseTuple(args, "s:acquire", &ipstr)) {
        return 0;
    }
    if ((ip = ntohl(inet_addr(ipstr))) == -1u) {
        PyErr_SetString(PyExc_ValueError, "bad ipv4 address");
        return 0;
    }
    return PyLong_FromLong(AcquireToken(g_tokenbucket.b, ip,
                                        g_tokenbucket.cidr));
}

PyDoc_STRVAR(count_doc,
"count($module, ip)\n\
--\n\n\
Counts token for IP address.\n\
\n\
Return values greater than zero mean a token was atomically countd.\n\
Values less than or equal zero means the bucket is empty.");

static PyObject *
tokenbucket_count(PyObject *self, PyObject *args)
{
    uint32_t ip;
    const char *ipstr;
    if (g_tokenbucket.pid <= 0) {
        PyErr_SetString(PyExc_ValueError,
                        "tokenbucket.config() needs to be called first");
        return 0;
    }
    if (!PyArg_ParseTuple(args, "s:count", &ipstr)) {
        return 0;
    }
    if ((ip = ntohl(inet_addr(ipstr))) == -1u) {
        PyErr_SetString(PyExc_ValueError, "bad ipv4 address");
        return 0;
    }
    return PyLong_FromLong(CountTokens(g_tokenbucket.b, ip,
                                       g_tokenbucket.cidr));
}

PyDoc_STRVAR(blackhole_doc,
"blackhole($module, ip)\n\
--\n\n\
Blackholes token for IP address.\n\
\n\
Bans IP address by sending it to blackholed.com. Returns 0 on success\n\
or errno on error. To test if blackholed is running, ban 0.0.0.0.");

static PyObject *
tokenbucket_blackhole(PyObject *self, PyObject *args)
{
    int fd, fd2;
    char buf[4];
    uint32_t ip;
    const char *ipstr;
    struct sockaddr_un addr = {AF_UNIX, "/var/run/blackhole.sock"};
    if (!PyArg_ParseTuple(args, "s:blackhole", &ipstr)) {
        return 0;
    }
    if ((ip = ntohl(inet_addr(ipstr))) == -1u) {
        PyErr_SetString(PyExc_ValueError, "bad ipv4 address");
        return 0;
    }
    if (!(fd = g_tokenbucket.fd)) {
        if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
            return PyLong_FromLong(errno);
        }
        fd2 = fcntl(fd, F_DUPFD_CLOEXEC, 10);
        close(fd);
        if (fd2 == -1) {
            return PyLong_FromLong(errno);
        }
        g_tokenbucket.fd = fd = fd2;
    }
    WRITE32BE(buf, ip);
    if (sendto(fd, buf, 4, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        return PyLong_FromLong(errno);
    }
    return PyLong_FromLong(0);
}

static PyMethodDef tokenbucket_methods[] = {
    {"config", tokenbucket_config, METH_VARARGS, config_doc},
    {"acquire", tokenbucket_acquire, METH_VARARGS, acquire_doc},
    {"count", tokenbucket_count, METH_VARARGS, count_doc},
    {"blackhole", tokenbucket_blackhole, METH_VARARGS, blackhole_doc},
    {0},
};

static struct PyModuleDef tokenbucketmodule = {
    PyModuleDef_HEAD_INIT,
    "tokenbucket",
    tokenbucket_doc,
    -1,
    tokenbucket_methods,
};

PyMODINIT_FUNC
PyInit_tokenbucket(void)
{
    PyObject *m;
    if (!(m = PyModule_Create(&tokenbucketmodule))) return 0;
    return !PyErr_Occurred() ? m : 0;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_tokenbucket = {
    "tokenbucket",
    PyInit_tokenbucket,
};
