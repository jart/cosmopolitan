/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "libc/calls/calls.h"
#include "libc/log/backtrace.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/md.h"
#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/pystrhex.h"
#include "third_party/python/Include/structmember.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Modules/hashlib.h"
/* clang-format off */

PYTHON_PROVIDE("_hashlib");
PYTHON_PROVIDE("_hashlib.HASH");
PYTHON_PROVIDE("_hashlib.__doc__");
PYTHON_PROVIDE("_hashlib.__loader__");
PYTHON_PROVIDE("_hashlib.__name__");
PYTHON_PROVIDE("_hashlib.__package__");
PYTHON_PROVIDE("_hashlib.__spec__");
PYTHON_PROVIDE("_hashlib.new");
PYTHON_PROVIDE("_hashlib.mbedtls_md5");
PYTHON_PROVIDE("_hashlib.mbedtls_md_meth_names");
PYTHON_PROVIDE("_hashlib.mbedtls_sha1");
PYTHON_PROVIDE("_hashlib.mbedtls_sha224");
PYTHON_PROVIDE("_hashlib.mbedtls_sha256");
PYTHON_PROVIDE("_hashlib.mbedtls_sha384");
PYTHON_PROVIDE("_hashlib.mbedtls_sha512");

#include "third_party/python/Modules/clinic/_hashmbedtls.inc"

/*[clinic input]
module _hashlib
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=c2b4ff081bac4be1]*/

#define MUNCH_SIZE 65536

#ifndef HASH_OBJ_CONSTRUCTOR
#define HASH_OBJ_CONSTRUCTOR 0
#endif

typedef struct {
    PyObject_HEAD
    PyObject            *name;
    mbedtls_md_context_t  ctx;
#ifdef WITH_THREAD
    PyThread_type_lock   lock;
#endif
} EVPobject;

static PyTypeObject EVPtype;

static PyObject *CONST_MD5_name_obj;
static PyObject *CONST_SHA1_name_obj;
static PyObject *CONST_SHA224_name_obj;
static PyObject *CONST_SHA256_name_obj;
static PyObject *CONST_SHA384_name_obj;
static PyObject *CONST_SHA512_name_obj;
static PyObject *CONST_BLAKE2B256_name_obj;

static PyObject *
SetMbedtlsError(PyObject *exc, int rc)
{
    char b[128];
    mbedtls_strerror(rc, b, sizeof(b));
    PyErr_SetString(exc, b);
    return NULL;
}

static EVPobject *
newEVPobject(PyObject *name)
{
    EVPobject *self;
    if ((self = PyObject_New(EVPobject, &EVPtype))) {
        mbedtls_md_init(&self->ctx);
        Py_INCREF(name);
        self->name = name;
#ifdef WITH_THREAD
        self->lock = 0;
#endif
    }
    return self;
}

static int
EVP_hash(EVPobject *self, const void *vp, Py_ssize_t len)
{
    int rc;
    unsigned int process;
    const unsigned char *cp;
    for (cp = vp; 0 < len; len -= process, cp += process) {
        if (len > MUNCH_SIZE) {
            process = MUNCH_SIZE;
        } else {
            process = Py_SAFE_DOWNCAST(len, Py_ssize_t, unsigned int);
        }
        if ((rc = mbedtls_md_update(&self->ctx, cp, process)) < 0) {
            return rc;
        }
    }
    return 0;
}

/* Internal methods for a hash object */

static void
EVP_dealloc(EVPobject *self)
{
#ifdef WITH_THREAD
    if (self->lock)
        PyThread_free_lock(self->lock);
#endif
    mbedtls_md_free(&self->ctx);
    Py_XDECREF(self->name);
    PyObject_Del(self);
}

static int
locked_mbedtls_md_clone(mbedtls_md_context_t *new_ctx_p, EVPobject *self)
{
    int rc;
    ENTER_HASHLIB(self);
    if (!(rc = mbedtls_md_setup(new_ctx_p, self->ctx.md_info, 0))) {
        rc = mbedtls_md_clone(new_ctx_p, &self->ctx);
    }
    LEAVE_HASHLIB(self);
    return rc;
}

/* External methods for a hash object */

PyDoc_STRVAR(EVP_copy__doc__, "Return a copy of the hash object.");

static PyObject *
EVP_copy(EVPobject *self, PyObject *unused)
{
    int rc;
    EVPobject *newobj;
    if ((newobj = newEVPobject(self->name))) {
        if ((rc = locked_mbedtls_md_clone(&newobj->ctx, self))) {
            EVP_dealloc(newobj);
            return SetMbedtlsError(PyExc_ValueError, rc);
        }
    }
    return (PyObject *)newobj;
}

PyDoc_STRVAR(EVP_digest__doc__,
"Return the digest value as a bytes object.");

static PyObject *
EVP_digest(EVPobject *self, PyObject *unused)
{
    int rc;
    PyObject *retval;
    unsigned int digest_size;
    mbedtls_md_context_t temp_ctx;
    unsigned char digest[MBEDTLS_MD_MAX_SIZE];
    mbedtls_md_init(&temp_ctx);
    if (!(rc = locked_mbedtls_md_clone(&temp_ctx, self))) {
        digest_size = mbedtls_md_get_size(temp_ctx.md_info);
        if (!(rc = mbedtls_md_finish(&temp_ctx, digest))) {
            retval = PyBytes_FromStringAndSize((const char *)digest, digest_size);
        } else {
            retval = SetMbedtlsError(PyExc_ValueError, rc);
        }
    } else {
        retval = SetMbedtlsError(PyExc_ValueError, rc);
    }
    mbedtls_md_free(&temp_ctx);
    return retval;
}

PyDoc_STRVAR(EVP_hexdigest__doc__,
"Return the digest value as a string of hexadecimal digits.");

static PyObject *
EVP_hexdigest(EVPobject *self, PyObject *unused)
{
    int rc;
    PyObject *retval;
    unsigned int digest_size;
    mbedtls_md_context_t temp_ctx;
    unsigned char digest[MBEDTLS_MD_MAX_SIZE];
    mbedtls_md_init(&temp_ctx);
    if (!(rc = locked_mbedtls_md_clone(&temp_ctx, self))) {
        digest_size = mbedtls_md_get_size(temp_ctx.md_info);
        if (!(rc = mbedtls_md_finish(&temp_ctx, digest))) {
            retval = _Py_strhex((const char *)digest, digest_size);
        } else {
            retval = SetMbedtlsError(PyExc_ValueError, rc);
        }
    } else {
        retval = SetMbedtlsError(PyExc_ValueError, rc);
    }
    mbedtls_md_free(&temp_ctx);
    return retval;
}

PyDoc_STRVAR(EVP_update__doc__,
"Update this hash object's state with the provided string.");

static PyObject *
EVP_update(EVPobject *self, PyObject *args)
{
    PyObject *obj;
    Py_buffer view;
    if (!PyArg_ParseTuple(args, "O:update", &obj)) return 0;
    GET_BUFFER_VIEW_OR_ERROUT(obj, &view);
    EVP_hash(self, view.buf, view.len);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyMethodDef EVP_methods[] = {
    {"update",    (PyCFunction)EVP_update,    METH_VARARGS, EVP_update__doc__},
    {"digest",    (PyCFunction)EVP_digest,    METH_NOARGS,  EVP_digest__doc__},
    {"hexdigest", (PyCFunction)EVP_hexdigest, METH_NOARGS,  EVP_hexdigest__doc__},
    {"copy",      (PyCFunction)EVP_copy,      METH_NOARGS,  EVP_copy__doc__},
    {NULL, NULL}  /* sentinel */
};

static PyObject *
EVP_get_block_size(EVPobject *self, void *closure)
{
    return PyLong_FromLong(mbedtls_md_get_block_size(self->ctx.md_info));
}

static PyObject *
EVP_get_digest_size(EVPobject *self, void *closure)
{
    return PyLong_FromLong(mbedtls_md_get_size(self->ctx.md_info));
}

static PyMemberDef EVP_members[] = {
    {"name", T_OBJECT, offsetof(EVPobject, name), READONLY, PyDoc_STR("algorithm name.")},
    {NULL}
};

static PyGetSetDef EVP_getseters[] = {
    {"digest_size", (getter)EVP_get_digest_size, NULL, NULL, NULL},
    {"block_size",  (getter)EVP_get_block_size,  NULL, NULL, NULL},
    {NULL}
};

static PyObject *
EVP_repr(EVPobject *self)
{
    return PyUnicode_FromFormat("<%U HASH object @ %p>", self->name, self);
}

#if HASH_OBJ_CONSTRUCTOR
#error wut
static int
EVP_tp_init(EVPobject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", "string", NULL};
    PyObject *name_obj = NULL;
    PyObject *data_obj = NULL;
    Py_buffer view;
    char *nameStr;
    const mbedtls_md_info_t *digest;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O:HASH", kwlist,
                                     &name_obj, &data_obj)) {
        return -1;
    }
    if (data_obj)
        GET_BUFFER_VIEW_OR_ERROUT(data_obj, &view);
    if (!PyArg_Parse(name_obj, "s", &nameStr)) {
        PyErr_SetString(PyExc_TypeError, "name must be a string");
        if (data_obj)
            PyBuffer_Release(&view);
        return -1;
    }
    digest = mbedtls_md_info_from_string(nameStr);
    if (!digest) {
        PyErr_SetString(PyExc_ValueError, "unknown hash function");
        if (data_obj)
            PyBuffer_Release(&view);
        return -1;
    }
    if (!EVP_DigestInit(self->ctx, digest)) {
        SetMbedtlsError(PyExc_ValueError);
        if (data_obj)
            PyBuffer_Release(&view);
        return -1;
    }
    Py_INCREF(name_obj);
    Py_XSETREF(self->name, name_obj);
    if (data_obj) {
        if (view.len >= HASHLIB_GIL_MINSIZE) {
            Py_BEGIN_ALLOW_THREADS
            EVP_hash(self, view.buf, view.len);
            Py_END_ALLOW_THREADS
        } else {
            EVP_hash(self, view.buf, view.len);
        }
        PyBuffer_Release(&view);
    }
    return 0;
}
#endif

PyDoc_STRVAR(hashtype_doc,
"A hash represents the object used to calculate a checksum of a\n\
string of information.\n\
\n\
Methods:\n\
\n\
update() -- updates the current digest with an additional string\n\
digest() -- return the current digest value\n\
hexdigest() -- return the current digest as a string of hexadecimal digits\n\
copy() -- return a copy of the current hash object\n\
\n\
Attributes:\n\
\n\
name -- the hash algorithm being used by this object\n\
digest_size -- number of bytes in this hashes output\n");

static PyTypeObject EVPtype = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_hashlib.HASH",    /*tp_name*/
    sizeof(EVPobject),  /*tp_basicsize*/
    0,                  /*tp_itemsize*/
    /* methods */
    (destructor)EVP_dealloc, /*tp_dealloc*/
    0,                  /*tp_print*/
    0,                  /*tp_getattr*/
    0,                  /*tp_setattr*/
    0,                  /*tp_reserved*/
    (reprfunc)EVP_repr, /*tp_repr*/
    0,                  /*tp_as_number*/
    0,                  /*tp_as_sequence*/
    0,                  /*tp_as_mapping*/
    0,                  /*tp_hash*/
    0,                  /*tp_call*/
    0,                  /*tp_str*/
    0,                  /*tp_getattro*/
    0,                  /*tp_setattro*/
    0,                  /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    hashtype_doc,       /*tp_doc*/
    0,                  /*tp_traverse*/
    0,                  /*tp_clear*/
    0,                  /*tp_richcompare*/
    0,                  /*tp_weaklistoffset*/
    0,                  /*tp_iter*/
    0,                  /*tp_iternext*/
    EVP_methods,        /* tp_methods */
    EVP_members,        /* tp_members */
    EVP_getseters,      /* tp_getset */
#if 1
    0,                  /* tp_base */
    0,                  /* tp_dict */
    0,                  /* tp_descr_get */
    0,                  /* tp_descr_set */
    0,                  /* tp_dictoffset */
#endif
#if HASH_OBJ_CONSTRUCTOR
    (initproc)EVP_tp_init, /* tp_init */
#endif
};

static PyObject *
EVPnew(PyObject *name_obj,
       const mbedtls_md_info_t *digest,
       const unsigned char *cp, Py_ssize_t len)
{
    int rc;
    EVPobject *self;
    if (!digest) {
        PyErr_SetString(PyExc_ValueError, "unsupported hash type");
        return NULL;
    }
    if ((self = newEVPobject(name_obj)) == NULL)
        return NULL;
    if ((rc = mbedtls_md_setup(&self->ctx, digest, 0))) {
        SetMbedtlsError(PyExc_ValueError, rc);
        Py_DECREF(self);
        return NULL;
    }
    if (cp && len) {
        if (len >= HASHLIB_GIL_MINSIZE) {
            Py_BEGIN_ALLOW_THREADS
            EVP_hash(self, cp, len);
            Py_END_ALLOW_THREADS
        } else {
            EVP_hash(self, cp, len);
        }
    }
    return (PyObject *)self;
}


/* The module-level function: new() */

PyDoc_STRVAR(EVP_new__doc__,
"Return a new hash object using the named algorithm.\n\
An optional string argument may be provided and will be\n\
automatically hashed.\n\
\n\
The MD5 and SHA1 algorithms are always supported.\n");

static PyObject *
EVP_new(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"name", "string", NULL};
    PyObject *name_obj = NULL;
    PyObject *data_obj = NULL;
    Py_buffer view = { 0 };
    PyObject *ret_obj;
    char *name;
    const mbedtls_md_info_t *digest;
    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "O|O:new", kwlist,
                                     &name_obj, &data_obj)) {
        return NULL;
    }
    if (!PyArg_Parse(name_obj, "s", &name)) {
        PyErr_SetString(PyExc_TypeError, "name must be a string");
        return NULL;
    }
    if (data_obj)
        GET_BUFFER_VIEW_OR_ERROUT(data_obj, &view);
    digest = mbedtls_md_info_from_string(name);
    ret_obj = EVPnew(name_obj, digest, (unsigned char*)view.buf, view.len);
    if (data_obj)
        PyBuffer_Release(&view);
    return ret_obj;
}

#if (MBEDTLS_VERSION_NUMBER >= 0x10000000 && !defined(MBEDTLS_NO_HMAC) \
     && !defined(MBEDTLS_NO_SHA))
#define PY_PBKDF2_HMAC 1
#if !HAS_FAST_PKCS5_PBKDF2_HMAC
/* Improved implementation of PKCS5_PBKDF2_HMAC()
 *
 * PKCS5_PBKDF2_HMAC_fast() hashes the password exactly one time instead of
 * `iter` times. Today (2013) the iteration count is typically 100,000 or
 * more. The improved algorithm is not subject to a Denial-of-Service
 * vulnerability with overly large passwords.
 *
 * Also Mbedtls < 1.0 don't provide PKCS5_PBKDF2_HMAC(), only
 * PKCS5_PBKDF2_SHA1.
 */
static int
PKCS5_PBKDF2_HMAC_fast(const char *pass, int passlen,
                       const unsigned char *salt, int saltlen,
                       int iter, const mbedtls_md_info_t *digest,
                       int keylen, unsigned char *out)
{
    unsigned char digtmp[MBEDTLS_MD_MAX_SIZE], *p, itmp[4];
    int cplen, j, k, tkeylen, mdlen;
    unsigned long i = 1;
    HMAC_CTX hctx_tpl, hctx;
    mdlen = mbedtls_md_get_size(digest);
    if (mdlen < 0)
        return 0;
    HMAC_CTX_init(&hctx_tpl);
    HMAC_CTX_init(&hctx);
    p = out;
    tkeylen = keylen;
    if (!HMAC_Init_ex(&hctx_tpl, pass, passlen, digest, NULL)) {
        HMAC_CTX_cleanup(&hctx_tpl);
        return 0;
    }
    while (tkeylen) {
        if (tkeylen > mdlen)
            cplen = mdlen;
        else
            cplen = tkeylen;
        /* We are unlikely to ever use more than 256 blocks (5120 bits!)
         * but just in case...
         */
        itmp[0] = (unsigned char)((i >> 24) & 0xff);
        itmp[1] = (unsigned char)((i >> 16) & 0xff);
        itmp[2] = (unsigned char)((i >> 8) & 0xff);
        itmp[3] = (unsigned char)(i & 0xff);
        if (!HMAC_CTX_copy(&hctx, &hctx_tpl)) {
            HMAC_CTX_cleanup(&hctx_tpl);
            return 0;
        }
        if (!HMAC_Update(&hctx, salt, saltlen)
                || !HMAC_Update(&hctx, itmp, 4)
                || !HMAC_Final(&hctx, digtmp, NULL)) {
            HMAC_CTX_cleanup(&hctx_tpl);
            HMAC_CTX_cleanup(&hctx);
            return 0;
        }
        HMAC_CTX_cleanup(&hctx);
        memcpy(p, digtmp, cplen);
        for (j = 1; j < iter; j++) {
            if (!HMAC_CTX_copy(&hctx, &hctx_tpl)) {
                HMAC_CTX_cleanup(&hctx_tpl);
                return 0;
            }
            if (!HMAC_Update(&hctx, digtmp, mdlen)
                    || !HMAC_Final(&hctx, digtmp, NULL)) {
                HMAC_CTX_cleanup(&hctx_tpl);
                HMAC_CTX_cleanup(&hctx);
                return 0;
            }
            HMAC_CTX_cleanup(&hctx);
            for (k = 0; k < cplen; k++) {
                p[k] ^= digtmp[k];
            }
        }
        tkeylen-= cplen;
        i++;
        p+= cplen;
    }
    HMAC_CTX_cleanup(&hctx_tpl);
    return 1;
}
#endif

PyDoc_STRVAR(pbkdf2_hmac__doc__,
"pbkdf2_hmac(hash_name, password, salt, iterations, dklen=None) -> key\n\
\n\
Password based key derivation function 2 (PKCS #5 v2.0) with HMAC as\n\
pseudorandom function.");

static PyObject *
pbkdf2_hmac(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {"hash_name", "password", "salt", "iterations",
                             "dklen", NULL};
    PyObject *key_obj = NULL, *dklen_obj = Py_None;
    char *name, *key;
    Py_buffer password, salt;
    long iterations, dklen;
    int retval;
    const mbedtls_md_info_t *digest;
    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "sy*y*l|O:pbkdf2_hmac",
                                     kwlist, &name, &password, &salt,
                                     &iterations, &dklen_obj)) {
        return NULL;
    }
    digest = mbedtls_md_info_from_string(name);
    if (digest == NULL) {
        PyErr_SetString(PyExc_ValueError, "unsupported hash type");
        goto end;
    }
    if (password.len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,
                        "password is too long.");
        goto end;
    }
    if (salt.len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,
                        "salt is too long.");
        goto end;
    }
    if (iterations < 1) {
        PyErr_SetString(PyExc_ValueError,
                        "iteration value must be greater than 0.");
        goto end;
    }
    if (iterations > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,
                        "iteration value is too great.");
        goto end;
    }
    if (dklen_obj == Py_None) {
        dklen = mbedtls_md_get_size(digest);
    } else {
        dklen = PyLong_AsLong(dklen_obj);
        if ((dklen == -1) && PyErr_Occurred()) {
            goto end;
        }
    }
    if (dklen < 1) {
        PyErr_SetString(PyExc_ValueError,
                        "key length must be greater than 0.");
        goto end;
    }
    if (dklen > INT_MAX) {
        /* INT_MAX is always smaller than dkLen max (2^32 - 1) * hLen */
        PyErr_SetString(PyExc_OverflowError,
                        "key length is too great.");
        goto end;
    }
    key_obj = PyBytes_FromStringAndSize(NULL, dklen);
    if (key_obj == NULL) {
        goto end;
    }
    key = PyBytes_AS_STRING(key_obj);
    Py_BEGIN_ALLOW_THREADS
#if HAS_FAST_PKCS5_PBKDF2_HMAC
    retval = PKCS5_PBKDF2_HMAC((char*)password.buf, (int)password.len,
                               (unsigned char *)salt.buf, (int)salt.len,
                               iterations, digest, dklen,
                               (unsigned char *)key);
#else
    retval = PKCS5_PBKDF2_HMAC_fast((char*)password.buf, (int)password.len,
                                    (unsigned char *)salt.buf, (int)salt.len,
                                    iterations, digest, dklen,
                                    (unsigned char *)key);
#endif
    Py_END_ALLOW_THREADS
    if (retval) {
        Py_CLEAR(key_obj);
        SetMbedtlsError(PyExc_ValueError, retval);
        goto end;
    }
  end:
    PyBuffer_Release(&password);
    PyBuffer_Release(&salt);
    return key_obj;
}
#endif

#if MBEDTLS_VERSION_NUMBER > 0x10100000L && !defined(MBEDTLS_NO_SCRYPT) && !defined(LIBRESSL_VERSION_NUMBER)
#define PY_SCRYPT 1
/* XXX: Parameters salt, n, r and p should be required keyword-only parameters.
   They are optional in the Argument Clinic declaration only due to a
   limitation of PyArg_ParseTupleAndKeywords. */

/*[clinic input]
_hashlib.scrypt

    password: Py_buffer
    *
    salt: Py_buffer = None
    n as n_obj: object(subclass_of='&PyLong_Type') = None
    r as r_obj: object(subclass_of='&PyLong_Type') = None
    p as p_obj: object(subclass_of='&PyLong_Type') = None
    maxmem: long = 0
    dklen: long = 64


scrypt password-based key derivation function.
[clinic start generated code]*/
static PyObject *
_hashlib_scrypt_impl(PyObject *module, Py_buffer *password, Py_buffer *salt,
                     PyObject *n_obj, PyObject *r_obj, PyObject *p_obj,
                     long maxmem, long dklen)
/*[clinic end generated code: output=14849e2aa2b7b46c input=48a7d63bf3f75c42]*/
{
    PyObject *key_obj = NULL;
    char *key;
    int retval;
    unsigned long n, r, p;
    if (password->len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,
                        "password is too long.");
        return NULL;
    }
    if (salt->buf == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "salt is required");
        return NULL;
    }
    if (salt->len > INT_MAX) {
        PyErr_SetString(PyExc_OverflowError,
                        "salt is too long.");
        return NULL;
    }
    n = PyLong_AsUnsignedLong(n_obj);
    if (n == (unsigned long) -1 && PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,
                        "n is required and must be an unsigned int");
        return NULL;
    }
    if (n < 2 || n & (n - 1)) {
        PyErr_SetString(PyExc_ValueError,
                        "n must be a power of 2.");
        return NULL;
    }
    r = PyLong_AsUnsignedLong(r_obj);
    if (r == (unsigned long) -1 && PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,
                         "r is required and must be an unsigned int");
        return NULL;
    }
    p = PyLong_AsUnsignedLong(p_obj);
    if (p == (unsigned long) -1 && PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError,
                         "p is required and must be an unsigned int");
        return NULL;
    }
    if (maxmem < 0 || maxmem > INT_MAX) {
        /* Mbedtls 1.1.0 restricts maxmem to 32MB. It may change in the
           future. The maxmem constant is private to Mbedtls. */
        PyErr_Format(PyExc_ValueError,
                     "maxmem must be positive and smaller than %d",
                      INT_MAX);
        return NULL;
    }
    if (dklen < 1 || dklen > INT_MAX) {
        PyErr_Format(PyExc_ValueError,
                    "dklen must be greater than 0 and smaller than %d",
                    INT_MAX);
        return NULL;
    }
    /* let Mbedtls validate the rest */
    retval = EVP_PBE_scrypt(NULL, 0, NULL, 0, n, r, p, maxmem, NULL, 0);
    if (!retval) {
        /* sorry, can't do much better */
        PyErr_SetString(PyExc_ValueError,
                        "Invalid paramemter combination for n, r, p, maxmem.");
        return NULL;
   }
    key_obj = PyBytes_FromStringAndSize(NULL, dklen);
    if (key_obj == NULL) {
        return NULL;
    }
    key = PyBytes_AS_STRING(key_obj);
    Py_BEGIN_ALLOW_THREADS
    retval = EVP_PBE_scrypt(
        (const char*)password->buf, (size_t)password->len,
        (const unsigned char *)salt->buf, (size_t)salt->len,
        n, r, p, maxmem,
        (unsigned char *)key, (size_t)dklen
    );
    Py_END_ALLOW_THREADS
    if (!retval) {
        Py_CLEAR(key_obj);
        SetMbedtlsError(PyExc_ValueError);
        return NULL;
    }
    return key_obj;
}
#endif

static PyObject *
GenerateHashNameList(void)
{
    int i;
    char *s;
    uint8_t *p;
    PyObject *set, *name;
    if ((set = PyFrozenSet_New(0))) {
        for (p = mbedtls_md_list(); *p != MBEDTLS_MD_NONE; ++p) {
            s = strdup(mbedtls_md_info_from_type(*p)->name);
            for (i = 0; s[i]; ++i) s[i] = tolower(s[i]);
            name = PyUnicode_FromString(s);
            PySet_Add(set, name);
            Py_DECREF(name);
            free(s);
        }
    }
    return set;
}

/*
 *  This macro generates constructor function definitions for specific
 *  hash algorithms.  These constructors are much faster than calling
 *  the generic one passing it a python string and are noticeably
 *  faster than calling a python new() wrapper.  That is important for
 *  code that wants to make hashes of a bunch of small strings.
 *  The first call will lazy-initialize, which reports an exception
 *  if initialization fails.
 */
#define GEN_CONSTRUCTOR(NAME, STRNAME)                                  \
    static PyObject *                                                   \
    EVP_new_ ## NAME (PyObject *self, PyObject *args)                   \
    {                                                                   \
        PyObject *ret;                                                  \
        PyObject *data = 0;                                             \
        Py_buffer view = { 0 };                                         \
        if (!PyArg_ParseTuple(args, "|O:" STRNAME , &data)) return 0;   \
        if (data) GET_BUFFER_VIEW_OR_ERROUT(data, &view);               \
        ret = EVPnew(CONST_ ## NAME ## _name_obj,                       \
                     mbedtls_md_info_from_type(MBEDTLS_MD_ ## NAME),    \
                     (unsigned char *)view.buf, view.len);              \
        if (data) PyBuffer_Release(&view);                              \
        return ret;                                                     \
    }

/* a PyMethodDef structure for the constructor */
#define CONSTRUCTOR_METH_DEF(NAME, STRNAME)                            \
    {"mbedtls_" STRNAME, (PyCFunction)EVP_new_ ## NAME, METH_VARARGS,  \
        PyDoc_STR("Returns a " STRNAME \
                  " hash object; optionally initialized with a string") \
    }

/* used in the init function to setup a constructor: initialize Mbedtls
   constructor constants if they haven't been initialized already.  */
#define INIT_CONSTRUCTOR_CONSTANTS(NAME, STRNAME)                       \
    if (CONST_ ## NAME ## _name_obj == NULL) {                          \
        CONST_ ## NAME ## _name_obj = PyUnicode_FromString(#NAME);      \
    }

GEN_CONSTRUCTOR(MD5, "md5")
GEN_CONSTRUCTOR(SHA1, "sha1")
GEN_CONSTRUCTOR(SHA224, "sha224")
GEN_CONSTRUCTOR(SHA256, "sha256")
GEN_CONSTRUCTOR(SHA384, "sha384")
GEN_CONSTRUCTOR(SHA512, "sha512")
GEN_CONSTRUCTOR(BLAKE2B256, "blake2b256")

/* List of functions exported by this module */

static struct PyMethodDef EVP_functions[] = {
    {"new", (PyCFunction)EVP_new, METH_VARARGS|METH_KEYWORDS, EVP_new__doc__},
#ifdef PY_PBKDF2_HMAC
    {"pbkdf2_hmac", (PyCFunction)pbkdf2_hmac, METH_VARARGS|METH_KEYWORDS,
     pbkdf2_hmac__doc__},
#endif
    _HASHLIB_SCRYPT_METHODDEF
    CONSTRUCTOR_METH_DEF(MD5, "md5"),
    CONSTRUCTOR_METH_DEF(SHA1, "sha1"),
    CONSTRUCTOR_METH_DEF(SHA224, "sha224"),
    CONSTRUCTOR_METH_DEF(SHA256, "sha256"),
    CONSTRUCTOR_METH_DEF(SHA384, "sha384"),
    CONSTRUCTOR_METH_DEF(SHA512, "sha512"),
    CONSTRUCTOR_METH_DEF(BLAKE2B256, "blake2b256"),
    {NULL}
};

static struct PyModuleDef _hashlibmodule = {
    PyModuleDef_HEAD_INIT,
    "_hashlib",
    NULL,
    -1,
    EVP_functions,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__hashlib(void)
{
    PyObject *m, *mbedtls_md_meth_names;
    Py_TYPE(&EVPtype) = &PyType_Type;
    if (PyType_Ready(&EVPtype) < 0)
        return NULL;
    if (!(m = PyModule_Create(&_hashlibmodule)))
        return NULL;
    if (!(mbedtls_md_meth_names = GenerateHashNameList())) {
        Py_DECREF(m);
        return NULL;
    }
    if (PyModule_AddObject(m, "mbedtls_md_meth_names", mbedtls_md_meth_names)) {
        Py_DECREF(m);
        return NULL;
    }
    Py_INCREF((PyObject *)&EVPtype);
    PyModule_AddObject(m, "HASH", (PyObject *)&EVPtype);
    INIT_CONSTRUCTOR_CONSTANTS(MD5, "md5")
    INIT_CONSTRUCTOR_CONSTANTS(SHA1, "sha1")
    INIT_CONSTRUCTOR_CONSTANTS(SHA224, "sha224")
    INIT_CONSTRUCTOR_CONSTANTS(SHA256, "sha256")
    INIT_CONSTRUCTOR_CONSTANTS(SHA384, "sha384")
    INIT_CONSTRUCTOR_CONSTANTS(SHA512, "sha512")
    INIT_CONSTRUCTOR_CONSTANTS(BLAKE2B256, "blake2b256")
    return m;
}

_Section(".rodata.pytab.1") const struct _inittab _PyImport_Inittab__hashlib = {
    "_hashlib",
    PyInit__hashlib,
};
