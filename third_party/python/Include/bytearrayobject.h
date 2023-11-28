#ifndef Py_BYTEARRAYOBJECT_H
#define Py_BYTEARRAYOBJECT_H
#include "libc/assert.h"
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

/* Type PyByteArrayObject represents a mutable array of bytes.
 * The Python API is that of a sequence;
 * the bytes are mapped to ints in [0, 256).
 * Bytes are not characters; they may be used to encode characters.
 * The only way to go between bytes and str/unicode is via encoding
 * and decoding.
 * For the convenience of C programmers, the bytes type is considered
 * to contain a char pointer, not an unsigned char pointer.
 */

/* Object layout */
#ifndef Py_LIMITED_API
typedef struct {
    PyObject_VAR_HEAD
    Py_ssize_t ob_alloc; /* How many bytes allocated in ob_bytes */
    char *ob_bytes;      /* Physical backing buffer */
    char *ob_start;      /* Logical start inside ob_bytes */
    /* XXX(nnorwitz): should ob_exports be Py_ssize_t? */
    int ob_exports;      /* How many buffer exports */
} PyByteArrayObject;
#endif

/* Type object */
extern PyTypeObject PyByteArray_Type;
extern PyTypeObject PyByteArrayIter_Type;

/* Type check macros */
#define PyByteArray_Check(self) PyObject_TypeCheck(self, &PyByteArray_Type)
#define PyByteArray_CheckExact(self) (Py_TYPE(self) == &PyByteArray_Type)

/* Direct API functions */
PyObject * PyByteArray_FromObject(PyObject *);
PyObject * PyByteArray_Concat(PyObject *, PyObject *);
PyObject * PyByteArray_FromStringAndSize(const char *, Py_ssize_t);
Py_ssize_t PyByteArray_Size(PyObject *);
char * PyByteArray_AsString(PyObject *);
int PyByteArray_Resize(PyObject *, Py_ssize_t);

/* Macros, trading safety for speed */
#ifndef Py_LIMITED_API
#define PyByteArray_AS_STRING(self) \
    (assert(PyByteArray_Check(self)), \
     Py_SIZE(self) ? ((PyByteArrayObject *)(self))->ob_start : _PyByteArray_empty_string)
#define PyByteArray_GET_SIZE(self) (assert(PyByteArray_Check(self)), Py_SIZE(self))

extern char _PyByteArray_empty_string[];
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_BYTEARRAYOBJECT_H */
