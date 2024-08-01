/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "libc/assert.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/boolobject.h"
#include "third_party/python/Include/bytearrayobject.h"
#include "third_party/python/Include/bytes_methods.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/floatobject.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/pyctype.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pyhash.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pystrhex.h"
#include "third_party/python/Include/pystrtod.h"
#include "third_party/python/Include/sliceobject.h"
#include "libc/ctype.h"
#include "third_party/python/Include/warnings.h"

/*[clinic input]
class bytes "PyBytesObject *" "&PyBytes_Type"
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=7a238f965d64892b]*/

#include "third_party/python/Objects/clinic/bytesobject.inc"

#ifdef COUNT_ALLOCS
Py_ssize_t null_strings, one_strings;
#endif

static PyBytesObject *characters[UCHAR_MAX + 1];
static PyBytesObject *nullstring;

/* PyBytesObject_SIZE gives the basic size of a string; any memory allocation
   for a string of length n should request PyBytesObject_SIZE + n bytes.

   Using PyBytesObject_SIZE instead of sizeof(PyBytesObject) saves
   3 bytes per string allocation on a typical system.
*/
#define PyBytesObject_SIZE (offsetof(PyBytesObject, ob_sval) + 1)

/* Forward declaration */
Py_LOCAL_INLINE(Py_ssize_t) _PyBytesWriter_GetSize(_PyBytesWriter *writer,
                                                   char *str);

/*
   For PyBytes_FromString(), the parameter `str' points to a null-terminated
   string containing exactly `size' bytes.

   For PyBytes_FromStringAndSize(), the parameter `str' is
   either NULL or else points to a string containing at least `size' bytes.
   For PyBytes_FromStringAndSize(), the string in the `str' parameter does
   not have to be null-terminated.  (Therefore it is safe to construct a
   substring by calling `PyBytes_FromStringAndSize(origstring, substrlen)'.)
   If `str' is NULL then PyBytes_FromStringAndSize() will allocate `size+1'
   bytes (setting the last byte to the null terminating character) and you can
   fill in the data yourself.  If `str' is non-NULL then the resulting
   PyBytes object must be treated as immutable and you must not fill in nor
   alter the data yourself, since the strings may be shared.

   The PyObject member `op->ob_size', which denotes the number of "extra
   items" in a variable-size object, will contain the number of bytes
   allocated for string data, not counting the null terminating character.
   It is therefore equal to the `size' parameter (for
   PyBytes_FromStringAndSize()) or the length of the string in the `str'
   parameter (for PyBytes_FromString()).
*/
static PyObject *
_PyBytes_FromSize(Py_ssize_t size, int use_calloc)
{
    PyBytesObject *op;
    assert(size >= 0);

    if (size == 0 && (op = nullstring) != NULL) {
#ifdef COUNT_ALLOCS
        null_strings++;
#endif
        Py_INCREF(op);
        return (PyObject *)op;
    }

    if ((size_t)size > (size_t)PY_SSIZE_T_MAX - PyBytesObject_SIZE) {
        PyErr_SetString(PyExc_OverflowError,
                        "byte string is too large");
        return NULL;
    }

    /* Inline PyObject_NewVar */
    if (use_calloc)
        op = (PyBytesObject *)PyObject_Calloc(1, PyBytesObject_SIZE + size);
    else
        op = (PyBytesObject *)PyObject_Malloc(PyBytesObject_SIZE + size);
    if (op == NULL)
        return PyErr_NoMemory();
    (void)PyObject_INIT_VAR(op, &PyBytes_Type, size);
    op->ob_shash = -1;
    if (!use_calloc)
        op->ob_sval[size] = '\0';
    /* empty byte string singleton */
    if (size == 0) {
        nullstring = op;
        Py_INCREF(op);
    }
    return (PyObject *) op;
}

PyObject *
PyBytes_FromStringAndSize(const char *str, Py_ssize_t size)
{
    PyBytesObject *op;
    if (size < 0) {
        PyErr_SetString(PyExc_SystemError,
            "Negative size passed to PyBytes_FromStringAndSize");
        return NULL;
    }
    if (size == 1 && str != NULL &&
        (op = characters[*str & UCHAR_MAX]) != NULL)
    {
#ifdef COUNT_ALLOCS
        one_strings++;
#endif
        Py_INCREF(op);
        return (PyObject *)op;
    }
    op = (PyBytesObject *)_PyBytes_FromSize(size, 0);
    if (op == NULL)
        return NULL;
    if (str == NULL)
        return (PyObject *) op;
    memcpy(op->ob_sval, str, size);
    /* share short strings */
    if (size == 1) {
        characters[*str & UCHAR_MAX] = op;
        Py_INCREF(op);
    }
    return (PyObject *) op;
}

PyObject *
PyBytes_FromString(const char *str)
{
    size_t size;
    PyBytesObject *op;
    assert(str != NULL);
    size = strlen(str);
    if (size > PY_SSIZE_T_MAX - PyBytesObject_SIZE) {
        PyErr_SetString(PyExc_OverflowError,
            "byte string is too long");
        return NULL;
    }
    if (size == 0 && (op = nullstring) != NULL) {
#ifdef COUNT_ALLOCS
        null_strings++;
#endif
        Py_INCREF(op);
        return (PyObject *)op;
    }
    if (size == 1 && (op = characters[*str & UCHAR_MAX]) != NULL) {
#ifdef COUNT_ALLOCS
        one_strings++;
#endif
        Py_INCREF(op);
        return (PyObject *)op;
    }
    /* Inline PyObject_NewVar */
    op = (PyBytesObject *)PyObject_MALLOC(PyBytesObject_SIZE + size);
    if (op == NULL)
        return PyErr_NoMemory();
    (void)PyObject_INIT_VAR(op, &PyBytes_Type, size);
    op->ob_shash = -1;
    memcpy(op->ob_sval, str, size+1);
    /* share short strings */
    if (size == 0) {
        nullstring = op;
        Py_INCREF(op);
    } else if (size == 1) {
        characters[*str & UCHAR_MAX] = op;
        Py_INCREF(op);
    }
    return (PyObject *) op;
}

PyObject *
PyBytes_FromFormatV(const char *format, va_list vargs)
{
    char *s;
    const char *f;
    const char *p;
    Py_ssize_t prec;
    int longflag;
    int size_tflag;
    /* Longest 64-bit formatted numbers:
       - "18446744073709551615\0" (21 bytes)
       - "-9223372036854775808\0" (21 bytes)
       Decimal takes the most space (it isn't enough for octal.)

       Longest 64-bit pointer representation:
       "0xffffffffffffffff\0" (19 bytes). */
    char buffer[21];
    _PyBytesWriter writer;

    _PyBytesWriter_Init(&writer);

    s = _PyBytesWriter_Alloc(&writer, strlen(format));
    if (s == NULL)
        return NULL;
    writer.overallocate = 1;

#define WRITE_BYTES(str) \
    do { \
        s = _PyBytesWriter_WriteBytes(&writer, s, (str), strlen(str)); \
        if (s == NULL) \
            goto error; \
    } while (0)

    for (f = format; *f; f++) {
        if (*f != '%') {
            *s++ = *f;
            continue;
        }

        p = f++;

        /* ignore the width (ex: 10 in "%10s") */
        while (Py_ISDIGIT(*f))
            f++;

        /* parse the precision (ex: 10 in "%.10s") */
        prec = 0;
        if (*f == '.') {
            f++;
            for (; Py_ISDIGIT(*f); f++) {
                prec = (prec * 10) + (*f - '0');
            }
        }

        while (*f && *f != '%' && !Py_ISALPHA(*f))
            f++;

        /* handle the long flag ('l'), but only for %ld and %lu.
           others can be added when necessary. */
        longflag = 0;
        if (*f == 'l' && (f[1] == 'd' || f[1] == 'u')) {
            longflag = 1;
            ++f;
        }

        /* handle the size_t flag ('z'). */
        size_tflag = 0;
        if (*f == 'z' && (f[1] == 'd' || f[1] == 'u')) {
            size_tflag = 1;
            ++f;
        }

        /* subtract bytes preallocated for the format string
           (ex: 2 for "%s") */
        writer.min_size -= (f - p + 1);

        switch (*f) {
        case 'c':
        {
            int c = va_arg(vargs, int);
            if (c < 0 || c > 255) {
                PyErr_SetString(PyExc_OverflowError,
                                "PyBytes_FromFormatV(): %c format "
                                "expects an integer in range [0; 255]");
                goto error;
            }
            writer.min_size++;
            *s++ = (unsigned char)c;
            break;
        }

        case 'd':
            if (longflag)
                sprintf(buffer, "%ld", va_arg(vargs, long));
            else if (size_tflag)
                sprintf(buffer, "%" PY_FORMAT_SIZE_T "d",
                    va_arg(vargs, Py_ssize_t));
            else
                sprintf(buffer, "%d", va_arg(vargs, int));
            assert(strlen(buffer) < sizeof(buffer));
            WRITE_BYTES(buffer);
            break;

        case 'u':
            if (longflag)
                sprintf(buffer, "%lu",
                    va_arg(vargs, unsigned long));
            else if (size_tflag)
                sprintf(buffer, "%" PY_FORMAT_SIZE_T "u",
                    va_arg(vargs, size_t));
            else
                sprintf(buffer, "%u",
                    va_arg(vargs, unsigned int));
            assert(strlen(buffer) < sizeof(buffer));
            WRITE_BYTES(buffer);
            break;

        case 'i':
            sprintf(buffer, "%i", va_arg(vargs, int));
            assert(strlen(buffer) < sizeof(buffer));
            WRITE_BYTES(buffer);
            break;

        case 'x':
            sprintf(buffer, "%x", va_arg(vargs, int));
            assert(strlen(buffer) < sizeof(buffer));
            WRITE_BYTES(buffer);
            break;

        case 's':
        {
            Py_ssize_t i;

            p = va_arg(vargs, const char*);
            i = strlen(p);
            if (prec > 0 && i > prec)
                i = prec;
            s = _PyBytesWriter_WriteBytes(&writer, s, p, i);
            if (s == NULL)
                goto error;
            break;
        }

        case 'p':
            sprintf(buffer, "%p", va_arg(vargs, void*));
            assert(strlen(buffer) < sizeof(buffer));
            /* %p is ill-defined:  ensure leading 0x. */
            if (buffer[1] == 'X')
                buffer[1] = 'x';
            else if (buffer[1] != 'x') {
                memmove(buffer+2, buffer, strlen(buffer)+1);
                buffer[0] = '0';
                buffer[1] = 'x';
            }
            WRITE_BYTES(buffer);
            break;

        case '%':
            writer.min_size++;
            *s++ = '%';
            break;

        default:
            if (*f == 0) {
                /* fix min_size if we reached the end of the format string */
                writer.min_size++;
            }

            /* invalid format string: copy unformatted string and exit */
            WRITE_BYTES(p);
            return _PyBytesWriter_Finish(&writer, s);
        }
    }

#undef WRITE_BYTES

    return _PyBytesWriter_Finish(&writer, s);

 error:
    _PyBytesWriter_Dealloc(&writer);
    return NULL;
}

PyObject *
PyBytes_FromFormat(const char *format, ...)
{
    PyObject* ret;
    va_list vargs;

#ifdef HAVE_STDARG_PROTOTYPES
    va_start(vargs, format);
#else
    va_start(vargs);
#endif
    ret = PyBytes_FromFormatV(format, vargs);
    va_end(vargs);
    return ret;
}

/* Helpers for formatstring */

Py_LOCAL_INLINE(PyObject *)
getnextarg(PyObject *args, Py_ssize_t arglen, Py_ssize_t *p_argidx)
{
    Py_ssize_t argidx = *p_argidx;
    if (argidx < arglen) {
        (*p_argidx)++;
        if (arglen < 0)
            return args;
        else
            return PyTuple_GetItem(args, argidx);
    }
    PyErr_SetString(PyExc_TypeError,
                    "not enough arguments for format string");
    return NULL;
}

/* Format codes
 * F_LJUST      '-'
 * F_SIGN       '+'
 * F_BLANK      ' '
 * F_ALT        '#'
 * F_ZERO       '0'
 */
#define F_LJUST (1<<0)
#define F_SIGN  (1<<1)
#define F_BLANK (1<<2)
#define F_ALT   (1<<3)
#define F_ZERO  (1<<4)

/* Returns a new reference to a PyBytes object, or NULL on failure. */

static char*
formatfloat(PyObject *v, int flags, int prec, int type,
            PyObject **p_result, _PyBytesWriter *writer, char *str)
{
    char *p;
    PyObject *result;
    double x;
    size_t len;

    x = PyFloat_AsDouble(v);
    if (x == -1.0 && PyErr_Occurred()) {
        PyErr_Format(PyExc_TypeError, "float argument required, "
                     "not %.200s", Py_TYPE(v)->tp_name);
        return NULL;
    }

    if (prec < 0)
        prec = 6;

    p = PyOS_double_to_string(x, type, prec,
                              (flags & F_ALT) ? Py_DTSF_ALT : 0, NULL);

    if (p == NULL)
        return NULL;

    len = strlen(p);
    if (writer != NULL) {
        str = _PyBytesWriter_Prepare(writer, str, len);
        if (str == NULL)
            return NULL;
        memcpy(str, p, len);
        PyMem_Free(p);
        str += len;
        return str;
    }

    result = PyBytes_FromStringAndSize(p, len);
    PyMem_Free(p);
    *p_result = result;
    return result != NULL ? str : NULL;
}

static PyObject *
formatlong(PyObject *v, int flags, int prec, int type)
{
    PyObject *result, *iobj;
    if (type == 'i')
        type = 'd';
    if (PyLong_Check(v))
        return _PyUnicode_FormatLong(v, flags & F_ALT, prec, type);
    if (PyNumber_Check(v)) {
        /* make sure number is a type of integer for o, x, and X */
        if (type == 'o' || type == 'x' || type == 'X')
            iobj = PyNumber_Index(v);
        else
            iobj = PyNumber_Long(v);
        if (iobj == NULL) {
            if (!PyErr_ExceptionMatches(PyExc_TypeError))
                return NULL;
        }
        else if (!PyLong_Check(iobj))
            Py_CLEAR(iobj);
        if (iobj != NULL) {
            result = _PyUnicode_FormatLong(iobj, flags & F_ALT, prec, type);
            Py_DECREF(iobj);
            return result;
        }
    }
    PyErr_Format(PyExc_TypeError,
        "%%%c format: %s is required, not %.200s", type,
        (type == 'o' || type == 'x' || type == 'X') ? "an integer"
                                                    : "a number",
        Py_TYPE(v)->tp_name);
    return NULL;
}

static int
byte_converter(PyObject *arg, char *p)
{
    if (PyBytes_Check(arg) && PyBytes_GET_SIZE(arg) == 1) {
        *p = PyBytes_AS_STRING(arg)[0];
        return 1;
    }
    else if (PyByteArray_Check(arg) && PyByteArray_GET_SIZE(arg) == 1) {
        *p = PyByteArray_AS_STRING(arg)[0];
        return 1;
    }
    else {
        PyObject *iobj;
        long ival;
        int overflow;
        /* make sure number is a type of integer */
        if (PyLong_Check(arg)) {
            ival = PyLong_AsLongAndOverflow(arg, &overflow);
        }
        else {
            iobj = PyNumber_Index(arg);
            if (iobj == NULL) {
                if (!PyErr_ExceptionMatches(PyExc_TypeError))
                    return 0;
                goto onError;
            }
            ival = PyLong_AsLongAndOverflow(iobj, &overflow);
            Py_DECREF(iobj);
        }
        if (!overflow && ival == -1 && PyErr_Occurred())
            goto onError;
        if (overflow || !(0 <= ival && ival <= 255)) {
            PyErr_SetString(PyExc_OverflowError,
                            "%c arg not in range(256)");
            return 0;
        }
        *p = (char)ival;
        return 1;
    }
  onError:
    PyErr_SetString(PyExc_TypeError,
        "%c requires an integer in range(256) or a single byte");
    return 0;
}

static PyObject *_PyBytes_FromBuffer(PyObject *x);

static PyObject *
format_obj(PyObject *v, const char **pbuf, Py_ssize_t *plen)
{
    PyObject *func, *result;
    _Py_IDENTIFIER(__bytes__);
    /* is it a bytes object? */
    if (PyBytes_Check(v)) {
        *pbuf = PyBytes_AS_STRING(v);
        *plen = PyBytes_GET_SIZE(v);
        Py_INCREF(v);
        return v;
    }
    if (PyByteArray_Check(v)) {
        *pbuf = PyByteArray_AS_STRING(v);
        *plen = PyByteArray_GET_SIZE(v);
        Py_INCREF(v);
        return v;
    }
    /* does it support __bytes__? */
    func = _PyObject_LookupSpecial(v, &PyId___bytes__);
    if (func != NULL) {
        result = PyObject_CallFunctionObjArgs(func, NULL);
        Py_DECREF(func);
        if (result == NULL)
            return NULL;
        if (!PyBytes_Check(result)) {
            PyErr_Format(PyExc_TypeError,
                         "__bytes__ returned non-bytes (type %.200s)",
                         Py_TYPE(result)->tp_name);
            Py_DECREF(result);
            return NULL;
        }
        *pbuf = PyBytes_AS_STRING(result);
        *plen = PyBytes_GET_SIZE(result);
        return result;
    }
    /* does it support buffer protocol? */
    if (PyObject_CheckBuffer(v)) {
        /* maybe we can avoid making a copy of the buffer object here? */
        result = _PyBytes_FromBuffer(v);
        if (result == NULL)
            return NULL;
        *pbuf = PyBytes_AS_STRING(result);
        *plen = PyBytes_GET_SIZE(result);
        return result;
    }
    PyErr_Format(PyExc_TypeError,
                 "%%b requires a bytes-like object, "
                 "or an object that implements __bytes__, not '%.100s'",
                 Py_TYPE(v)->tp_name);
    return NULL;
}

/* fmt%(v1,v2,...) is roughly equivalent to sprintf(fmt, v1, v2, ...) */

PyObject *
_PyBytes_FormatEx(const char *format, Py_ssize_t format_len,
                  PyObject *args, int use_bytearray)
{
    const char *fmt;
    char *res;
    Py_ssize_t arglen, argidx;
    Py_ssize_t fmtcnt;
    int args_owned = 0;
    PyObject *dict = NULL;
    _PyBytesWriter writer;

    if (args == NULL) {
        PyErr_BadInternalCall();
        return NULL;
    }
    fmt = format;
    fmtcnt = format_len;

    _PyBytesWriter_Init(&writer);
    writer.use_bytearray = use_bytearray;

    res = _PyBytesWriter_Alloc(&writer, fmtcnt);
    if (res == NULL)
        return NULL;
    if (!use_bytearray)
        writer.overallocate = 1;

    if (PyTuple_Check(args)) {
        arglen = PyTuple_GET_SIZE(args);
        argidx = 0;
    }
    else {
        arglen = -1;
        argidx = -2;
    }
    if (Py_TYPE(args)->tp_as_mapping && Py_TYPE(args)->tp_as_mapping->mp_subscript &&
        !PyTuple_Check(args) && !PyBytes_Check(args) && !PyUnicode_Check(args) &&
        !PyByteArray_Check(args)) {
            dict = args;
    }

    while (--fmtcnt >= 0) {
        if (*fmt != '%') {
            Py_ssize_t len;
            char *pos;

            pos = (char *)memchr(fmt + 1, '%', fmtcnt);
            if (pos != NULL)
                len = pos - fmt;
            else
                len = fmtcnt + 1;
            assert(len != 0);

            memcpy(res, fmt, len);
            res += len;
            fmt += len;
            fmtcnt -= (len - 1);
        }
        else {
            /* Got a format specifier */
            int flags = 0;
            Py_ssize_t width = -1;
            int prec = -1;
            int c = '\0';
            int fill;
            PyObject *v = NULL;
            PyObject *temp = NULL;
            const char *pbuf = NULL;
            int sign;
            Py_ssize_t len = 0;
            char onechar; /* For byte_converter() */
            Py_ssize_t alloc;
#ifdef Py_DEBUG
            char *before;
#endif

            fmt++;
            if (*fmt == '(') {
                const char *keystart;
                Py_ssize_t keylen;
                PyObject *key;
                int pcount = 1;

                if (dict == NULL) {
                    PyErr_SetString(PyExc_TypeError,
                             "format requires a mapping");
                    goto error;
                }
                ++fmt;
                --fmtcnt;
                keystart = fmt;
                /* Skip over balanced parentheses */
                while (pcount > 0 && --fmtcnt >= 0) {
                    if (*fmt == ')')
                        --pcount;
                    else if (*fmt == '(')
                        ++pcount;
                    fmt++;
                }
                keylen = fmt - keystart - 1;
                if (fmtcnt < 0 || pcount > 0) {
                    PyErr_SetString(PyExc_ValueError,
                               "incomplete format key");
                    goto error;
                }
                key = PyBytes_FromStringAndSize(keystart,
                                                 keylen);
                if (key == NULL)
                    goto error;
                if (args_owned) {
                    Py_DECREF(args);
                    args_owned = 0;
                }
                args = PyObject_GetItem(dict, key);
                Py_DECREF(key);
                if (args == NULL) {
                    goto error;
                }
                args_owned = 1;
                arglen = -1;
                argidx = -2;
            }

            /* Parse flags. Example: "%+i" => flags=F_SIGN. */
            while (--fmtcnt >= 0) {
                switch (c = *fmt++) {
                case '-': flags |= F_LJUST; continue;
                case '+': flags |= F_SIGN; continue;
                case ' ': flags |= F_BLANK; continue;
                case '#': flags |= F_ALT; continue;
                case '0': flags |= F_ZERO; continue;
                }
                break;
            }

            /* Parse width. Example: "%10s" => width=10 */
            if (c == '*') {
                v = getnextarg(args, arglen, &argidx);
                if (v == NULL)
                    goto error;
                if (!PyLong_Check(v)) {
                    PyErr_SetString(PyExc_TypeError,
                                    "* wants int");
                    goto error;
                }
                width = PyLong_AsSsize_t(v);
                if (width == -1 && PyErr_Occurred())
                    goto error;
                if (width < 0) {
                    flags |= F_LJUST;
                    width = -width;
                }
                if (--fmtcnt >= 0)
                    c = *fmt++;
            }
            else if (c >= 0 && isdigit(c)) {
                width = c - '0';
                while (--fmtcnt >= 0) {
                    c = Py_CHARMASK(*fmt++);
                    if (!isdigit(c))
                        break;
                    if (width > (PY_SSIZE_T_MAX - ((int)c - '0')) / 10) {
                        PyErr_SetString(
                            PyExc_ValueError,
                            "width too big");
                        goto error;
                    }
                    width = width*10 + (c - '0');
                }
            }

            /* Parse precision. Example: "%.3f" => prec=3 */
            if (c == '.') {
                prec = 0;
                if (--fmtcnt >= 0)
                    c = *fmt++;
                if (c == '*') {
                    v = getnextarg(args, arglen, &argidx);
                    if (v == NULL)
                        goto error;
                    if (!PyLong_Check(v)) {
                        PyErr_SetString(
                            PyExc_TypeError,
                            "* wants int");
                        goto error;
                    }
                    prec = _PyLong_AsInt(v);
                    if (prec == -1 && PyErr_Occurred())
                        goto error;
                    if (prec < 0)
                        prec = 0;
                    if (--fmtcnt >= 0)
                        c = *fmt++;
                }
                else if (c >= 0 && isdigit(c)) {
                    prec = c - '0';
                    while (--fmtcnt >= 0) {
                        c = Py_CHARMASK(*fmt++);
                        if (!isdigit(c))
                            break;
                        if (prec > (INT_MAX - ((int)c - '0')) / 10) {
                            PyErr_SetString(
                                PyExc_ValueError,
                                "prec too big");
                            goto error;
                        }
                        prec = prec*10 + (c - '0');
                    }
                }
            } /* prec */
            if (fmtcnt >= 0) {
                if (c == 'h' || c == 'l' || c == 'L') {
                    if (--fmtcnt >= 0)
                        c = *fmt++;
                }
            }
            if (fmtcnt < 0) {
                PyErr_SetString(PyExc_ValueError,
                                "incomplete format");
                goto error;
            }
            if (c != '%') {
                v = getnextarg(args, arglen, &argidx);
                if (v == NULL)
                    goto error;
            }

            if (fmtcnt == 0) {
                /* last write: disable writer overallocation */
                writer.overallocate = 0;
            }

            sign = 0;
            fill = ' ';
            switch (c) {
            case '%':
                *res++ = '%';
                continue;

            case 'r':
                // %r is only for 2/3 code; 3 only code should use %a
            case 'a':
                temp = PyObject_ASCII(v);
                if (temp == NULL)
                    goto error;
                assert(PyUnicode_IS_ASCII(temp));
                pbuf = (const char *)PyUnicode_1BYTE_DATA(temp);
                len = PyUnicode_GET_LENGTH(temp);
                if (prec >= 0 && len > prec)
                    len = prec;
                break;

            case 's':
                // %s is only for 2/3 code; 3 only code should use %b
            case 'b':
                temp = format_obj(v, &pbuf, &len);
                if (temp == NULL)
                    goto error;
                if (prec >= 0 && len > prec)
                    len = prec;
                break;

            case 'i':
            case 'd':
            case 'u':
            case 'o':
            case 'x':
            case 'X':
                if (PyLong_CheckExact(v)
                    && width == -1 && prec == -1
                    && !(flags & (F_SIGN | F_BLANK))
                    && c != 'X')
                {
                    /* Fast path */
                    int alternate = flags & F_ALT;
                    int base;

                    switch(c)
                    {
                        default:
                            assert(0 && "'type' not in [diuoxX]");
                        case 'd':
                        case 'i':
                        case 'u':
                            base = 10;
                            break;
                        case 'o':
                            base = 8;
                            break;
                        case 'x':
                        case 'X':
                            base = 16;
                            break;
                    }

                    /* Fast path */
                    writer.min_size -= 2; /* size preallocated for "%d" */
                    res = _PyLong_FormatBytesWriter(&writer, res,
                                                    v, base, alternate);
                    if (res == NULL)
                        goto error;
                    continue;
                }

                temp = formatlong(v, flags, prec, c);
                if (!temp)
                    goto error;
                assert(PyUnicode_IS_ASCII(temp));
                pbuf = (const char *)PyUnicode_1BYTE_DATA(temp);
                len = PyUnicode_GET_LENGTH(temp);
                sign = 1;
                if (flags & F_ZERO)
                    fill = '0';
                break;

            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                if (width == -1 && prec == -1
                    && !(flags & (F_SIGN | F_BLANK)))
                {
                    /* Fast path */
                    writer.min_size -= 2; /* size preallocated for "%f" */
                    res = formatfloat(v, flags, prec, c, NULL, &writer, res);
                    if (res == NULL)
                        goto error;
                    continue;
                }

                if (!formatfloat(v, flags, prec, c, &temp, NULL, res))
                    goto error;
                pbuf = PyBytes_AS_STRING(temp);
                len = PyBytes_GET_SIZE(temp);
                sign = 1;
                if (flags & F_ZERO)
                    fill = '0';
                break;

            case 'c':
                pbuf = &onechar;
                len = byte_converter(v, &onechar);
                if (!len)
                    goto error;
                if (width == -1) {
                    /* Fast path */
                    *res++ = onechar;
                    continue;
                }
                break;

            default:
                PyErr_Format(PyExc_ValueError,
                  "unsupported format character '%c' (0x%x) "
                  "at index %zd",
                  c, c,
                  (Py_ssize_t)(fmt - 1 - format));
                goto error;
            }

            if (sign) {
                if (*pbuf == '-' || *pbuf == '+') {
                    sign = *pbuf++;
                    len--;
                }
                else if (flags & F_SIGN)
                    sign = '+';
                else if (flags & F_BLANK)
                    sign = ' ';
                else
                    sign = 0;
            }
            if (width < len)
                width = len;

            alloc = width;
            if (sign != 0 && len == width)
                alloc++;
            /* 2: size preallocated for %s */
            if (alloc > 2) {
                res = _PyBytesWriter_Prepare(&writer, res, alloc - 2);
                if (res == NULL)
                    goto error;
            }
#ifdef Py_DEBUG
            before = res;
#endif

            /* Write the sign if needed */
            if (sign) {
                if (fill != ' ')
                    *res++ = sign;
                if (width > len)
                    width--;
            }

            /* Write the numeric prefix for "x", "X" and "o" formats
               if the alternate form is used.
               For example, write "0x" for the "%#x" format. */
            if ((flags & F_ALT) && (c == 'o' || c == 'x' || c == 'X')) {
                assert(pbuf[0] == '0');
                assert(pbuf[1] == c);
                if (fill != ' ') {
                    *res++ = *pbuf++;
                    *res++ = *pbuf++;
                }
                width -= 2;
                if (width < 0)
                    width = 0;
                len -= 2;
            }

            /* Pad left with the fill character if needed */
            if (width > len && !(flags & F_LJUST)) {
                memset(res, fill, width - len);
                res += (width - len);
                width = len;
            }

            /* If padding with spaces: write sign if needed and/or numeric
               prefix if the alternate form is used */
            if (fill == ' ') {
                if (sign)
                    *res++ = sign;
                if ((flags & F_ALT) && (c == 'o' || c == 'x' || c == 'X')) {
                    assert(pbuf[0] == '0');
                    assert(pbuf[1] == c);
                    *res++ = *pbuf++;
                    *res++ = *pbuf++;
                }
            }

            /* Copy bytes */
            memcpy(res, pbuf, len);
            res += len;

            /* Pad right with the fill character if needed */
            if (width > len) {
                memset(res, ' ', width - len);
                res += (width - len);
            }

            if (dict && (argidx < arglen) && c != '%') {
                PyErr_SetString(PyExc_TypeError,
                           "not all arguments converted during bytes formatting");
                Py_XDECREF(temp);
                goto error;
            }
            Py_XDECREF(temp);

#ifdef Py_DEBUG
            /* check that we computed the exact size for this write */
            assert((res - before) == alloc);
#endif
        } /* '%' */

        /* If overallocation was disabled, ensure that it was the last
           write. Otherwise, we missed an optimization */
        assert(writer.overallocate || fmtcnt == 0 || use_bytearray);
    } /* until end */

    if (argidx < arglen && !dict) {
        PyErr_SetString(PyExc_TypeError,
                        "not all arguments converted during bytes formatting");
        goto error;
    }

    if (args_owned) {
        Py_DECREF(args);
    }
    return _PyBytesWriter_Finish(&writer, res);

 error:
    _PyBytesWriter_Dealloc(&writer);
    if (args_owned) {
        Py_DECREF(args);
    }
    return NULL;
}

/* =-= */

static void
bytes_dealloc(PyObject *op)
{
    Py_TYPE(op)->tp_free(op);
}

/* Unescape a backslash-escaped string. If unicode is non-zero,
   the string is a u-literal. If recode_encoding is non-zero,
   the string is UTF-8 encoded and should be re-encoded in the
   specified encoding.  */

static char *
_PyBytes_DecodeEscapeRecode(const char **s, const char *end,
                            const char *errors, const char *recode_encoding,
                            _PyBytesWriter *writer, char *p)
{
    PyObject *u, *w;
    const char* t;

    t = *s;
    /* Decode non-ASCII bytes as UTF-8. */
    while (t < end && (*t & 0x80))
        t++;
    u = PyUnicode_DecodeUTF8(*s, t - *s, errors);
    if (u == NULL)
        return NULL;

    /* Recode them in target encoding. */
    w = PyUnicode_AsEncodedString(u, recode_encoding, errors);
    Py_DECREF(u);
    if  (w == NULL)
        return NULL;
    assert(PyBytes_Check(w));

    /* Append bytes to output buffer. */
    writer->min_size--;   /* subtract 1 preallocated byte */
    p = _PyBytesWriter_WriteBytes(writer, p,
                                  PyBytes_AS_STRING(w),
                                  PyBytes_GET_SIZE(w));
    Py_DECREF(w);
    if (p == NULL)
        return NULL;

    *s = t;
    return p;
}

PyObject *_PyBytes_DecodeEscape(const char *s,
                                Py_ssize_t len,
                                const char *errors,
                                Py_ssize_t unicode,
                                const char *recode_encoding,
                                const char **first_invalid_escape)
{
    int c;
    char *p;
    const char *end;
    _PyBytesWriter writer;

    _PyBytesWriter_Init(&writer);

    p = _PyBytesWriter_Alloc(&writer, len);
    if (p == NULL)
        return NULL;
    writer.overallocate = 1;

    *first_invalid_escape = NULL;

    end = s + len;
    while (s < end) {
        if (*s != '\\') {
          non_esc:
            if (!(recode_encoding && (*s & 0x80))) {
                *p++ = *s++;
            }
            else {
                /* non-ASCII character and need to recode */
                p = _PyBytes_DecodeEscapeRecode(&s, end,
                                                errors, recode_encoding,
                                                &writer, p);
                if (p == NULL)
                    goto failed;
            }
            continue;
        }

        s++;
        if (s == end) {
            PyErr_SetString(PyExc_ValueError,
                            "Trailing \\ in string");
            goto failed;
        }

        switch (*s++) {
        /* XXX This assumes ASCII! */
        case '\n': break;
        case '\\': *p++ = '\\'; break;
        case '\'': *p++ = '\''; break;
        case '\"': *p++ = '\"'; break;
        case 'b': *p++ = '\b'; break;
        case 'f': *p++ = '\014'; break; /* FF */
        case 't': *p++ = '\t'; break;
        case 'n': *p++ = '\n'; break;
        case 'r': *p++ = '\r'; break;
        case 'v': *p++ = '\013'; break; /* VT */
        case 'e': *p++ = '\033'; break; /* [jart] ansi escape */
        case 'a': *p++ = '\007'; break; /* BEL, not classic C */
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            c = s[-1] - '0';
            if (s < end && '0' <= *s && *s <= '7') {
                c = (c<<3) + *s++ - '0';
                if (s < end && '0' <= *s && *s <= '7')
                    c = (c<<3) + *s++ - '0';
            }
            *p++ = c;
            break;
        case 'x':
            if (s+1 < end) {
                int digit1, digit2;
                digit1 = _PyLong_DigitValue[Py_CHARMASK(s[0])];
                digit2 = _PyLong_DigitValue[Py_CHARMASK(s[1])];
                if (digit1 < 16 && digit2 < 16) {
                    *p++ = (unsigned char)((digit1 << 4) + digit2);
                    s += 2;
                    break;
                }
            }
            /* invalid hexadecimal digits */

            if (!errors || strcmp(errors, "strict") == 0) {
                PyErr_Format(PyExc_ValueError,
                             "invalid \\x escape at position %d",
                             s - 2 - (end - len));
                goto failed;
            }
            if (strcmp(errors, "replace") == 0) {
                *p++ = '?';
            } else if (strcmp(errors, "ignore") == 0)
                /* do nothing */;
            else {
                PyErr_Format(PyExc_ValueError,
                             "decoding error; unknown "
                             "error handling code: %.400s",
                             errors);
                goto failed;
            }
            /* skip \x */
            if (s < end && Py_ISXDIGIT(s[0]))
                s++; /* and a hexdigit */
            break;

        default:
            if (*first_invalid_escape == NULL) {
                *first_invalid_escape = s-1; /* Back up one char, since we've
                                                already incremented s. */
            }
            *p++ = '\\';
            s--;
            goto non_esc; /* an arbitrary number of unescaped
                             UTF-8 bytes may follow. */
        }
    }

    return _PyBytesWriter_Finish(&writer, p);

  failed:
    _PyBytesWriter_Dealloc(&writer);
    return NULL;
}

PyObject *PyBytes_DecodeEscape(const char *s,
                                Py_ssize_t len,
                                const char *errors,
                                Py_ssize_t unicode,
                                const char *recode_encoding)
{
    const char* first_invalid_escape;
    PyObject *result = _PyBytes_DecodeEscape(s, len, errors, unicode,
                                             recode_encoding,
                                             &first_invalid_escape);
    if (result == NULL)
        return NULL;
    if (first_invalid_escape != NULL) {
        if (PyErr_WarnFormat(PyExc_DeprecationWarning, 1,
                             "invalid escape sequence '\\%c'",
                             (unsigned char)*first_invalid_escape) < 0) {
            Py_DECREF(result);
            return NULL;
        }
    }
    return result;

}
/* -------------------------------------------------------------------- */
/* object api */

Py_ssize_t
PyBytes_Size(PyObject *op)
{
    if (!PyBytes_Check(op)) {
        PyErr_Format(PyExc_TypeError,
             "expected bytes, %.200s found", Py_TYPE(op)->tp_name);
        return -1;
    }
    return Py_SIZE(op);
}

char *
PyBytes_AsString(PyObject *op)
{
    if (!PyBytes_Check(op)) {
        PyErr_Format(PyExc_TypeError,
             "expected bytes, %.200s found", Py_TYPE(op)->tp_name);
        return NULL;
    }
    return ((PyBytesObject *)op)->ob_sval;
}

int
PyBytes_AsStringAndSize(PyObject *obj,
                         char **s,
                         Py_ssize_t *len)
{
    if (s == NULL) {
        PyErr_BadInternalCall();
        return -1;
    }

    if (!PyBytes_Check(obj)) {
        PyErr_Format(PyExc_TypeError,
             "expected bytes, %.200s found", Py_TYPE(obj)->tp_name);
        return -1;
    }

    *s = PyBytes_AS_STRING(obj);
    if (len != NULL)
        *len = PyBytes_GET_SIZE(obj);
    else if (strlen(*s) != (size_t)PyBytes_GET_SIZE(obj)) {
        PyErr_SetString(PyExc_ValueError,
                        "embedded null byte");
        return -1;
    }
    return 0;
}

/* -------------------------------------------------------------------- */
/* Methods */

#include "third_party/python/Objects/stringlib/stringdefs.inc"

#include "third_party/python/Objects/stringlib/fastsearch.inc"
#include "third_party/python/Objects/stringlib/count.inc"
#include "third_party/python/Objects/stringlib/find.inc"
#include "third_party/python/Objects/stringlib/join.inc"
#include "third_party/python/Objects/stringlib/partition.inc"
#include "third_party/python/Objects/stringlib/split.inc"
#include "third_party/python/Objects/stringlib/ctype.inc"

#include "third_party/python/Objects/stringlib/transmogrify.inc"

PyObject *
PyBytes_Repr(PyObject *obj, int smartquotes)
{
    PyBytesObject* op = (PyBytesObject*) obj;
    Py_ssize_t i, length = Py_SIZE(op);
    Py_ssize_t newsize, squotes, dquotes;
    PyObject *v;
    unsigned char quote, *s, *p;

    /* Compute size of output string */
    squotes = dquotes = 0;
    newsize = 3; /* b'' */
    s = (unsigned char*)op->ob_sval;
    for (i = 0; i < length; i++) {
        Py_ssize_t incr = 1;
        switch(s[i]) {
        case '\'': squotes++; break;
        case '"':  dquotes++; break;
        case '\\': case '\t': case '\n': case '\r':
            incr = 2; break; /* \C */
        default:
            if (s[i] < ' ' || s[i] >= 0x7f)
                incr = 4; /* \xHH */
        }
        if (newsize > PY_SSIZE_T_MAX - incr)
            goto overflow;
        newsize += incr;
    }
    quote = '\'';
    if (smartquotes && squotes && !dquotes)
        quote = '"';
    if (squotes && quote == '\'') {
        if (newsize > PY_SSIZE_T_MAX - squotes)
            goto overflow;
        newsize += squotes;
    }

    v = PyUnicode_New(newsize, 127);
    if (v == NULL) {
        return NULL;
    }
    p = PyUnicode_1BYTE_DATA(v);

    *p++ = 'b', *p++ = quote;
    for (i = 0; i < length; i++) {
        unsigned char c = op->ob_sval[i];
        if (c == quote || c == '\\')
            *p++ = '\\', *p++ = c;
        else if (c == '\t')
            *p++ = '\\', *p++ = 't';
        else if (c == '\n')
            *p++ = '\\', *p++ = 'n';
        else if (c == '\r')
            *p++ = '\\', *p++ = 'r';
        else if (c < ' ' || c >= 0x7f) {
            *p++ = '\\';
            *p++ = 'x';
            *p++ = Py_hexdigits[(c & 0xf0) >> 4];
            *p++ = Py_hexdigits[c & 0xf];
        }
        else
            *p++ = c;
    }
    *p++ = quote;
    assert(_PyUnicode_CheckConsistency(v, 1));
    return v;

  overflow:
    PyErr_SetString(PyExc_OverflowError,
                    "bytes object is too large to make repr");
    return NULL;
}

static PyObject *
bytes_repr(PyObject *op)
{
    return PyBytes_Repr(op, 1);
}

static PyObject *
bytes_str(PyObject *op)
{
    if (Py_BytesWarningFlag) {
        if (PyErr_WarnEx(PyExc_BytesWarning,
                         "str() on a bytes instance", 1))
            return NULL;
    }
    return bytes_repr(op);
}

static Py_ssize_t
bytes_length(PyBytesObject *a)
{
    return Py_SIZE(a);
}

/* This is also used by PyBytes_Concat() */
static PyObject *
bytes_concat(PyObject *a, PyObject *b)
{
    Py_buffer va, vb;
    PyObject *result = NULL;

    va.len = -1;
    vb.len = -1;
    if (PyObject_GetBuffer(a, &va, PyBUF_SIMPLE) != 0 ||
        PyObject_GetBuffer(b, &vb, PyBUF_SIMPLE) != 0) {
        PyErr_Format(PyExc_TypeError, "can't concat %.100s to %.100s",
                     Py_TYPE(b)->tp_name, Py_TYPE(a)->tp_name);
        goto done;
    }

    /* Optimize end cases */
    if (va.len == 0 && PyBytes_CheckExact(b)) {
        result = b;
        Py_INCREF(result);
        goto done;
    }
    if (vb.len == 0 && PyBytes_CheckExact(a)) {
        result = a;
        Py_INCREF(result);
        goto done;
    }

    if (va.len > PY_SSIZE_T_MAX - vb.len) {
        PyErr_NoMemory();
        goto done;
    }

    result = PyBytes_FromStringAndSize(NULL, va.len + vb.len);
    if (result != NULL) {
        memcpy(PyBytes_AS_STRING(result), va.buf, va.len);
        memcpy(PyBytes_AS_STRING(result) + va.len, vb.buf, vb.len);
    }

  done:
    if (va.len != -1)
        PyBuffer_Release(&va);
    if (vb.len != -1)
        PyBuffer_Release(&vb);
    return result;
}

static PyObject *
bytes_repeat(PyBytesObject *a, Py_ssize_t n)
{
    Py_ssize_t i;
    Py_ssize_t j;
    Py_ssize_t size;
    PyBytesObject *op;
    size_t nbytes;
    if (n < 0)
        n = 0;
    /* watch out for overflows:  the size can overflow int,
     * and the # of bytes needed can overflow size_t
     */
    if (n > 0 && Py_SIZE(a) > PY_SSIZE_T_MAX / n) {
        PyErr_SetString(PyExc_OverflowError,
            "repeated bytes are too long");
        return NULL;
    }
    size = Py_SIZE(a) * n;
    if (size == Py_SIZE(a) && PyBytes_CheckExact(a)) {
        Py_INCREF(a);
        return (PyObject *)a;
    }
    nbytes = (size_t)size;
    if (nbytes + PyBytesObject_SIZE <= nbytes) {
        PyErr_SetString(PyExc_OverflowError,
            "repeated bytes are too long");
        return NULL;
    }
    op = (PyBytesObject *)PyObject_MALLOC(PyBytesObject_SIZE + nbytes);
    if (op == NULL)
        return PyErr_NoMemory();
    (void)PyObject_INIT_VAR(op, &PyBytes_Type, size);
    op->ob_shash = -1;
    op->ob_sval[size] = '\0';
    if (Py_SIZE(a) == 1 && n > 0) {
        memset(op->ob_sval, a->ob_sval[0] , n);
        return (PyObject *) op;
    }
    i = 0;
    if (i < size) {
        memcpy(op->ob_sval, a->ob_sval, Py_SIZE(a));
        i = Py_SIZE(a);
    }
    while (i < size) {
        j = (i <= size-i)  ?  i  :  size-i;
        memcpy(op->ob_sval+i, op->ob_sval, j);
        i += j;
    }
    return (PyObject *) op;
}

static int
bytes_contains(PyObject *self, PyObject *arg)
{
    return _Py_bytes_contains(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), arg);
}

static PyObject *
bytes_item(PyBytesObject *a, Py_ssize_t i)
{
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }
    return PyLong_FromLong((unsigned char)a->ob_sval[i]);
}

static inline int
bytes_compare_eq(PyBytesObject *a, PyBytesObject *b)
{
    Py_ssize_t len;
    len = Py_SIZE(a);
    if (Py_SIZE(b) != len)
        return 0;
    return !bcmp(a->ob_sval, b->ob_sval, len);
}

static PyObject*
bytes_richcompare(PyBytesObject *a, PyBytesObject *b, int op)
{
    int c;
    Py_ssize_t len_a, len_b;
    Py_ssize_t min_len;
    PyObject *result;
    int rc;

    /* Make sure both arguments are strings. */
    if (!(PyBytes_Check(a) && PyBytes_Check(b))) {
        if (Py_BytesWarningFlag && (op == Py_EQ || op == Py_NE)) {
            rc = PyObject_IsInstance((PyObject*)a,
                                     (PyObject*)&PyUnicode_Type);
            if (!rc)
                rc = PyObject_IsInstance((PyObject*)b,
                                         (PyObject*)&PyUnicode_Type);
            if (rc < 0)
                return NULL;
            if (rc) {
                if (PyErr_WarnEx(PyExc_BytesWarning,
                                 "Comparison between bytes and string", 1))
                    return NULL;
            }
            else {
                rc = PyObject_IsInstance((PyObject*)a,
                                         (PyObject*)&PyLong_Type);
                if (!rc)
                    rc = PyObject_IsInstance((PyObject*)b,
                                             (PyObject*)&PyLong_Type);
                if (rc < 0)
                    return NULL;
                if (rc) {
                    if (PyErr_WarnEx(PyExc_BytesWarning,
                                     "Comparison between bytes and int", 1))
                        return NULL;
                }
            }
        }
        result = Py_NotImplemented;
    }
    else if (a == b) {
        switch (op) {
        case Py_EQ:
        case Py_LE:
        case Py_GE:
            /* a string is equal to itself */
            result = Py_True;
            break;
        case Py_NE:
        case Py_LT:
        case Py_GT:
            result = Py_False;
            break;
        default:
            PyErr_BadArgument();
            return NULL;
        }
    }
    else if (op == Py_EQ || op == Py_NE) {
        int eq = bytes_compare_eq(a, b);
        eq ^= (op == Py_NE);
        result = eq ? Py_True : Py_False;
    }
    else {
        len_a = Py_SIZE(a);
        len_b = Py_SIZE(b);
        min_len = Py_MIN(len_a, len_b);
        if (min_len > 0) {
            c = Py_CHARMASK(*a->ob_sval) - Py_CHARMASK(*b->ob_sval);
            if (c == 0)
                c = memcmp(a->ob_sval, b->ob_sval, min_len);
        }
        else
            c = 0;
        if (c == 0)
            c = (len_a < len_b) ? -1 : (len_a > len_b) ? 1 : 0;
        switch (op) {
        case Py_LT: c = c <  0; break;
        case Py_LE: c = c <= 0; break;
        case Py_GT: c = c >  0; break;
        case Py_GE: c = c >= 0; break;
        default:
            PyErr_BadArgument();
            return NULL;
        }
        result = c ? Py_True : Py_False;
    }

    Py_INCREF(result);
    return result;
}

static Py_hash_t
bytes_hash(PyBytesObject *a)
{
    if (a->ob_shash == -1) {
        /* Can't fail */
        a->ob_shash = _Py_HashBytes(a->ob_sval, Py_SIZE(a));
    }
    return a->ob_shash;
}

static PyObject*
bytes_subscript(PyBytesObject* self, PyObject* item)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return NULL;
        if (i < 0)
            i += PyBytes_GET_SIZE(self);
        if (i < 0 || i >= PyBytes_GET_SIZE(self)) {
            PyErr_SetString(PyExc_IndexError,
                            "index out of range");
            return NULL;
        }
        return PyLong_FromLong((unsigned char)self->ob_sval[i]);
    }
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength, cur, i;
        char* source_buf;
        char* result_buf;
        PyObject* result;

        if (PySlice_Unpack(item, &start, &stop, &step) < 0) {
            return NULL;
        }
        slicelength = PySlice_AdjustIndices(PyBytes_GET_SIZE(self), &start,
                                            &stop, step);

        if (slicelength <= 0) {
            return PyBytes_FromStringAndSize("", 0);
        }
        else if (start == 0 && step == 1 &&
                 slicelength == PyBytes_GET_SIZE(self) &&
                 PyBytes_CheckExact(self)) {
            Py_INCREF(self);
            return (PyObject *)self;
        }
        else if (step == 1) {
            return PyBytes_FromStringAndSize(
                PyBytes_AS_STRING(self) + start,
                slicelength);
        }
        else {
            source_buf = PyBytes_AS_STRING(self);
            result = PyBytes_FromStringAndSize(NULL, slicelength);
            if (result == NULL)
                return NULL;

            result_buf = PyBytes_AS_STRING(result);
            for (cur = start, i = 0; i < slicelength;
                 cur += step, i++) {
                result_buf[i] = source_buf[cur];
            }

            return result;
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "byte indices must be integers or slices, not %.200s",
                     Py_TYPE(item)->tp_name);
        return NULL;
    }
}

static int
bytes_buffer_getbuffer(PyBytesObject *self, Py_buffer *view, int flags)
{
    return PyBuffer_FillInfo(view, (PyObject*)self, (void *)self->ob_sval, Py_SIZE(self),
                             1, flags);
}

static PySequenceMethods bytes_as_sequence = {
    (lenfunc)bytes_length, /*sq_length*/
    (binaryfunc)bytes_concat, /*sq_concat*/
    (ssizeargfunc)bytes_repeat, /*sq_repeat*/
    (ssizeargfunc)bytes_item, /*sq_item*/
    0,                  /*sq_slice*/
    0,                  /*sq_ass_item*/
    0,                  /*sq_ass_slice*/
    (objobjproc)bytes_contains /*sq_contains*/
};

static PyMappingMethods bytes_as_mapping = {
    (lenfunc)bytes_length,
    (binaryfunc)bytes_subscript,
    0,
};

static PyBufferProcs bytes_as_buffer = {
    (getbufferproc)bytes_buffer_getbuffer,
    NULL,
};


#define LEFTSTRIP 0
#define RIGHTSTRIP 1
#define BOTHSTRIP 2

/*[clinic input]
bytes.split

    sep: object = None
        The delimiter according which to split the bytes.
        None (the default value) means split on ASCII whitespace characters
        (space, tab, return, newline, formfeed, vertical tab).
    maxsplit: Py_ssize_t = -1
        Maximum number of splits to do.
        -1 (the default value) means no limit.

Return a list of the sections in the bytes, using sep as the delimiter.
[clinic start generated code]*/

static PyObject *
bytes_split_impl(PyBytesObject *self, PyObject *sep, Py_ssize_t maxsplit)
/*[clinic end generated code: output=52126b5844c1d8ef input=8b809b39074abbfa]*/
{
    Py_ssize_t len = PyBytes_GET_SIZE(self), n;
    const char *s = PyBytes_AS_STRING(self), *sub;
    Py_buffer vsub;
    PyObject *list;

    if (maxsplit < 0)
        maxsplit = PY_SSIZE_T_MAX;
    if (sep == Py_None)
        return stringlib_split_whitespace((PyObject*) self, s, len, maxsplit);
    if (PyObject_GetBuffer(sep, &vsub, PyBUF_SIMPLE) != 0)
        return NULL;
    sub = vsub.buf;
    n = vsub.len;

    list = stringlib_split((PyObject*) self, s, len, sub, n, maxsplit);
    PyBuffer_Release(&vsub);
    return list;
}

/*[clinic input]
bytes.partition

    sep: Py_buffer
    /

Partition the bytes into three parts using the given separator.

This will search for the separator sep in the bytes. If the separator is found,
returns a 3-tuple containing the part before the separator, the separator
itself, and the part after it.

If the separator is not found, returns a 3-tuple containing the original bytes
object and two empty bytes objects.
[clinic start generated code]*/

static PyObject *
bytes_partition_impl(PyBytesObject *self, Py_buffer *sep)
/*[clinic end generated code: output=f532b392a17ff695 input=61cca95519406099]*/
{
    return stringlib_partition(
        (PyObject*) self,
        PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self),
        sep->obj, (const char *)sep->buf, sep->len
        );
}

/*[clinic input]
bytes.rpartition

    sep: Py_buffer
    /

Partition the bytes into three parts using the given separator.

This will search for the separator sep in the bytes, starting at the end. If
the separator is found, returns a 3-tuple containing the part before the
separator, the separator itself, and the part after it.

If the separator is not found, returns a 3-tuple containing two empty bytes
objects and the original bytes object.
[clinic start generated code]*/

static PyObject *
bytes_rpartition_impl(PyBytesObject *self, Py_buffer *sep)
/*[clinic end generated code: output=191b114cbb028e50 input=d78db010c8cfdbe1]*/
{
    return stringlib_rpartition(
        (PyObject*) self,
        PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self),
        sep->obj, (const char *)sep->buf, sep->len
        );
}

/*[clinic input]
bytes.rsplit = bytes.split

Return a list of the sections in the bytes, using sep as the delimiter.

Splitting is done starting at the end of the bytes and working to the front.
[clinic start generated code]*/

static PyObject *
bytes_rsplit_impl(PyBytesObject *self, PyObject *sep, Py_ssize_t maxsplit)
/*[clinic end generated code: output=ba698d9ea01e1c8f input=0f86c9f28f7d7b7b]*/
{
    Py_ssize_t len = PyBytes_GET_SIZE(self), n;
    const char *s = PyBytes_AS_STRING(self), *sub;
    Py_buffer vsub;
    PyObject *list;

    if (maxsplit < 0)
        maxsplit = PY_SSIZE_T_MAX;
    if (sep == Py_None)
        return stringlib_rsplit_whitespace((PyObject*) self, s, len, maxsplit);
    if (PyObject_GetBuffer(sep, &vsub, PyBUF_SIMPLE) != 0)
        return NULL;
    sub = vsub.buf;
    n = vsub.len;

    list = stringlib_rsplit((PyObject*) self, s, len, sub, n, maxsplit);
    PyBuffer_Release(&vsub);
    return list;
}


/*[clinic input]
bytes.join

    iterable_of_bytes: object
    /

Concatenate any number of bytes objects.

The bytes whose method is called is inserted in between each pair.

The result is returned as a new bytes object.

Example: b'.'.join([b'ab', b'pq', b'rs']) -> b'ab.pq.rs'.
[clinic start generated code]*/

static PyObject *
bytes_join(PyBytesObject *self, PyObject *iterable_of_bytes)
/*[clinic end generated code: output=a046f379f626f6f8 input=7fe377b95bd549d2]*/
{
    return stringlib_bytes_join((PyObject*)self, iterable_of_bytes);
}

PyObject *
_PyBytes_Join(PyObject *sep, PyObject *x)
{
    assert(sep != NULL && PyBytes_Check(sep));
    assert(x != NULL);
    return bytes_join((PyBytesObject*)sep, x);
}

static PyObject *
bytes_find(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_find(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}

static PyObject *
bytes_index(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_index(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}


static PyObject *
bytes_rfind(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_rfind(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}


static PyObject *
bytes_rindex(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_rindex(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}


Py_LOCAL_INLINE(PyObject *)
do_xstrip(PyBytesObject *self, int striptype, PyObject *sepobj)
{
    Py_buffer vsep;
    char *s = PyBytes_AS_STRING(self);
    Py_ssize_t len = PyBytes_GET_SIZE(self);
    char *sep;
    Py_ssize_t seplen;
    Py_ssize_t i, j;

    if (PyObject_GetBuffer(sepobj, &vsep, PyBUF_SIMPLE) != 0)
        return NULL;
    sep = vsep.buf;
    seplen = vsep.len;

    i = 0;
    if (striptype != RIGHTSTRIP) {
        while (i < len && memchr(sep, Py_CHARMASK(s[i]), seplen)) {
            i++;
        }
    }

    j = len;
    if (striptype != LEFTSTRIP) {
        do {
            j--;
        } while (j >= i && memchr(sep, Py_CHARMASK(s[j]), seplen));
        j++;
    }

    PyBuffer_Release(&vsep);

    if (i == 0 && j == len && PyBytes_CheckExact(self)) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    else
        return PyBytes_FromStringAndSize(s+i, j-i);
}


Py_LOCAL_INLINE(PyObject *)
do_strip(PyBytesObject *self, int striptype)
{
    char *s = PyBytes_AS_STRING(self);
    Py_ssize_t len = PyBytes_GET_SIZE(self), i, j;

    i = 0;
    if (striptype != RIGHTSTRIP) {
        while (i < len && Py_ISSPACE(s[i])) {
            i++;
        }
    }

    j = len;
    if (striptype != LEFTSTRIP) {
        do {
            j--;
        } while (j >= i && Py_ISSPACE(s[j]));
        j++;
    }

    if (i == 0 && j == len && PyBytes_CheckExact(self)) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    else
        return PyBytes_FromStringAndSize(s+i, j-i);
}


Py_LOCAL_INLINE(PyObject *)
do_argstrip(PyBytesObject *self, int striptype, PyObject *bytes)
{
    if (bytes != NULL && bytes != Py_None) {
        return do_xstrip(self, striptype, bytes);
    }
    return do_strip(self, striptype);
}

/*[clinic input]
bytes.strip

    bytes: object = None
    /

Strip leading and trailing bytes contained in the argument.

If the argument is omitted or None, strip leading and trailing ASCII whitespace.
[clinic start generated code]*/

static PyObject *
bytes_strip_impl(PyBytesObject *self, PyObject *bytes)
/*[clinic end generated code: output=c7c228d3bd104a1b input=8a354640e4e0b3ef]*/
{
    return do_argstrip(self, BOTHSTRIP, bytes);
}

/*[clinic input]
bytes.lstrip

    bytes: object = None
    /

Strip leading bytes contained in the argument.

If the argument is omitted or None, strip leading  ASCII whitespace.
[clinic start generated code]*/

static PyObject *
bytes_lstrip_impl(PyBytesObject *self, PyObject *bytes)
/*[clinic end generated code: output=28602e586f524e82 input=9baff4398c3f6857]*/
{
    return do_argstrip(self, LEFTSTRIP, bytes);
}

/*[clinic input]
bytes.rstrip

    bytes: object = None
    /

Strip trailing bytes contained in the argument.

If the argument is omitted or None, strip trailing ASCII whitespace.
[clinic start generated code]*/

static PyObject *
bytes_rstrip_impl(PyBytesObject *self, PyObject *bytes)
/*[clinic end generated code: output=547e3815c95447da input=b78af445c727e32b]*/
{
    return do_argstrip(self, RIGHTSTRIP, bytes);
}


static PyObject *
bytes_count(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_count(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}


/*[clinic input]
bytes.translate

    table: object
        Translation table, which must be a bytes object of length 256.
    /
    delete as deletechars: object(c_default="NULL") = b''

Return a copy with each character mapped by the given translation table.

All characters occurring in the optional argument delete are removed.
The remaining characters are mapped through the given translation table.
[clinic start generated code]*/

static PyObject *
bytes_translate_impl(PyBytesObject *self, PyObject *table,
                     PyObject *deletechars)
/*[clinic end generated code: output=43be3437f1956211 input=0ecdf159f654233c]*/
{
    char *input, *output;
    Py_buffer table_view = {NULL, NULL};
    Py_buffer del_table_view = {NULL, NULL};
    const char *table_chars;
    Py_ssize_t i, c, changed = 0;
    PyObject *input_obj = (PyObject*)self;
    const char *output_start, *del_table_chars=NULL;
    Py_ssize_t inlen, tablen, dellen = 0;
    PyObject *result;
    int trans_table[256];

    if (PyBytes_Check(table)) {
        table_chars = PyBytes_AS_STRING(table);
        tablen = PyBytes_GET_SIZE(table);
    }
    else if (table == Py_None) {
        table_chars = NULL;
        tablen = 256;
    }
    else {
        if (PyObject_GetBuffer(table, &table_view, PyBUF_SIMPLE) != 0)
            return NULL;
        table_chars = table_view.buf;
        tablen = table_view.len;
    }

    if (tablen != 256) {
        PyErr_SetString(PyExc_ValueError,
          "translation table must be 256 characters long");
        PyBuffer_Release(&table_view);
        return NULL;
    }

    if (deletechars != NULL) {
        if (PyBytes_Check(deletechars)) {
            del_table_chars = PyBytes_AS_STRING(deletechars);
            dellen = PyBytes_GET_SIZE(deletechars);
        }
        else {
            if (PyObject_GetBuffer(deletechars, &del_table_view, PyBUF_SIMPLE) != 0) {
                PyBuffer_Release(&table_view);
                return NULL;
            }
            del_table_chars = del_table_view.buf;
            dellen = del_table_view.len;
        }
    }
    else {
        del_table_chars = NULL;
        dellen = 0;
    }

    inlen = PyBytes_GET_SIZE(input_obj);
    result = PyBytes_FromStringAndSize((char *)NULL, inlen);
    if (result == NULL) {
        PyBuffer_Release(&del_table_view);
        PyBuffer_Release(&table_view);
        return NULL;
    }
    output_start = output = PyBytes_AS_STRING(result);
    input = PyBytes_AS_STRING(input_obj);

    if (dellen == 0 && table_chars != NULL) {
        /* If no deletions are required, use faster code */
        for (i = inlen; --i >= 0; ) {
            c = Py_CHARMASK(*input++);
            if (Py_CHARMASK((*output++ = table_chars[c])) != c)
                changed = 1;
        }
        if (!changed && PyBytes_CheckExact(input_obj)) {
            Py_INCREF(input_obj);
            Py_DECREF(result);
            result = input_obj;
        }
        PyBuffer_Release(&del_table_view);
        PyBuffer_Release(&table_view);
        return result;
    }

    if (table_chars == NULL) {
        for (i = 0; i < 256; i++)
            trans_table[i] = Py_CHARMASK(i);
    } else {
        for (i = 0; i < 256; i++)
            trans_table[i] = Py_CHARMASK(table_chars[i]);
    }
    PyBuffer_Release(&table_view);

    for (i = 0; i < dellen; i++)
        trans_table[(int) Py_CHARMASK(del_table_chars[i])] = -1;
    PyBuffer_Release(&del_table_view);

    for (i = inlen; --i >= 0; ) {
        c = Py_CHARMASK(*input++);
        if (trans_table[c] != -1)
            if (Py_CHARMASK(*output++ = (char)trans_table[c]) == c)
                continue;
        changed = 1;
    }
    if (!changed && PyBytes_CheckExact(input_obj)) {
        Py_DECREF(result);
        Py_INCREF(input_obj);
        return input_obj;
    }
    /* Fix the size of the resulting string */
    if (inlen > 0)
        _PyBytes_Resize(&result, output - output_start);
    return result;
}


/*[clinic input]

@staticmethod
bytes.maketrans

    frm: Py_buffer
    to: Py_buffer
    /

Return a translation table useable for the bytes or bytearray translate method.

The returned table will be one where each byte in frm is mapped to the byte at
the same position in to.

The bytes objects frm and to must be of the same length.
[clinic start generated code]*/

static PyObject *
bytes_maketrans_impl(Py_buffer *frm, Py_buffer *to)
/*[clinic end generated code: output=a36f6399d4b77f6f input=de7a8fc5632bb8f1]*/
{
    return _Py_bytes_maketrans(frm, to);
}


/*[clinic input]
bytes.replace

    old: Py_buffer
    new: Py_buffer
    count: Py_ssize_t = -1
        Maximum number of occurrences to replace.
        -1 (the default value) means replace all occurrences.
    /

Return a copy with all occurrences of substring old replaced by new.

If the optional argument count is given, only the first count occurrences are
replaced.
[clinic start generated code]*/

static PyObject *
bytes_replace_impl(PyBytesObject *self, Py_buffer *old, Py_buffer *new,
                   Py_ssize_t count)
/*[clinic end generated code: output=994fa588b6b9c104 input=b2fbbf0bf04de8e5]*/
{
    return stringlib_replace((PyObject *)self,
                             (const char *)old->buf, old->len,
                             (const char *)new->buf, new->len, count);
}

/** End DALKE **/


static PyObject *
bytes_startswith(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_startswith(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}

static PyObject *
bytes_endswith(PyBytesObject *self, PyObject *args)
{
    return _Py_bytes_endswith(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self), args);
}


/*[clinic input]
bytes.decode

    encoding: str(c_default="NULL") = 'utf-8'
        The encoding with which to decode the bytes.
    errors: str(c_default="NULL") = 'strict'
        The error handling scheme to use for the handling of decoding errors.
        The default is 'strict' meaning that decoding errors raise a
        UnicodeDecodeError. Other possible values are 'ignore' and 'replace'
        as well as any other name registered with codecs.register_error that
        can handle UnicodeDecodeErrors.

Decode the bytes using the codec registered for encoding.
[clinic start generated code]*/

static PyObject *
bytes_decode_impl(PyBytesObject *self, const char *encoding,
                  const char *errors)
/*[clinic end generated code: output=5649a53dde27b314 input=958174769d2a40ca]*/
{
    return PyUnicode_FromEncodedObject((PyObject*)self, encoding, errors);
}


/*[clinic input]
bytes.splitlines

    keepends: int(c_default="0") = False

Return a list of the lines in the bytes, breaking at line boundaries.

Line breaks are not included in the resulting list unless keepends is given and
true.
[clinic start generated code]*/

static PyObject *
bytes_splitlines_impl(PyBytesObject *self, int keepends)
/*[clinic end generated code: output=3484149a5d880ffb input=7f4aac67144f9944]*/
{
    return stringlib_splitlines(
        (PyObject*) self, PyBytes_AS_STRING(self),
        PyBytes_GET_SIZE(self), keepends
        );
}

/*[clinic input]
@classmethod
bytes.fromhex

    string: unicode
    /

Create a bytes object from a string of hexadecimal numbers.

Spaces between two numbers are accepted.
Example: bytes.fromhex('B9 01EF') -> b'\\xb9\\x01\\xef'.
[clinic start generated code]*/

static PyObject *
bytes_fromhex_impl(PyTypeObject *type, PyObject *string)
/*[clinic end generated code: output=0973acc63661bb2e input=bf4d1c361670acd3]*/
{
    PyObject *result = _PyBytes_FromHex(string, 0);
    if (type != &PyBytes_Type && result != NULL) {
        Py_SETREF(result, PyObject_CallFunctionObjArgs((PyObject *)type,
                                                       result, NULL));
    }
    return result;
}

PyObject*
_PyBytes_FromHex(PyObject *string, int use_bytearray)
{
    char *buf;
    Py_ssize_t hexlen, invalid_char;
    unsigned int top, bot;
    Py_UCS1 *str, *end;
    _PyBytesWriter writer;

    _PyBytesWriter_Init(&writer);
    writer.use_bytearray = use_bytearray;

    assert(PyUnicode_Check(string));
    if (PyUnicode_READY(string))
        return NULL;
    hexlen = PyUnicode_GET_LENGTH(string);

    if (!PyUnicode_IS_ASCII(string)) {
        void *data = PyUnicode_DATA(string);
        unsigned int kind = PyUnicode_KIND(string);
        Py_ssize_t i;

        /* search for the first non-ASCII character */
        for (i = 0; i < hexlen; i++) {
            if (PyUnicode_READ(kind, data, i) >= 128)
                break;
        }
        invalid_char = i;
        goto error;
    }

    assert(PyUnicode_KIND(string) == PyUnicode_1BYTE_KIND);
    str = PyUnicode_1BYTE_DATA(string);

    /* This overestimates if there are spaces */
    buf = _PyBytesWriter_Alloc(&writer, hexlen / 2);
    if (buf == NULL)
        return NULL;

    end = str + hexlen;
    while (str < end) {
        /* skip over spaces in the input */
        if (*str == ' ') {
            do {
                str++;
            } while (*str == ' ');
            if (str >= end)
                break;
        }

        top = _PyLong_DigitValue[*str];
        if (top >= 16) {
            invalid_char = str - PyUnicode_1BYTE_DATA(string);
            goto error;
        }
        str++;

        bot = _PyLong_DigitValue[*str];
        if (bot >= 16) {
            invalid_char = str - PyUnicode_1BYTE_DATA(string);
            goto error;
        }
        str++;

        *buf++ = (unsigned char)((top << 4) + bot);
    }

    return _PyBytesWriter_Finish(&writer, buf);

  error:
    PyErr_Format(PyExc_ValueError,
                 "non-hexadecimal number found in "
                 "fromhex() arg at position %zd", invalid_char);
    _PyBytesWriter_Dealloc(&writer);
    return NULL;
}

PyDoc_STRVAR(hex__doc__,
"B.hex() -> string\n\
\n\
Create a string of hexadecimal numbers from a bytes object.\n\
Example: b'\\xb9\\x01\\xef'.hex() -> 'b901ef'.");

static PyObject *
bytes_hex(PyBytesObject *self)
{
    char* argbuf = PyBytes_AS_STRING(self);
    Py_ssize_t arglen = PyBytes_GET_SIZE(self);
    return _Py_strhex(argbuf, arglen);
}

static PyObject *
bytes_getnewargs(PyBytesObject *v)
{
    return Py_BuildValue("(y#)", v->ob_sval, Py_SIZE(v));
}


static PyMethodDef
bytes_methods[] = {
    {"__getnewargs__",          (PyCFunction)bytes_getnewargs,  METH_NOARGS},
    {"capitalize", (PyCFunction)stringlib_capitalize, METH_NOARGS,
     _Py_capitalize__doc__},
    {"center", (PyCFunction)stringlib_center, METH_VARARGS,
     _Py_center__doc__},
    {"count", (PyCFunction)bytes_count, METH_VARARGS,
     _Py_count__doc__},
    BYTES_DECODE_METHODDEF
    {"endswith", (PyCFunction)bytes_endswith, METH_VARARGS,
     _Py_endswith__doc__},
    {"expandtabs", (PyCFunction)stringlib_expandtabs, METH_VARARGS | METH_KEYWORDS,
     _Py_expandtabs__doc__},
    {"find", (PyCFunction)bytes_find, METH_VARARGS,
     _Py_find__doc__},
    BYTES_FROMHEX_METHODDEF
    {"hex", (PyCFunction)bytes_hex, METH_NOARGS, hex__doc__},
    {"index", (PyCFunction)bytes_index, METH_VARARGS, _Py_index__doc__},
    {"isalnum", (PyCFunction)stringlib_isalnum, METH_NOARGS,
     _Py_isalnum__doc__},
    {"isalpha", (PyCFunction)stringlib_isalpha, METH_NOARGS,
     _Py_isalpha__doc__},
    {"isdigit", (PyCFunction)stringlib_isdigit, METH_NOARGS,
     _Py_isdigit__doc__},
    {"islower", (PyCFunction)stringlib_islower, METH_NOARGS,
     _Py_islower__doc__},
    {"isspace", (PyCFunction)stringlib_isspace, METH_NOARGS,
     _Py_isspace__doc__},
    {"istitle", (PyCFunction)stringlib_istitle, METH_NOARGS,
     _Py_istitle__doc__},
    {"isupper", (PyCFunction)stringlib_isupper, METH_NOARGS,
     _Py_isupper__doc__},
    BYTES_JOIN_METHODDEF
    {"ljust", (PyCFunction)stringlib_ljust, METH_VARARGS, _Py_ljust__doc__},
    {"lower", (PyCFunction)stringlib_lower, METH_NOARGS, _Py_lower__doc__},
    BYTES_LSTRIP_METHODDEF
    BYTES_MAKETRANS_METHODDEF
    BYTES_PARTITION_METHODDEF
    BYTES_REPLACE_METHODDEF
    {"rfind", (PyCFunction)bytes_rfind, METH_VARARGS, _Py_rfind__doc__},
    {"rindex", (PyCFunction)bytes_rindex, METH_VARARGS, _Py_rindex__doc__},
    {"rjust", (PyCFunction)stringlib_rjust, METH_VARARGS, _Py_rjust__doc__},
    BYTES_RPARTITION_METHODDEF
    BYTES_RSPLIT_METHODDEF
    BYTES_RSTRIP_METHODDEF
    BYTES_SPLIT_METHODDEF
    BYTES_SPLITLINES_METHODDEF
    {"startswith", (PyCFunction)bytes_startswith, METH_VARARGS,
     _Py_startswith__doc__},
    BYTES_STRIP_METHODDEF
    {"swapcase", (PyCFunction)stringlib_swapcase, METH_NOARGS,
     _Py_swapcase__doc__},
    {"title", (PyCFunction)stringlib_title, METH_NOARGS, _Py_title__doc__},
    BYTES_TRANSLATE_METHODDEF
    {"upper", (PyCFunction)stringlib_upper, METH_NOARGS, _Py_upper__doc__},
    {"zfill", (PyCFunction)stringlib_zfill, METH_VARARGS, _Py_zfill__doc__},
    {NULL,     NULL}                         /* sentinel */
};

static PyObject *
bytes_mod(PyObject *self, PyObject *arg)
{
    if (!PyBytes_Check(self)) {
        Py_RETURN_NOTIMPLEMENTED;
    }
    return _PyBytes_FormatEx(PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self),
                             arg, 0);
}

static PyNumberMethods bytes_as_number = {
    0,              /*nb_add*/
    0,              /*nb_subtract*/
    0,              /*nb_multiply*/
    bytes_mod,      /*nb_remainder*/
};

static PyObject *
bytes_subtype_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

static PyObject *
bytes_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *x = NULL;
    const char *encoding = NULL;
    const char *errors = NULL;
    PyObject *new = NULL;
    PyObject *func;
    Py_ssize_t size;
    static char *kwlist[] = {"source", "encoding", "errors", 0};
    _Py_IDENTIFIER(__bytes__);

    if (type != &PyBytes_Type)
        return bytes_subtype_new(type, args, kwds);
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oss:bytes", kwlist, &x,
                                     &encoding, &errors))
        return NULL;
    if (x == NULL) {
        if (encoding != NULL || errors != NULL) {
            PyErr_SetString(PyExc_TypeError,
                            "encoding or errors without sequence "
                            "argument");
            return NULL;
        }
        return PyBytes_FromStringAndSize(NULL, 0);
    }

    if (encoding != NULL) {
        /* Encode via the codec registry */
        if (!PyUnicode_Check(x)) {
            PyErr_SetString(PyExc_TypeError,
                            "encoding without a string argument");
            return NULL;
        }
        new = PyUnicode_AsEncodedString(x, encoding, errors);
        if (new == NULL)
            return NULL;
        assert(PyBytes_Check(new));
        return new;
    }

    if (errors != NULL) {
        PyErr_SetString(PyExc_TypeError,
                        PyUnicode_Check(x) ?
                        "string argument without an encoding" :
                        "errors without a string argument");
        return NULL;
    }

    /* We'd like to call PyObject_Bytes here, but we need to check for an
       integer argument before deferring to PyBytes_FromObject, something
       PyObject_Bytes doesn't do. */
    func = _PyObject_LookupSpecial(x, &PyId___bytes__);
    if (func != NULL) {
        new = PyObject_CallFunctionObjArgs(func, NULL);
        Py_DECREF(func);
        if (new == NULL)
            return NULL;
        if (!PyBytes_Check(new)) {
            PyErr_Format(PyExc_TypeError,
                         "__bytes__ returned non-bytes (type %.200s)",
                         Py_TYPE(new)->tp_name);
            Py_DECREF(new);
            return NULL;
        }
        return new;
    }
    else if (PyErr_Occurred())
        return NULL;

    if (PyUnicode_Check(x)) {
        PyErr_SetString(PyExc_TypeError,
                        "string argument without an encoding");
        return NULL;
    }
    /* Is it an integer? */
    if (PyIndex_Check(x)) {
        size = PyNumber_AsSsize_t(x, PyExc_OverflowError);
        if (size == -1 && PyErr_Occurred()) {
            if (!PyErr_ExceptionMatches(PyExc_TypeError))
                return NULL;
            PyErr_Clear();  /* fall through */
        }
        else {
            if (size < 0) {
                PyErr_SetString(PyExc_ValueError, "negative count");
                return NULL;
            }
            new = _PyBytes_FromSize(size, 1);
            if (new == NULL)
                return NULL;
            return new;
        }
    }

    return PyBytes_FromObject(x);
}

static PyObject*
_PyBytes_FromBuffer(PyObject *x)
{
    PyObject *new;
    Py_buffer view;

    if (PyObject_GetBuffer(x, &view, PyBUF_FULL_RO) < 0)
        return NULL;

    new = PyBytes_FromStringAndSize(NULL, view.len);
    if (!new)
        goto fail;
    if (PyBuffer_ToContiguous(((PyBytesObject *)new)->ob_sval,
                &view, view.len, 'C') < 0)
        goto fail;
    PyBuffer_Release(&view);
    return new;

fail:
    Py_XDECREF(new);
    PyBuffer_Release(&view);
    return NULL;
}

static PyObject*
_PyBytes_FromList(PyObject *x)
{
    Py_ssize_t i, size = PyList_GET_SIZE(x);
    Py_ssize_t value;
    char *str;
    PyObject *item;
    _PyBytesWriter writer;

    _PyBytesWriter_Init(&writer);
    str = _PyBytesWriter_Alloc(&writer, size);
    if (str == NULL)
        return NULL;
    writer.overallocate = 1;
    size = writer.allocated;

    for (i = 0; i < PyList_GET_SIZE(x); i++) {
        item = PyList_GET_ITEM(x, i);
        Py_INCREF(item);
        value = PyNumber_AsSsize_t(item, NULL);
        Py_DECREF(item);
        if (value == -1 && PyErr_Occurred())
            goto error;

        if (value < 0 || value >= 256) {
            PyErr_SetString(PyExc_ValueError,
                            "bytes must be in range(0, 256)");
            goto error;
        }

        if (i >= size) {
            str = _PyBytesWriter_Resize(&writer, str, size+1);
            if (str == NULL)
                return NULL;
            size = writer.allocated;
        }
        *str++ = (char) value;
    }
    return _PyBytesWriter_Finish(&writer, str);

  error:
    _PyBytesWriter_Dealloc(&writer);
    return NULL;
}

static PyObject*
_PyBytes_FromTuple(PyObject *x)
{
    PyObject *bytes;
    Py_ssize_t i, size = PyTuple_GET_SIZE(x);
    Py_ssize_t value;
    char *str;
    PyObject *item;

    bytes = PyBytes_FromStringAndSize(NULL, size);
    if (bytes == NULL)
        return NULL;
    str = ((PyBytesObject *)bytes)->ob_sval;

    for (i = 0; i < size; i++) {
        item = PyTuple_GET_ITEM(x, i);
        value = PyNumber_AsSsize_t(item, NULL);
        if (value == -1 && PyErr_Occurred())
            goto error;

        if (value < 0 || value >= 256) {
            PyErr_SetString(PyExc_ValueError,
                            "bytes must be in range(0, 256)");
            goto error;
        }
        *str++ = (char) value;
    }
    return bytes;

  error:
    Py_DECREF(bytes);
    return NULL;
}

static PyObject *
_PyBytes_FromIterator(PyObject *it, PyObject *x)
{
    char *str;
    Py_ssize_t i, size;
    _PyBytesWriter writer;

    /* For iterator version, create a string object and resize as needed */
    size = PyObject_LengthHint(x, 64);
    if (size == -1 && PyErr_Occurred())
        return NULL;

    _PyBytesWriter_Init(&writer);
    str = _PyBytesWriter_Alloc(&writer, size);
    if (str == NULL)
        return NULL;
    writer.overallocate = 1;
    size = writer.allocated;

    /* Run the iterator to exhaustion */
    for (i = 0; ; i++) {
        PyObject *item;
        Py_ssize_t value;

        /* Get the next item */
        item = PyIter_Next(it);
        if (item == NULL) {
            if (PyErr_Occurred())
                goto error;
            break;
        }

        /* Interpret it as an int (__index__) */
        value = PyNumber_AsSsize_t(item, NULL);
        Py_DECREF(item);
        if (value == -1 && PyErr_Occurred())
            goto error;

        /* Range check */
        if (value < 0 || value >= 256) {
            PyErr_SetString(PyExc_ValueError,
                            "bytes must be in range(0, 256)");
            goto error;
        }

        /* Append the byte */
        if (i >= size) {
            str = _PyBytesWriter_Resize(&writer, str, size+1);
            if (str == NULL)
                return NULL;
            size = writer.allocated;
        }
        *str++ = (char) value;
    }

    return _PyBytesWriter_Finish(&writer, str);

  error:
    _PyBytesWriter_Dealloc(&writer);
    return NULL;
}

PyObject *
PyBytes_FromObject(PyObject *x)
{
    PyObject *it, *result;

    if (x == NULL) {
        PyErr_BadInternalCall();
        return NULL;
    }

    if (PyBytes_CheckExact(x)) {
        Py_INCREF(x);
        return x;
    }

    /* Use the modern buffer interface */
    if (PyObject_CheckBuffer(x))
        return _PyBytes_FromBuffer(x);

    if (PyList_CheckExact(x))
        return _PyBytes_FromList(x);

    if (PyTuple_CheckExact(x))
        return _PyBytes_FromTuple(x);

    if (!PyUnicode_Check(x)) {
        it = PyObject_GetIter(x);
        if (it != NULL) {
            result = _PyBytes_FromIterator(it, x);
            Py_DECREF(it);
            return result;
        }
        if (!PyErr_ExceptionMatches(PyExc_TypeError)) {
            return NULL;
        }
    }

    PyErr_Format(PyExc_TypeError,
                 "cannot convert '%.200s' object to bytes",
                 x->ob_type->tp_name);
    return NULL;
}

static PyObject *
bytes_subtype_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *tmp, *pnew;
    Py_ssize_t n;

    assert(PyType_IsSubtype(type, &PyBytes_Type));
    tmp = bytes_new(&PyBytes_Type, args, kwds);
    if (tmp == NULL)
        return NULL;
    assert(PyBytes_Check(tmp));
    n = PyBytes_GET_SIZE(tmp);
    pnew = type->tp_alloc(type, n);
    if (pnew != NULL) {
        memcpy(PyBytes_AS_STRING(pnew),
                  PyBytes_AS_STRING(tmp), n+1);
        ((PyBytesObject *)pnew)->ob_shash =
            ((PyBytesObject *)tmp)->ob_shash;
    }
    Py_DECREF(tmp);
    return pnew;
}

PyDoc_STRVAR(bytes_doc,
"bytes(iterable_of_ints) -> bytes\n\
bytes(string, encoding[, errors]) -> bytes\n\
bytes(bytes_or_buffer) -> immutable copy of bytes_or_buffer\n\
bytes(int) -> bytes object of size given by the parameter initialized with null bytes\n\
bytes() -> empty bytes object\n\
\n\
Construct an immutable array of bytes from:\n\
  - an iterable yielding integers in range(256)\n\
  - a text string encoded using the specified encoding\n\
  - any object implementing the buffer API.\n\
  - an integer");

static PyObject *bytes_iter(PyObject *seq);

PyTypeObject PyBytes_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "bytes",
    PyBytesObject_SIZE,
    sizeof(char),
    bytes_dealloc,                      /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    (reprfunc)bytes_repr,                       /* tp_repr */
    &bytes_as_number,                           /* tp_as_number */
    &bytes_as_sequence,                         /* tp_as_sequence */
    &bytes_as_mapping,                          /* tp_as_mapping */
    (hashfunc)bytes_hash,                       /* tp_hash */
    0,                                          /* tp_call */
    bytes_str,                                  /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    &bytes_as_buffer,                           /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE |
        Py_TPFLAGS_BYTES_SUBCLASS,              /* tp_flags */
    bytes_doc,                                  /* tp_doc */
    0,                                          /* tp_traverse */
    0,                                          /* tp_clear */
    (richcmpfunc)bytes_richcompare,             /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    bytes_iter,                                 /* tp_iter */
    0,                                          /* tp_iternext */
    bytes_methods,                              /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    &PyBaseObject_Type,                         /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    0,                                          /* tp_alloc */
    bytes_new,                                  /* tp_new */
    PyObject_Del,                               /* tp_free */
};

void
PyBytes_Concat(PyObject **pv, PyObject *w)
{
    assert(pv != NULL);
    if (*pv == NULL)
        return;
    if (w == NULL) {
        Py_CLEAR(*pv);
        return;
    }

    if (Py_REFCNT(*pv) == 1 && PyBytes_CheckExact(*pv)) {
        /* Only one reference, so we can resize in place */
        Py_ssize_t oldsize;
        Py_buffer wb;

        wb.len = -1;
        if (PyObject_GetBuffer(w, &wb, PyBUF_SIMPLE) != 0) {
            PyErr_Format(PyExc_TypeError, "can't concat %.100s to %.100s",
                         Py_TYPE(w)->tp_name, Py_TYPE(*pv)->tp_name);
            Py_CLEAR(*pv);
            return;
        }

        oldsize = PyBytes_GET_SIZE(*pv);
        if (oldsize > PY_SSIZE_T_MAX - wb.len) {
            PyErr_NoMemory();
            goto error;
        }
        if (_PyBytes_Resize(pv, oldsize + wb.len) < 0)
            goto error;

        memcpy(PyBytes_AS_STRING(*pv) + oldsize, wb.buf, wb.len);
        PyBuffer_Release(&wb);
        return;

      error:
        PyBuffer_Release(&wb);
        Py_CLEAR(*pv);
        return;
    }

    else {
        /* Multiple references, need to create new object */
        PyObject *v;
        v = bytes_concat(*pv, w);
        Py_SETREF(*pv, v);
    }
}

void
PyBytes_ConcatAndDel(PyObject **pv, PyObject *w)
{
    PyBytes_Concat(pv, w);
    Py_XDECREF(w);
}


/* The following function breaks the notion that bytes are immutable:
   it changes the size of a bytes object.  We get away with this only if there
   is only one module referencing the object.  You can also think of it
   as creating a new bytes object and destroying the old one, only
   more efficiently.  In any case, don't use this if the bytes object may
   already be known to some other part of the code...
   Note that if there's not enough memory to resize the bytes object, the
   original bytes object at *pv is deallocated, *pv is set to NULL, an "out of
   memory" exception is set, and -1 is returned.  Else (on success) 0 is
   returned, and the value in *pv may or may not be the same as on input.
   As always, an extra byte is allocated for a trailing \0 byte (newsize
   does *not* include that), and a trailing \0 byte is stored.
*/

int
_PyBytes_Resize(PyObject **pv, Py_ssize_t newsize)
{
    PyObject *v;
    PyBytesObject *sv;
    v = *pv;
    if (!PyBytes_Check(v) || newsize < 0) {
        goto error;
    }
    if (Py_SIZE(v) == newsize) {
        /* return early if newsize equals to v->ob_size */
        return 0;
    }
    if (Py_REFCNT(v) != 1) {
        goto error;
    }
    /* XXX UNREF/NEWREF interface should be more symmetrical */
    _Py_DEC_REFTOTAL;
    _Py_ForgetReference(v);
    *pv = (PyObject *)
        PyObject_REALLOC(v, PyBytesObject_SIZE + newsize);
    if (*pv == NULL) {
        PyObject_Del(v);
        PyErr_NoMemory();
        return -1;
    }
    _Py_NewReference(*pv);
    sv = (PyBytesObject *) *pv;
    Py_SIZE(sv) = newsize;
    sv->ob_sval[newsize] = '\0';
    sv->ob_shash = -1;          /* invalidate cached hash value */
    return 0;
error:
    *pv = 0;
    Py_DECREF(v);
    PyErr_BadInternalCall();
    return -1;
}

void
PyBytes_Fini(void)
{
    int i;
    for (i = 0; i < UCHAR_MAX + 1; i++)
        Py_CLEAR(characters[i]);
    Py_CLEAR(nullstring);
}

/*********************** Bytes Iterator ****************************/

typedef struct {
    PyObject_HEAD
    Py_ssize_t it_index;
    PyBytesObject *it_seq; /* Set to NULL when iterator is exhausted */
} striterobject;

static void
striter_dealloc(striterobject *it)
{
    _PyObject_GC_UNTRACK(it);
    Py_XDECREF(it->it_seq);
    PyObject_GC_Del(it);
}

static int
striter_traverse(striterobject *it, visitproc visit, void *arg)
{
    Py_VISIT(it->it_seq);
    return 0;
}

static PyObject *
striter_next(striterobject *it)
{
    PyBytesObject *seq;
    PyObject *item;

    assert(it != NULL);
    seq = it->it_seq;
    if (seq == NULL)
        return NULL;
    assert(PyBytes_Check(seq));

    if (it->it_index < PyBytes_GET_SIZE(seq)) {
        item = PyLong_FromLong(
            (unsigned char)seq->ob_sval[it->it_index]);
        if (item != NULL)
            ++it->it_index;
        return item;
    }

    it->it_seq = NULL;
    Py_DECREF(seq);
    return NULL;
}

static PyObject *
striter_len(striterobject *it)
{
    Py_ssize_t len = 0;
    if (it->it_seq)
        len = PyBytes_GET_SIZE(it->it_seq) - it->it_index;
    return PyLong_FromSsize_t(len);
}

PyDoc_STRVAR(length_hint_doc,
             "Private method returning an estimate of len(list(it)).");

static PyObject *
striter_reduce(striterobject *it)
{
    if (it->it_seq != NULL) {
        return Py_BuildValue("N(O)n", _PyObject_GetBuiltin("iter"),
                             it->it_seq, it->it_index);
    } else {
        PyObject *u = PyUnicode_FromUnicode(NULL, 0);
        if (u == NULL)
            return NULL;
        return Py_BuildValue("N(N)", _PyObject_GetBuiltin("iter"), u);
    }
}

PyDoc_STRVAR(reduce_doc, "Return state information for pickling.");

static PyObject *
striter_setstate(striterobject *it, PyObject *state)
{
    Py_ssize_t index = PyLong_AsSsize_t(state);
    if (index == -1 && PyErr_Occurred())
        return NULL;
    if (it->it_seq != NULL) {
        if (index < 0)
            index = 0;
        else if (index > PyBytes_GET_SIZE(it->it_seq))
            index = PyBytes_GET_SIZE(it->it_seq); /* iterator exhausted */
        it->it_index = index;
    }
    Py_RETURN_NONE;
}

PyDoc_STRVAR(setstate_doc, "Set state information for unpickling.");

static PyMethodDef striter_methods[] = {
    {"__length_hint__", (PyCFunction)striter_len, METH_NOARGS,
     length_hint_doc},
    {"__reduce__",      (PyCFunction)striter_reduce, METH_NOARGS,
     reduce_doc},
    {"__setstate__",    (PyCFunction)striter_setstate, METH_O,
     setstate_doc},
    {NULL,              NULL}           /* sentinel */
};

PyTypeObject PyBytesIter_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "bytes_iterator",                           /* tp_name */
    sizeof(striterobject),                      /* tp_basicsize */
    0,                                          /* tp_itemsize */
    /* methods */
    (destructor)striter_dealloc,                /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
    0,                                          /* tp_doc */
    (traverseproc)striter_traverse,     /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    PyObject_SelfIter,                          /* tp_iter */
    (iternextfunc)striter_next,                 /* tp_iternext */
    striter_methods,                            /* tp_methods */
    0,
};

static PyObject *
bytes_iter(PyObject *seq)
{
    striterobject *it;

    if (!PyBytes_Check(seq)) {
        PyErr_BadInternalCall();
        return NULL;
    }
    it = PyObject_GC_New(striterobject, &PyBytesIter_Type);
    if (it == NULL)
        return NULL;
    it->it_index = 0;
    Py_INCREF(seq);
    it->it_seq = (PyBytesObject *)seq;
    _PyObject_GC_TRACK(it);
    return (PyObject *)it;
}


/* _PyBytesWriter API */

#ifdef MS_WINDOWS
   /* On Windows, overallocate by 50% is the best factor */
#  define OVERALLOCATE_FACTOR 2
#else
   /* On Linux, overallocate by 25% is the best factor */
#  define OVERALLOCATE_FACTOR 4
#endif

void
_PyBytesWriter_Init(_PyBytesWriter *writer)
{
    /* Set all attributes before small_buffer to 0 */
    bzero(writer, offsetof(_PyBytesWriter, small_buffer));
#ifdef Py_DEBUG
    memset(writer->small_buffer, 0xCB, sizeof(writer->small_buffer));
#endif
}

void
_PyBytesWriter_Dealloc(_PyBytesWriter *writer)
{
    Py_CLEAR(writer->buffer);
}

Py_LOCAL_INLINE(char*)
_PyBytesWriter_AsString(_PyBytesWriter *writer)
{
    if (writer->use_small_buffer) {
        assert(writer->buffer == NULL);
        return writer->small_buffer;
    }
    else if (writer->use_bytearray) {
        assert(writer->buffer != NULL);
        return PyByteArray_AS_STRING(writer->buffer);
    }
    else {
        assert(writer->buffer != NULL);
        return PyBytes_AS_STRING(writer->buffer);
    }
}

Py_LOCAL_INLINE(Py_ssize_t)
_PyBytesWriter_GetSize(_PyBytesWriter *writer, char *str)
{
    char *start = _PyBytesWriter_AsString(writer);
    assert(str != NULL);
    assert(str >= start);
    assert(str - start <= writer->allocated);
    return str - start;
}

Py_LOCAL_INLINE(void)
_PyBytesWriter_CheckConsistency(_PyBytesWriter *writer, char *str)
{
#ifdef Py_DEBUG
    char *start, *end;

    if (writer->use_small_buffer) {
        assert(writer->buffer == NULL);
    }
    else {
        assert(writer->buffer != NULL);
        if (writer->use_bytearray)
            assert(PyByteArray_CheckExact(writer->buffer));
        else
            assert(PyBytes_CheckExact(writer->buffer));
        assert(Py_REFCNT(writer->buffer) == 1);
    }

    if (writer->use_bytearray) {
        /* bytearray has its own overallocation algorithm,
           writer overallocation must be disabled */
        assert(!writer->overallocate);
    }

    assert(0 <= writer->allocated);
    assert(0 <= writer->min_size && writer->min_size <= writer->allocated);
    /* the last byte must always be null */
    start = _PyBytesWriter_AsString(writer);
    assert(start[writer->allocated] == 0);

    end = start + writer->allocated;
    assert(str != NULL);
    assert(start <= str && str <= end);
#endif
}

void*
_PyBytesWriter_Resize(_PyBytesWriter *writer, void *str, Py_ssize_t size)
{
    Py_ssize_t allocated, pos;

    _PyBytesWriter_CheckConsistency(writer, str);
    assert(writer->allocated < size);

    allocated = size;
    if (writer->overallocate
        && allocated <= (PY_SSIZE_T_MAX - allocated / OVERALLOCATE_FACTOR)) {
        /* overallocate to limit the number of realloc() */
        allocated += allocated / OVERALLOCATE_FACTOR;
    }

    pos = _PyBytesWriter_GetSize(writer, str);
    if (!writer->use_small_buffer) {
        if (writer->use_bytearray) {
            if (PyByteArray_Resize(writer->buffer, allocated))
                goto error;
            /* writer->allocated can be smaller than writer->buffer->ob_alloc,
               but we cannot use ob_alloc because bytes may need to be moved
               to use the whole buffer. bytearray uses an internal optimization
               to avoid moving or copying bytes when bytes are removed at the
               beginning (ex: del bytearray[:1]). */
        }
        else {
            if (_PyBytes_Resize(&writer->buffer, allocated))
                goto error;
        }
    }
    else {
        /* convert from stack buffer to bytes object buffer */
        assert(writer->buffer == NULL);

        if (writer->use_bytearray)
            writer->buffer = PyByteArray_FromStringAndSize(NULL, allocated);
        else
            writer->buffer = PyBytes_FromStringAndSize(NULL, allocated);
        if (writer->buffer == NULL)
            goto error;

        if (pos != 0) {
            char *dest;
            if (writer->use_bytearray)
                dest = PyByteArray_AS_STRING(writer->buffer);
            else
                dest = PyBytes_AS_STRING(writer->buffer);
            memcpy(dest,
                      writer->small_buffer,
                      pos);
        }

        writer->use_small_buffer = 0;
#ifdef Py_DEBUG
        memset(writer->small_buffer, 0xDB, sizeof(writer->small_buffer));
#endif
    }
    writer->allocated = allocated;

    str = _PyBytesWriter_AsString(writer) + pos;
    _PyBytesWriter_CheckConsistency(writer, str);
    return str;

error:
    _PyBytesWriter_Dealloc(writer);
    return NULL;
}

void*
_PyBytesWriter_Prepare(_PyBytesWriter *writer, void *str, Py_ssize_t size)
{
    Py_ssize_t new_min_size;

    _PyBytesWriter_CheckConsistency(writer, str);
    assert(size >= 0);

    if (size == 0) {
        /* nothing to do */
        return str;
    }

    if (writer->min_size > PY_SSIZE_T_MAX - size) {
        PyErr_NoMemory();
        _PyBytesWriter_Dealloc(writer);
        return NULL;
    }
    new_min_size = writer->min_size + size;

    if (new_min_size > writer->allocated)
        str = _PyBytesWriter_Resize(writer, str, new_min_size);

    writer->min_size = new_min_size;
    return str;
}

/* Allocate the buffer to write size bytes.
   Return the pointer to the beginning of buffer data.
   Raise an exception and return NULL on error. */
void*
_PyBytesWriter_Alloc(_PyBytesWriter *writer, Py_ssize_t size)
{
    /* ensure that _PyBytesWriter_Alloc() is only called once */
    assert(writer->min_size == 0 && writer->buffer == NULL);
    assert(size >= 0);

    writer->use_small_buffer = 1;
#ifdef Py_DEBUG
    writer->allocated = sizeof(writer->small_buffer) - 1;
    /* In debug mode, don't use the full small buffer because it is less
       efficient than bytes and bytearray objects to detect buffer underflow
       and buffer overflow. Use 10 bytes of the small buffer to test also
       code using the smaller buffer in debug mode.

       Don't modify the _PyBytesWriter structure (use a shorter small buffer)
       in debug mode to also be able to detect stack overflow when running
       tests in debug mode. The _PyBytesWriter is large (more than 512 bytes),
       if Py_EnterRecursiveCall() is not used in deep C callback, we may hit a
       stack overflow. */
    writer->allocated = Py_MIN(writer->allocated, 10);
    /* _PyBytesWriter_CheckConsistency() requires the last byte to be 0,
       to detect buffer overflow */
    writer->small_buffer[writer->allocated] = 0;
#else
    writer->allocated = sizeof(writer->small_buffer);
#endif
    return _PyBytesWriter_Prepare(writer, writer->small_buffer, size);
}

PyObject *
_PyBytesWriter_Finish(_PyBytesWriter *writer, void *str)
{
    Py_ssize_t size;
    PyObject *result;

    _PyBytesWriter_CheckConsistency(writer, str);

    size = _PyBytesWriter_GetSize(writer, str);
    if (size == 0 && !writer->use_bytearray) {
        Py_CLEAR(writer->buffer);
        /* Get the empty byte string singleton */
        result = PyBytes_FromStringAndSize(NULL, 0);
    }
    else if (writer->use_small_buffer) {
        if (writer->use_bytearray) {
            result = PyByteArray_FromStringAndSize(writer->small_buffer, size);
        }
        else {
            result = PyBytes_FromStringAndSize(writer->small_buffer, size);
        }
    }
    else {
        result = writer->buffer;
        writer->buffer = NULL;

        if (size != writer->allocated) {
            if (writer->use_bytearray) {
                if (PyByteArray_Resize(result, size)) {
                    Py_DECREF(result);
                    return NULL;
                }
            }
            else {
                if (_PyBytes_Resize(&result, size)) {
                    assert(result == NULL);
                    return NULL;
                }
            }
        }
    }
    return result;
}

void*
_PyBytesWriter_WriteBytes(_PyBytesWriter *writer, void *ptr,
                          const void *bytes, Py_ssize_t size)
{
    char *str = (char *)ptr;

    str = _PyBytesWriter_Prepare(writer, str, size);
    if (str == NULL)
        return NULL;

    memcpy(str, bytes, size);
    str += size;

    return str;
}
