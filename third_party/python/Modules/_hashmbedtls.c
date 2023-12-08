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
#include "libc/calls/calls.h"
#include "libc/log/backtrace.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/mbedtls/error.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/pkcs5.h"
#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pystrhex.h"
#include "third_party/python/Include/structmember.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Modules/hashlib.h"

PYTHON_PROVIDE("_hashlib");
PYTHON_PROVIDE("_hashlib.HASH");
PYTHON_PROVIDE("_hashlib.__doc__");
PYTHON_PROVIDE("_hashlib.__loader__");
PYTHON_PROVIDE("_hashlib.__name__");
PYTHON_PROVIDE("_hashlib.__package__");
PYTHON_PROVIDE("_hashlib.__spec__");
PYTHON_PROVIDE("_hashlib.new");
PYTHON_PROVIDE("_hashlib.pbkdf2_hmac");
PYTHON_PROVIDE("_hashlib.mbedtls_md5");
PYTHON_PROVIDE("_hashlib.mbedtls_md_meth_names");
PYTHON_PROVIDE("_hashlib.mbedtls_sha1");
PYTHON_PROVIDE("_hashlib.mbedtls_sha224");
PYTHON_PROVIDE("_hashlib.mbedtls_sha256");
PYTHON_PROVIDE("_hashlib.mbedtls_sha384");
PYTHON_PROVIDE("_hashlib.mbedtls_sha512");

struct Hasher {
    PyObject_HEAD
    const PyObject       *name;
    mbedtls_md_context_t  ctx;
#ifdef WITH_THREAD
    PyThread_type_lock    lock;
#endif
};

static PyTypeObject hasher_type;
static const PyObject *CONST_MD5_name_obj;
static const PyObject *CONST_SHA1_name_obj;
static const PyObject *CONST_SHA224_name_obj;
static const PyObject *CONST_SHA256_name_obj;
static const PyObject *CONST_SHA384_name_obj;
static const PyObject *CONST_SHA512_name_obj;
static const PyObject *CONST_BLAKE2B256_name_obj;

static PyObject *
SetMbedtlsError(PyObject *exc, int rc)
{
    char b[128];
    stpcpy(b, "MBEDTLS - ");
    mbedtls_strerror(rc, b + 10, sizeof(b) - 10);
    PyErr_SetString(exc, b);
    return NULL;
}

static struct Hasher *
hasher_new(const PyObject *name)
{
    struct Hasher *self;
    if ((self = PyObject_New(struct Hasher, &hasher_type))) {
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
hasher_hash(struct Hasher *self, const void *p, Py_ssize_t n)
{
    return mbedtls_md_update(&self->ctx, p, n);
}

static void
hasher_dealloc(struct Hasher *self)
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
mbedtls_md_clone_locked(mbedtls_md_context_t *new_ctx_p, struct Hasher *self)
{
    int rc;
    ENTER_HASHLIB(self);
    if (!(rc = mbedtls_md_setup(new_ctx_p, self->ctx.md_info, 0))) {
        rc = mbedtls_md_clone(new_ctx_p, &self->ctx);
    }
    LEAVE_HASHLIB(self);
    return rc;
}

PyDoc_STRVAR(hashlib_copy__doc__, "\
copy($self, /)\n\
--\n\
\n\
Return a copy of the hash object.");

static PyObject *
hashlib_copy(struct Hasher *self, PyObject *unused)
{
    int rc;
    struct Hasher *newobj;
    if ((newobj = hasher_new(self->name))) {
        if ((rc = mbedtls_md_clone_locked(&newobj->ctx, self))) {
            hasher_dealloc(newobj);
            return SetMbedtlsError(PyExc_ValueError, rc);
        }
    }
    return (PyObject *)newobj;
}

PyDoc_STRVAR(hashlib_digest__doc__, "\
digest($self, /)\n\
--\n\
\n\
Return the digest value as a bytes object.");

static PyObject *
hashlib_digest(struct Hasher *self, PyObject *unused)
{
    int rc;
    PyObject *retval;
    unsigned int digest_size;
    mbedtls_md_context_t temp_ctx;
    unsigned char digest[MBEDTLS_MD_MAX_SIZE];
    mbedtls_md_init(&temp_ctx);
    if (!(rc = mbedtls_md_clone_locked(&temp_ctx, self))) {
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

PyDoc_STRVAR(hashlib_hexdigest__doc__, "\
hexdigest($self, /)\n\
--\n\
\n\
Return the digest value as a string of hexadecimal digits.");

static PyObject *
hashlib_hexdigest(struct Hasher *self, PyObject *unused)
{
    int rc;
    PyObject *retval;
    unsigned int digest_size;
    mbedtls_md_context_t temp_ctx;
    unsigned char digest[MBEDTLS_MD_MAX_SIZE];
    mbedtls_md_init(&temp_ctx);
    if (!(rc = mbedtls_md_clone_locked(&temp_ctx, self))) {
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

PyDoc_STRVAR(hashlib_update__doc__, "\
update($self, bytes, /)\n\
--\n\
\n\
Update this hash object's state with the provided string.");

static PyObject *
hashlib_update(struct Hasher *self, PyObject *args)
{
    Py_buffer data;
    if (!PyArg_ParseTuple(args, "y*:update", &data)) return 0;
    hasher_hash(self, data.buf, data.len);
    PyBuffer_Release(&data);
    Py_RETURN_NONE;
}

static PyMethodDef hashlib_methods[] = {
    {"update",    (PyCFunction)hashlib_update,    METH_VARARGS, hashlib_update__doc__},
    {"digest",    (PyCFunction)hashlib_digest,    METH_NOARGS,  hashlib_digest__doc__},
    {"hexdigest", (PyCFunction)hashlib_hexdigest, METH_NOARGS,  hashlib_hexdigest__doc__},
    {"copy",      (PyCFunction)hashlib_copy,      METH_NOARGS,  hashlib_copy__doc__},
    {0}
};

static PyObject *
hashlib_get_block_size(struct Hasher *self, void *closure)
{
    return PyLong_FromLong(mbedtls_md_get_block_size(self->ctx.md_info));
}

static PyObject *
hashlib_get_digest_size(struct Hasher *self, void *closure)
{
    return PyLong_FromLong(mbedtls_md_get_size(self->ctx.md_info));
}

static PyMemberDef hashlib_members[] = {
    {"name", T_OBJECT, offsetof(struct Hasher, name), READONLY, PyDoc_STR("algorithm name.")},
    {0}
};

static PyGetSetDef hashlib_getseters[] = {
    {"digest_size", (getter)hashlib_get_digest_size, NULL, NULL, NULL},
    {"block_size",  (getter)hashlib_get_block_size,  NULL, NULL, NULL},
    {0}
};

static PyObject *
hasher_repr(struct Hasher *self)
{
    return PyUnicode_FromFormat("<%U HASH object @ %p>", self->name, self);
}

PyDoc_STRVAR(hashtype_doc,
"A hash represents the object used to calculate a checksum of a\n\
string of information.\n\
\n\
Attributes:\n\
\n\
name -- the hash algorithm being used by this object\n\
digest_size -- number of bytes in this hashes output\n");

static PyTypeObject hasher_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    /*tp_name*/ "_hashlib.HASH",
    /*tp_basicsize*/ sizeof(struct Hasher),
    /*tp_itemsize*/ 0,
    /*tp_dealloc*/ (destructor)hasher_dealloc,
    /*tp_print*/ 0,
    /*tp_getattr*/ 0,
    /*tp_setattr*/ 0,
    /*tp_reserved*/ 0,
    /*tp_repr*/ (reprfunc)hasher_repr,
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
    /*tp_doc*/ hashtype_doc,
    /*tp_traverse*/ 0,
    /*tp_clear*/ 0,
    /*tp_richcompare*/ 0,
    /*tp_weaklistoffset*/ 0,
    /*tp_iter*/ 0,
    /*tp_iternext*/ 0,
    /*tp_methods*/ hashlib_methods,
    /*tp_members*/ hashlib_members,
    /*tp_getset*/ hashlib_getseters,
    /*tp_base*/ 0,
    /*tp_dict*/ 0,
    /*tp_descr_get*/ 0,
    /*tp_descr_set*/ 0,
    /*tp_dictoffset*/ 0,
};

static PyObject *
NewHasher(const PyObject *name_obj,
          const mbedtls_md_info_t *digest,
          void *p, Py_ssize_t n)
{
    int rc;
    struct Hasher *self;
    if (!digest) {
        PyErr_SetString(PyExc_ValueError, "unsupported hash type");
        return NULL;
    }
    if (!(self = hasher_new(name_obj))) return 0;
    if ((rc = mbedtls_md_setup(&self->ctx, digest, 0)) ||
        (rc = mbedtls_md_starts(&self->ctx))) {
        SetMbedtlsError(PyExc_ValueError, rc);
        Py_DECREF(self);
        return NULL;
    }
    if (n) {
        if (n >= HASHLIB_GIL_MINSIZE) {
            Py_BEGIN_ALLOW_THREADS
            hasher_hash(self, p, n);
            Py_END_ALLOW_THREADS
        } else {
            hasher_hash(self, p, n);
        }
    }
    return (PyObject *)self;
}

PyDoc_STRVAR(hashlib_new__doc__,
"new($module, name, string=b'')\n\
--\n\
\n\
Return a new hash object using the named algorithm.\n\
An optional string argument may be provided and will be\n\
automatically hashed.\n\
\n\
The MD5 and SHA1 algorithms are always supported.\n");

static PyObject *
hashlib_new(PyObject *self, PyObject *args, PyObject *kwdict)
{
    char *name;
    PyObject *res;
    Py_buffer data = {0};
    PyObject *name_obj = 0;
    static char *kwlist[] = {"name", "string", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "O|y*:new", kwlist,
                                     &name_obj, &data)) {
        return NULL;
    }
    if (!PyArg_Parse(name_obj, "s", &name)) {
        PyErr_SetString(PyExc_TypeError, "name must be a string");
        PyBuffer_Release(&data);
        return NULL;
    }
    res = NewHasher(name_obj, mbedtls_md_info_from_string(name),
                    data.buf, data.len);
    PyBuffer_Release(&data);
    return res;
}

static int
pbkdf2(const mbedtls_md_info_t *digest,
       const void *pass, size_t passlen,
       const void *salt, size_t saltlen,
       size_t c, size_t dklen, void *dk)
{
    int rc;
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    if (!(rc = mbedtls_md_setup(&ctx, digest, 1))) {
        rc = mbedtls_pkcs5_pbkdf2_hmac(
            &ctx, pass, passlen, salt, saltlen, c, dklen, dk);
    }
    mbedtls_md_free(&ctx);
    return rc;
}

PyDoc_STRVAR(pbkdf2_hmac__doc__,
"pbkdf2_hmac($module, hash_name, password, salt, iterations, dklen=None)\n\
--\n\
\n\
Password based key derivation function 2 (PKCS #5 v2.o) with HMAC as\n\
pseudorandom function.");

static PyObject *
pbkdf2_hmac(PyObject *self, PyObject *args, PyObject *kwdict)
{
    static char *kwlist[] = {
        "hash_name",
        "password",
        "salt",
        "iterations",
        "dklen",
        NULL,
    };
    int rc;
    char *name, *key;
    Py_buffer password, salt;
    long iterations, dklen;
    PyObject *key_obj = NULL;
    PyObject *dklen_obj = Py_None;
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
    rc = pbkdf2(digest, password.buf, password.len,
                salt.buf, salt.len, iterations, dklen, key);
    Py_END_ALLOW_THREADS
    if (rc) {
        Py_CLEAR(key_obj);
        SetMbedtlsError(PyExc_ValueError, rc);
        goto end;
    }
  end:
    PyBuffer_Release(&password);
    PyBuffer_Release(&salt);
    return key_obj;
}

static PyObject *
GenerateHashNameList(void)
{
    int i;
    char *s;
    const uint8_t *p;
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

#define GEN_CONSTRUCTOR(NAME, STRNAME)                                  \
static PyObject *                                                       \
hashlib_new_ ## NAME (PyObject *self, PyObject *args)                   \
{                                                                       \
    PyObject *ret;                                                      \
    Py_buffer data = {0};                                               \
    if (!PyArg_ParseTuple(args, "|y*:" STRNAME, &data)) return 0;       \
    ret = NewHasher(CONST_ ## NAME ## _name_obj,                        \
                    mbedtls_md_info_from_type(MBEDTLS_MD_ ## NAME),     \
                    data.buf, data.len);                                \
    PyBuffer_Release(&data);                                            \
    return ret;                                                         \
}

#define CONSTRUCTOR_METH_DEF(NAME, STRNAME)                              \
    {"mbedtls_" STRNAME, (PyCFunction)hashlib_new_ ## NAME, METH_VARARGS,\
     PyDoc_STR("mbedtls_" STRNAME "($module, string=b'')\n--\n\n"        \
               "Returns a " STRNAME " hash object; optionally "          \
               "initialized with a string")}

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

static struct PyMethodDef hashlib_functions[] = {
    {"new",         (PyCFunction)hashlib_new, METH_VARARGS|METH_KEYWORDS, hashlib_new__doc__},
    {"pbkdf2_hmac", (PyCFunction)pbkdf2_hmac, METH_VARARGS|METH_KEYWORDS, pbkdf2_hmac__doc__},
    CONSTRUCTOR_METH_DEF(MD5, "md5"),
    CONSTRUCTOR_METH_DEF(SHA1, "sha1"),
    CONSTRUCTOR_METH_DEF(SHA224, "sha224"),
    CONSTRUCTOR_METH_DEF(SHA256, "sha256"),
    CONSTRUCTOR_METH_DEF(SHA384, "sha384"),
    CONSTRUCTOR_METH_DEF(SHA512, "sha512"),
    CONSTRUCTOR_METH_DEF(BLAKE2B256, "blake2b256"),
    {0}
};

static struct PyModuleDef _hashlibmodule = {
    PyModuleDef_HEAD_INIT,
    "_hashlib",
    NULL,
    -1,
    hashlib_functions,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__hashlib(void)
{
    PyObject *m, *mbedtls_md_meth_names;
    Py_TYPE(&hasher_type) = &PyType_Type;
    if (PyType_Ready(&hasher_type) < 0) return 0;
    if (!(m = PyModule_Create(&_hashlibmodule))) return 0;
    if (!(mbedtls_md_meth_names = GenerateHashNameList())) {
        Py_DECREF(m);
        return NULL;
    }
    if (PyModule_AddObject(m, "mbedtls_md_meth_names", mbedtls_md_meth_names)) {
        Py_DECREF(m);
        return NULL;
    }
    Py_INCREF((PyObject *)&hasher_type);
    PyModule_AddObject(m, "HASH", (PyObject *)&hasher_type);
    INIT_CONSTRUCTOR_CONSTANTS(MD5, "md5")
    INIT_CONSTRUCTOR_CONSTANTS(SHA1, "sha1")
    INIT_CONSTRUCTOR_CONSTANTS(SHA224, "sha224")
    INIT_CONSTRUCTOR_CONSTANTS(SHA256, "sha256")
    INIT_CONSTRUCTOR_CONSTANTS(SHA384, "sha384")
    INIT_CONSTRUCTOR_CONSTANTS(SHA512, "sha512")
    INIT_CONSTRUCTOR_CONSTANTS(BLAKE2B256, "blake2b256")
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab__hashlib = {
    "_hashlib",
    PyInit__hashlib,
};
