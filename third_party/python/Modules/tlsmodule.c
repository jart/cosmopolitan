/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Copying of this file is authorized only if (1) you are Justine Tunney,       │
│ or (2) you make absolutely no changes to your copy.                          │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "net/https/https.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/structmember.h"
#include "third_party/python/Include/yoink.h"

/**
 * @fileoverview Enough TLS support for HttpsClient so far.
 */

PYTHON_PROVIDE("tls");

#if 0
#define LOG(...) kprintf(__VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

struct Tls {
    PyObject_HEAD
    int fd;
    PyObject *todo;
    mbedtls_ssl_config conf;
    mbedtls_ssl_context ssl;
    mbedtls_ctr_drbg_context rng;
};

static PyObject *TlsError;
static PyTypeObject tls_type;

static PyObject *
SetTlsError(int rc)
{
    char b[128];
    mbedtls_strerror(rc, b, sizeof(b));
    PyErr_SetString(TlsError, b);
    return NULL;
}

static int
TlsSend(void *c, const unsigned char *p, size_t n)
{
    int rc;
    struct Tls *self = c;
    for (;;) {
        rc = write(self->fd, p, n);
        if (rc != -1) {
            return rc;
        } else if (errno == EINTR) {
            if (PyErr_CheckSignals()) {
                return -1;
            }
        } else {
            PyErr_SetFromErrno(PyExc_OSError);
            return -1;
        }
    }
}

static int
TlsRecv(void *c, unsigned char *p, size_t n, uint32_t t)
{
    int rc;
    struct Tls *self = c;
    for (;;) {
        rc = read(self->fd, p, n);
        if (rc != -1) {
            return rc;
        } else if (errno == EINTR) {
            if (PyErr_CheckSignals()) {
                return -1;
            }
        } else {
            PyErr_SetFromErrno(PyExc_OSError);
            return -1;
        }
    }
}

static struct Tls *
tls_new(int fd, const char *host, PyObject *todo)
{
    struct Tls *self;
    LOG("TLS.new\n");
    if ((self = PyObject_New(struct Tls, &tls_type))) {
        self->fd = fd;
        self->todo = todo;
        Py_INCREF(todo);
        InitializeRng(&self->rng);
        mbedtls_ssl_init(&self->ssl);
        mbedtls_ssl_config_init(&self->conf);
        mbedtls_ssl_config_defaults(&self->conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
        mbedtls_ssl_conf_rng(&self->conf, mbedtls_ctr_drbg_random, &self->rng);
        mbedtls_ssl_conf_ca_chain(&self->conf, GetSslRoots(), 0);
        /* mbedtls_ssl_conf_dbg(&self->conf, TlsDebug, 0); */
        /* mbedtls_debug_threshold = 5; */
        if (host && *host) {
            mbedtls_ssl_conf_authmode(&self->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
            mbedtls_ssl_set_hostname(&self->ssl, host);
        } else {
            mbedtls_ssl_conf_authmode(&self->conf, MBEDTLS_SSL_VERIFY_NONE);
        }
        mbedtls_ssl_set_bio(&self->ssl, self, TlsSend, 0, TlsRecv);
        mbedtls_ssl_setup(&self->ssl, &self->conf);
        self->conf.disable_compression = true;
    }
    return self;
}

static void
tls_dealloc(struct Tls *self)
{
    LOG("TLS.dealloc\n");
    if (self->fd != -1) {
        LOG("TLS CLOSING\n");
        close(self->fd);
    }
    Py_DECREF(self->todo);
    if (PyErr_Occurred()) {
        PyErr_Clear();
    }
    mbedtls_ssl_free(&self->ssl);
    mbedtls_ctr_drbg_free(&self->rng);
    mbedtls_ssl_config_free(&self->conf);
    PyObject_Del(self);
}

PyDoc_STRVAR(tls_send__doc__, "\
send($self, bytes, /)\n\
--\n\n\
Sends bytes as ciphertext to file descriptor, returning number\n\
of bytes from buffer transmitted, or raising OSError/TlsError.");

static PyObject *
tls_send(struct Tls *self, PyObject *args)
{
    int rc;
    PyObject *res;
    Py_buffer data;
    LOG("TLS.send\n");
    if (!PyArg_ParseTuple(args, "y*:send", &data)) return 0;
    rc = mbedtls_ssl_write(&self->ssl, data.buf, data.len);
    if (rc != -1) {
        if (rc >= 0) {
            res = PyLong_FromLong(rc);
        } else {
            SetTlsError(rc);
            LOG("TLS CLOSING\n");
            close(self->fd);
            self->fd = -1;
            res = 0;
        }
    } else {
        res = 0;
    }
    PyBuffer_Release(&data);
    return res;
}

PyDoc_STRVAR(tls_sendall__doc__, "\
sendall($self, bytes, /)\n\
--\n\n\
Sends all bytes to file descriptor as ciphertext, returning number\n\
of bytes from buffer transmitted, or raising OSError / TlsError.");

static PyObject *
tls_sendall(struct Tls *self, PyObject *args)
{
    LOG("TLS.sendall\n");
    int rc;
    Py_ssize_t i;
    PyObject *res;
    Py_buffer data;
    if (!PyArg_ParseTuple(args, "y*:sendall", &data)) return 0;
    for (i = 0;;) {
        rc = mbedtls_ssl_write(&self->ssl, (char *)data.buf + i, data.len - i);
        if (rc > 0) {
            if ((i += rc) == data.len) {
                res = Py_None;
                Py_INCREF(res);
                break;
            }
        } else {
            if (rc != -1) {
                SetTlsError(rc);
                LOG("TLS CLOSING\n");
                close(self->fd);
                self->fd = -1;
            }
            res = 0;
            break;
        }
    }
    PyBuffer_Release(&data);
    return res;
}

PyDoc_STRVAR(tls_recv__doc__, "\
recv($self, nbytes, /)\n\
--\n\n\
Receives deciphered bytes from file descriptor, returning bytes\n\
or raising OSError / TlsError.");

static PyObject *
tls_recv(struct Tls *self, PyObject *args)
{
    LOG("TLS.recv\n");
    int rc;
    Py_ssize_t n;
    PyObject *res, *buf;
    if (!PyArg_ParseTuple(args, "n:recv", &n)) return 0;
    if (n < 0) {
        PyErr_SetString(PyExc_ValueError,
                        "negative buffersize in recv");
        return NULL;
    }
    if (!(buf = PyBytes_FromStringAndSize(0, n))) return 0;
    rc = mbedtls_ssl_read(&self->ssl, PyBytes_AS_STRING(buf), n);
    if (rc != -1) {
        if (rc >= 0) {
            if (rc != n) {
                _PyBytes_Resize(&buf, rc);
            }
            res = buf;
        } else {
            Py_DECREF(buf);
            SetTlsError(rc);
            LOG("TLS CLOSING\n");
            close(self->fd);
            self->fd = -1;
            res = 0;
        }
    } else {
        Py_DECREF(buf);
        res = 0;
    }
    return res;
}

PyDoc_STRVAR(tls_recv_into__doc__, "\
recv_into($self, buf, /)\n\
--\n\n\
Reads into an existing buffer...");

static PyObject *
tls_recv_into(struct Tls *self, PyObject *args)
{
    LOG("TLS.recv_into\n");
    int rc;
    PyObject *res;
    Py_buffer buf;
    if (!PyArg_ParseTuple(args, "w*:recv_into", &buf)) return 0;
    rc = mbedtls_ssl_read(&self->ssl, buf.buf, buf.len);
    if (rc != -1) {
        if (rc >= 0) {
            res = PyLong_FromLong(rc);
            LOG("got %d\n", rc);
        } else {
            SetTlsError(rc);
            LOG("TLS CLOSING\n");
            close(self->fd);
            self->fd = -1;
            res = 0;
        }
    } else {
        res = 0;
    }
    PyBuffer_Release(&buf);
    return res;
}

PyDoc_STRVAR(tls_fileno__doc__, "\
fileno($self, /)\n\
--\n\n\
Returns file descriptor passed to constructor.");

static PyObject *
tls_fileno(struct Tls *self, PyObject *unused)
{
    LOG("TLS.fileno\n");
    return PyLong_FromLong(self->fd);
}

PyDoc_STRVAR(tls_shutdown__doc__, "\
shutdown($self, how, /)\n\
--\n\n\
Does nothing currently.");

static PyObject *
tls_shutdown(struct Tls *self, PyObject *unused)
{
    LOG("TLS.shutdown\n");
    Py_RETURN_NONE;
}

PyDoc_STRVAR(tls_close__doc__, "\
close($self, /)\n\
--\n\n\
Closes SSL connection and file descriptor.");

static PyObject *
tls_close(struct Tls *self, PyObject *unused)
{
    int rc, fd;
    /* TODO(jart): do nothing until we can figure out how to own fd */
    Py_RETURN_NONE;
    fd = self->fd;
    if (fd != -1) {
        self->fd = -1;
        rc = mbedtls_ssl_close_notify(&self->ssl);
        if (rc) {
            LOG("TLS CLOSING\n");
            close(fd);
            if (rc != -1) {
                SetTlsError(rc);
            }
            return 0;
        }
        LOG("TLS CLOSING\n");
        rc = close(fd);
        if (rc == -1) {
            return PyErr_SetFromErrno(PyExc_OSError);
        }
    }
    Py_RETURN_NONE;
}

PyDoc_STRVAR(tls_handshake__doc__, "\
handshake($self, /)\n\
--\n\n\
Handshakes SSL connection and file descriptor.");

static PyObject *
tls_handshake(struct Tls *self, PyObject *unused)
{
    int rc;
    LOG("TLS.handshake\n");
    rc = mbedtls_ssl_handshake(&self->ssl);
    if (rc) {
        LOG("TLS CLOSING\n");
        close(self->fd);
        self->fd = -1;
        if (rc != -1) {
            if (rc == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
                PyErr_SetString(TlsError, gc(DescribeSslVerifyFailure(self->ssl.session_negotiate->verify_result)));
            } else {
                SetTlsError(rc);
            }
        }
        return 0;
    }
    Py_RETURN_NONE;
}

static PyMethodDef tls_methods[] = {
    {"handshake", (PyCFunction)tls_handshake, METH_NOARGS,  tls_handshake__doc__},
    {"recv",      (PyCFunction)tls_recv,      METH_VARARGS, tls_recv__doc__},
    {"recv_into", (PyCFunction)tls_recv_into, METH_VARARGS, tls_recv_into__doc__},
    {"send",      (PyCFunction)tls_send,      METH_VARARGS, tls_send__doc__},
    {"sendall",   (PyCFunction)tls_sendall,   METH_VARARGS, tls_sendall__doc__},
    {"fileno",    (PyCFunction)tls_fileno,    METH_NOARGS,  tls_fileno__doc__},
    {"shutdown",  (PyCFunction)tls_shutdown,  METH_VARARGS, tls_shutdown__doc__},
    {"close",     (PyCFunction)tls_close,     METH_NOARGS,  tls_close__doc__},
    {0}
};

static PyObject *
tls_repr(struct Tls *self)
{
    return PyUnicode_FromFormat("<TLS object @ %p>", self);
}

PyDoc_STRVAR(tls_doc,
"An MbedTLS object.");

static PyTypeObject tls_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/ "tls.TLS",
    /*tp_basicsize*/ sizeof(struct Tls),
    /*tp_itemsize*/ 0,
    /*tp_dealloc*/ (destructor)tls_dealloc,
    /*tp_print*/ 0,
    /*tp_getattr*/ 0,
    /*tp_setattr*/ 0,
    /*tp_reserved*/ 0,
    /*tp_repr*/ (reprfunc)tls_repr,
    /*tp_as_number*/ 0,
    /*tp_as_sequence*/ 0,
    /*tp_as_mapping*/ 0,
    /*tp_hash*/ 0,
    /*tp_call*/ 0,
    /*tp_str*/ 0,
    /*tp_getattro*/ 0,
    /*tp_setattro*/ 0,
    /*tp_as_buffer*/ 0,
    /*tp_flags*/ Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    /*tp_doc*/ tls_doc,
    /*tp_traverse*/ 0,
    /*tp_clear*/ 0,
    /*tp_richcompare*/ 0,
    /*tp_weaklistoffset*/ 0,
    /*tp_iter*/ 0,
    /*tp_iternext*/ 0,
    /*tp_methods*/ tls_methods,
    /*tp_members*/ 0,
    /*tp_getset*/ 0,
    /*tp_base*/ 0,
    /*tp_dict*/ 0,
    /*tp_descr_get*/ 0,
    /*tp_descr_set*/ 0,
    /*tp_dictoffset*/ 0,
};

PyDoc_STRVAR(newclient__doc__,
"newclient($module, fd, host)\n\
--\n\n\
Creates TLS client.");

static PyObject *
newclient(PyObject *self, PyObject *args)
{
    int fd;
    PyObject *todo;
    struct Tls *tls;
    const char *host;
    if (!PyArg_ParseTuple(args, "isO:newclient", &fd, &host, &todo)) return 0;
    tls = tls_new(fd, host, todo);
    return (PyObject *)tls;
}

static struct PyMethodDef mbedtls_functions[] = {
    {"newclient", (PyCFunction)newclient, METH_VARARGS, newclient__doc__},
    {0}
};

static struct PyModuleDef mbedtls_module = {
    PyModuleDef_HEAD_INIT,
    "tls",
    NULL,
    -1,
    mbedtls_functions
};

PyMODINIT_FUNC
PyInit_tls(void)
{
    PyObject *m;
    Py_TYPE(&tls_type) = &PyType_Type;
    if (PyType_Ready(&tls_type) < 0) return 0;
    if (!(m = PyModule_Create(&mbedtls_module))) return 0;
    Py_INCREF((PyObject *)&tls_type);
    PyModule_AddObject(m, "TLS", (PyObject *)&tls_type);
    TlsError = PyErr_NewException("tls.TlsError", NULL, NULL);
    Py_INCREF(TlsError);
    PyModule_AddObject(m, "TlsError", TlsError);
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_tls = {
    "tls",
    PyInit_tls,
};
