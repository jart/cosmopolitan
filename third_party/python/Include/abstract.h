#ifndef Py_ABSTRACTOBJECT_H
#define Py_ABSTRACTOBJECT_H
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/tupleobject.h"
COSMOPOLITAN_C_START_

/* Abstract Object Interface (many thanks to Jim Fulton) */

#define PyObject_DelAttrString(O, A) PyObject_SetAttrString((O), (A), NULL)
#define PyObject_DelAttr(O, A)       PyObject_SetAttr((O), (A), NULL)
PyObject *PyObject_Call(PyObject *callable_object, PyObject *args,
                        PyObject *kwargs);

#ifndef Py_LIMITED_API
PyObject *_PyStack_AsTuple(PyObject **stack, Py_ssize_t nargs);
PyObject *_PyStack_AsTupleSlice(PyObject **stack, Py_ssize_t nargs,
                                Py_ssize_t start, Py_ssize_t end);

PyObject *_PyStack_AsDict(PyObject **values, PyObject *kwnames);

int _PyStack_UnpackDict(
    PyObject **args,
    Py_ssize_t nargs,
    PyObject *kwargs,
    PyObject ***p_stack,
    PyObject **p_kwnames);

PyObject *_PyObject_FastCallDict(PyObject *func, PyObject **args,
                                 Py_ssize_t nargs, PyObject *kwargs);

PyObject *_PyObject_FastCallKeywords(PyObject *func, PyObject **args,
                                     Py_ssize_t nargs, PyObject *kwnames);

#define _PyObject_FastCall(func, args, nargs) \
  _PyObject_FastCallDict((func), (args), (nargs), NULL)

#define _PyObject_CallNoArg(func) _PyObject_FastCall((func), NULL, 0)

#define _PyObject_CallArg1(func, arg) _PyObject_FastCall((func), &(arg), 1)

PyObject *_PyObject_Call_Prepend(PyObject *func, PyObject *obj, PyObject *args,
                                 PyObject *kwargs);

#define _PY_FASTCALL_SMALL_STACK 5

int _PyObject_HasFastCall(PyObject *callable);

PyObject *_PyObject_FastCall_Prepend(
    PyObject *callable,
    PyObject *obj,
    PyObject **args,
    Py_ssize_t nargs);

#if IsModeDbg()
PyObject *_Py_CheckFunctionResult(PyObject *func, PyObject *result,
                                  const char *where);
#else
#define _Py_CheckFunctionResult(func, result, where) (result)
#endif
#endif /* Py_LIMITED_API */

PyObject *PyObject_CallObject(PyObject *callable_object, PyObject *args);

/*
Call a callable Python object, callable_object, with
arguments given by the tuple, args.  If no arguments are
needed, then args may be NULL.  Returns the result of the
call on success, or NULL on failure.  This is the equivalent
of the Python expression: o(*args).
*/

PyObject *PyObject_CallFunction(PyObject *callable_object, const char *format,
                                ...);

/*
Call a callable Python object, callable_object, with a
variable number of C arguments. The C arguments are described
using a mkvalue-style format string. The format may be NULL,
indicating that no arguments are provided.  Returns the
result of the call on success, or NULL on failure.  This is
the equivalent of the Python expression: o(*args).
*/

PyObject *PyObject_CallMethod(PyObject *o, const char *method,
                              const char *format, ...);

/*
Call the method named m of object o with a variable number of
C arguments.  The C arguments are described by a mkvalue
format string.  The format may be NULL, indicating that no
arguments are provided. Returns the result of the call on
success, or NULL on failure.  This is the equivalent of the
Python expression: o.method(args).
*/

#ifndef Py_LIMITED_API
PyObject *_PyObject_CallMethodId(PyObject *o, _Py_Identifier *method,
                                 const char *format, ...);
/*
  Like PyObject_CallMethod, but expect a _Py_Identifier* as the
  method name.
*/
#endif /* !Py_LIMITED_API */

#ifdef PY_SSIZE_T_CLEAN
#define PyObject_CallFunction _PyObject_CallFunction_SizeT
#define PyObject_CallMethod   _PyObject_CallMethod_SizeT
#ifndef Py_LIMITED_API
#define _PyObject_CallMethodId _PyObject_CallMethodId_SizeT
#endif /* !Py_LIMITED_API */
#endif

PyObject *_PyObject_CallFunction_SizeT(PyObject *callable, const char *format,
                                       ...);
PyObject *_PyObject_CallMethod_SizeT(PyObject *o, const char *name,
                                     const char *format, ...);
#ifndef Py_LIMITED_API
PyObject *_PyObject_CallMethodId_SizeT(PyObject *o, _Py_Identifier *name,
                                       const char *format, ...);
#endif /* !Py_LIMITED_API */

PyObject *PyObject_CallFunctionObjArgs(PyObject *callable, ...);

/*
Call a callable Python object, callable_object, with a
variable number of C arguments.  The C arguments are provided
as PyObject * values, terminated by a NULL.  Returns the
result of the call on success, or NULL on failure.  This is
the equivalent of the Python expression: o(*args).
*/

PyObject *PyObject_CallMethodObjArgs(PyObject *o, PyObject *method, ...);
#ifndef Py_LIMITED_API
PyObject *_PyObject_CallMethodIdObjArgs(PyObject *o,
                                        struct _Py_Identifier *method, ...);
#endif /* !Py_LIMITED_API */

/*
Call the method named m of object o with a variable number of
C arguments.  The C arguments are provided as PyObject *
values, terminated by NULL.  Returns the result of the call
on success, or NULL on failure.  This is the equivalent of
the Python expression: o.method(args).
*/

/* Implemented elsewhere:

long PyObject_Hash(PyObject *o);

Compute and return the hash, hash_value, of an object, o.  On
failure, return -1.  This is the equivalent of the Python
expression: hash(o).
  */

/* Implemented elsewhere:

int PyObject_IsTrue(PyObject *o);

Returns 1 if the object, o, is considered to be true, 0 if o is
considered to be false and -1 on failure. This is equivalent to the
Python expression: not not o
  */

/* Implemented elsewhere:

int PyObject_Not(PyObject *o);

Returns 0 if the object, o, is considered to be true, 1 if o is
considered to be false and -1 on failure. This is equivalent to the
Python expression: not o
  */

PyObject *PyObject_Type(PyObject *o);

/*
On success, returns a type object corresponding to the object
type of object o. On failure, returns NULL.  This is
equivalent to the Python expression: type(o).
*/

Py_ssize_t PyObject_Size(PyObject *o);

/*
Return the size of object o.  If the object, o, provides
both sequence and mapping protocols, the sequence size is
returned. On error, -1 is returned.  This is the equivalent
to the Python expression: len(o).
*/

/* For DLL compatibility */
#undef PyObject_Length
Py_ssize_t PyObject_Length(PyObject *o);
#define PyObject_Length PyObject_Size

#ifndef Py_LIMITED_API
int _PyObject_HasLen(PyObject *o);
Py_ssize_t PyObject_LengthHint(PyObject *o, Py_ssize_t);
#endif

/*
Guess the size of object o using len(o) or o.__length_hint__().
If neither of those return a non-negative value, then return the
default value.  If one of the calls fails, this function returns -1.
*/

PyObject *PyObject_GetItem(PyObject *o, PyObject *key);

/*
Return element of o corresponding to the object, key, or NULL
on failure. This is the equivalent of the Python expression:
o[key].
*/

int PyObject_SetItem(PyObject *o, PyObject *key, PyObject *v);

/*
Map the object key to the value v.  Raise an exception and return -1
on failure; return 0 on success.  This is the equivalent of the Python
statement o[key]=v.
*/

int PyObject_DelItemString(PyObject *o, const char *key);

/*
Remove the mapping for object, key, from the object *o.
Returns -1 on failure.  This is equivalent to
the Python statement: del o[key].
*/

int PyObject_DelItem(PyObject *o, PyObject *key);

/*
Delete the mapping for key from *o.  Returns -1 on failure.
This is the equivalent of the Python statement: del o[key].
*/

/* old buffer API
   FIXME:  usage of these should all be replaced in Python itself
   but for backwards compatibility we will implement them.
   Their usage without a corresponding "unlock" mechanism
   may create issues (but they would already be there). */

int PyObject_AsCharBuffer(PyObject *obj, const char **buffer,
                          Py_ssize_t *buffer_len);

/*
Takes an arbitrary object which must support the (character,
single segment) buffer interface and returns a pointer to a
read-only memory location useable as character based input
for subsequent processing.

0 is returned on success.  buffer and buffer_len are only
set in case no error occurs. Otherwise, -1 is returned and
an exception set.
*/

int PyObject_CheckReadBuffer(PyObject *obj);

/*
Checks whether an arbitrary object supports the (character,
single segment) buffer interface.  Returns 1 on success, 0
on failure.
*/

int PyObject_AsReadBuffer(PyObject *obj, const void **buffer,
                          Py_ssize_t *buffer_len);

/*
Same as PyObject_AsCharBuffer() except that this API expects
(readable, single segment) buffer interface and returns a
pointer to a read-only memory location which can contain
arbitrary data.

0 is returned on success.  buffer and buffer_len are only
set in case no error occurs.  Otherwise, -1 is returned and
an exception set.
*/

int PyObject_AsWriteBuffer(PyObject *obj, void **buffer,
                           Py_ssize_t *buffer_len);

/*
Takes an arbitrary object which must support the (writable,
single segment) buffer interface and returns a pointer to a
writable memory location in buffer of size buffer_len.

0 is returned on success.  buffer and buffer_len are only
set in case no error occurs. Otherwise, -1 is returned and
an exception set.
*/

/* new buffer API */

#ifndef Py_LIMITED_API
#define PyObject_CheckBuffer(obj)            \
  (((obj)->ob_type->tp_as_buffer != NULL) && \
   ((obj)->ob_type->tp_as_buffer->bf_getbuffer != NULL))

/* Return 1 if the getbuffer function is available, otherwise
   return 0 */

int PyObject_GetBuffer(PyObject *obj, Py_buffer *view, int flags);

/* This is a C-API version of the getbuffer function call.  It checks
   to make sure object has the required function pointer and issues the
   call.  Returns -1 and raises an error on failure and returns 0 on
   success
*/

void *PyBuffer_GetPointer(Py_buffer *view, Py_ssize_t *indices);

/* Get the memory area pointed to by the indices for the buffer given.
   Note that view->ndim is the assumed size of indices
*/

int PyBuffer_SizeFromFormat(const char *);

/* Return the implied itemsize of the data-format area from a
   struct-style description */

/* Implementation in memoryobject.c */
int PyBuffer_ToContiguous(void *buf, Py_buffer *view, Py_ssize_t len,
                          char order);

int PyBuffer_FromContiguous(Py_buffer *view, void *buf, Py_ssize_t len,
                            char order);

/* Copy len bytes of data from the contiguous chunk of memory
   pointed to by buf into the buffer exported by obj.  Return
   0 on success and return -1 and raise a PyBuffer_Error on
   error (i.e. the object does not have a buffer interface or
   it is not working).

   If fort is 'F', then if the object is multi-dimensional,
   then the data will be copied into the array in
   Fortran-style (first dimension varies the fastest).  If
   fort is 'C', then the data will be copied into the array
   in C-style (last dimension varies the fastest).  If fort
   is 'A', then it does not matter and the copy will be made
   in whatever way is more efficient.

*/

int PyObject_CopyData(PyObject *dest, PyObject *src);

/* Copy the data from the src buffer to the buffer of destination
 */

int PyBuffer_IsContiguous(const Py_buffer *view, char fort);

void PyBuffer_FillContiguousStrides(int ndims, Py_ssize_t *shape,
                                    Py_ssize_t *strides, int itemsize,
                                    char fort);

/*  Fill the strides array with byte-strides of a contiguous
    (Fortran-style if fort is 'F' or C-style otherwise)
    array of the given shape with the given number of bytes
    per element.
*/

int PyBuffer_FillInfo(Py_buffer *view, PyObject *o, void *buf, Py_ssize_t len,
                      int readonly, int flags);

/* Fills in a buffer-info structure correctly for an exporter
   that can only share a contiguous chunk of memory of
   "unsigned bytes" of the given length. Returns 0 on success
   and -1 (with raising an error) on error.
 */

void PyBuffer_Release(Py_buffer *view);

/* Releases a Py_buffer obtained from getbuffer ParseTuple's s*.
 */
#endif /* Py_LIMITED_API */

PyObject *PyObject_Format(PyObject *obj, PyObject *format_spec);
/*
Takes an arbitrary object and returns the result of
calling obj.__format__(format_spec).
*/

/* Iterators */

PyObject *PyObject_GetIter(PyObject *);
/* Takes an object and returns an iterator for it.
This is typically a new iterator but if the argument
is an iterator, this returns itself. */

#define PyIter_Check(obj)                 \
  ((obj)->ob_type->tp_iternext != NULL && \
   (obj)->ob_type->tp_iternext != &_PyObject_NextNotImplemented)

PyObject *PyIter_Next(PyObject *);
/* Takes an iterator object and calls its tp_iternext slot,
returning the next value.  If the iterator is exhausted,
this returns NULL without setting an exception.
NULL with an exception means an error occurred. */

/*  Number Protocol:*/

int PyNumber_Check(PyObject *o);

/*
Returns 1 if the object, o, provides numeric protocols, and
false otherwise.

This function always succeeds.
*/

PyObject *PyNumber_Add(PyObject *o1, PyObject *o2);

/*
Returns the result of adding o1 and o2, or null on failure.
This is the equivalent of the Python expression: o1+o2.
*/

PyObject *PyNumber_Subtract(PyObject *o1, PyObject *o2);

/*
Returns the result of subtracting o2 from o1, or null on
failure.  This is the equivalent of the Python expression:
o1-o2.
*/

PyObject *PyNumber_Multiply(PyObject *o1, PyObject *o2);

/*
Returns the result of multiplying o1 and o2, or null on
failure.  This is the equivalent of the Python expression:
o1*o2.
*/

#if !defined(Py_LIMITED_API) || Py_LIMITED_API + 0 >= 0x03050000
PyObject *PyNumber_MatrixMultiply(PyObject *o1, PyObject *o2);

/*
This is the equivalent of the Python expression: o1 @ o2.
*/
#endif

PyObject *PyNumber_FloorDivide(PyObject *o1, PyObject *o2);

/*
Returns the result of dividing o1 by o2 giving an integral result,
or null on failure.
This is the equivalent of the Python expression: o1//o2.
*/

PyObject *PyNumber_TrueDivide(PyObject *o1, PyObject *o2);

/*
Returns the result of dividing o1 by o2 giving a float result,
or null on failure.
This is the equivalent of the Python expression: o1/o2.
*/

PyObject *PyNumber_Remainder(PyObject *o1, PyObject *o2);

/*
Returns the remainder of dividing o1 by o2, or null on
failure.  This is the equivalent of the Python expression:
o1%o2.
*/

PyObject *PyNumber_Divmod(PyObject *o1, PyObject *o2);

/*
See the built-in function divmod.  Returns NULL on failure.
This is the equivalent of the Python expression:
divmod(o1,o2).
*/

PyObject *PyNumber_Power(PyObject *o1, PyObject *o2, PyObject *o3);

/*
See the built-in function pow.  Returns NULL on failure.
This is the equivalent of the Python expression:
pow(o1,o2,o3), where o3 is optional.
*/

PyObject *PyNumber_Negative(PyObject *o);

/*
Returns the negation of o on success, or null on failure.
This is the equivalent of the Python expression: -o.
*/

PyObject *PyNumber_Positive(PyObject *o);

/*
Returns the (what?) of o on success, or NULL on failure.
This is the equivalent of the Python expression: +o.
*/

PyObject *PyNumber_Absolute(PyObject *o);

/*
Returns the absolute value of o, or null on failure.  This is
the equivalent of the Python expression: abs(o).
*/

PyObject *PyNumber_Invert(PyObject *o);

/*
Returns the bitwise negation of o on success, or NULL on
failure.  This is the equivalent of the Python expression:
~o.
*/

PyObject *PyNumber_Lshift(PyObject *o1, PyObject *o2);

/*
Returns the result of left shifting o1 by o2 on success, or
NULL on failure.  This is the equivalent of the Python
expression: o1 << o2.
*/

PyObject *PyNumber_Rshift(PyObject *o1, PyObject *o2);

/*
Returns the result of right shifting o1 by o2 on success, or
NULL on failure.  This is the equivalent of the Python
expression: o1 >> o2.
*/

PyObject *PyNumber_And(PyObject *o1, PyObject *o2);

/*
Returns the result of bitwise and of o1 and o2 on success, or
NULL on failure. This is the equivalent of the Python
expression: o1&o2.

*/

PyObject *PyNumber_Xor(PyObject *o1, PyObject *o2);

/*
Returns the bitwise exclusive or of o1 by o2 on success, or
NULL on failure.  This is the equivalent of the Python
expression: o1^o2.
*/

PyObject *PyNumber_Or(PyObject *o1, PyObject *o2);

/*
Returns the result of bitwise or on o1 and o2 on success, or
NULL on failure.  This is the equivalent of the Python
expression: o1|o2.
*/

#define PyIndex_Check(obj)                 \
  ((obj)->ob_type->tp_as_number != NULL && \
   (obj)->ob_type->tp_as_number->nb_index != NULL)

PyObject *PyNumber_Index(PyObject *o);

/*
Returns the object converted to a Python int
or NULL with an error raised on failure.
*/

Py_ssize_t PyNumber_AsSsize_t(PyObject *o, PyObject *exc);

/*
Returns the object converted to Py_ssize_t by going through
PyNumber_Index first.  If an overflow error occurs while
converting the int to Py_ssize_t, then the second argument
is the error-type to return.  If it is NULL, then the overflow error
is cleared and the value is clipped.
*/

PyObject *PyNumber_Long(PyObject *o);

/*
Returns the o converted to an integer object on success, or
NULL on failure.  This is the equivalent of the Python
expression: int(o).
*/

PyObject *PyNumber_Float(PyObject *o);

/*
Returns the o converted to a float object on success, or NULL
on failure.  This is the equivalent of the Python expression:
float(o).
*/

/*  In-place variants of (some of) the above number protocol functions */

PyObject *PyNumber_InPlaceAdd(PyObject *o1, PyObject *o2);

/*
Returns the result of adding o2 to o1, possibly in-place, or null
on failure.  This is the equivalent of the Python expression:
o1 += o2.
*/

PyObject *PyNumber_InPlaceSubtract(PyObject *o1, PyObject *o2);

/*
Returns the result of subtracting o2 from o1, possibly in-place or
null on failure.  This is the equivalent of the Python expression:
o1 -= o2.
*/

PyObject *PyNumber_InPlaceMultiply(PyObject *o1, PyObject *o2);

/*
Returns the result of multiplying o1 by o2, possibly in-place, or
null on failure.  This is the equivalent of the Python expression:
o1 *= o2.
*/

#if !defined(Py_LIMITED_API) || Py_LIMITED_API + 0 >= 0x03050000
PyObject *PyNumber_InPlaceMatrixMultiply(PyObject *o1, PyObject *o2);

/*
This is the equivalent of the Python expression: o1 @= o2.
*/
#endif

PyObject *PyNumber_InPlaceFloorDivide(PyObject *o1, PyObject *o2);

/*
Returns the result of dividing o1 by o2 giving an integral result,
possibly in-place, or null on failure.
This is the equivalent of the Python expression:
o1 /= o2.
*/

PyObject *PyNumber_InPlaceTrueDivide(PyObject *o1, PyObject *o2);

/*
Returns the result of dividing o1 by o2 giving a float result,
possibly in-place, or null on failure.
This is the equivalent of the Python expression:
o1 /= o2.
*/

PyObject *PyNumber_InPlaceRemainder(PyObject *o1, PyObject *o2);

/*
Returns the remainder of dividing o1 by o2, possibly in-place, or
null on failure.  This is the equivalent of the Python expression:
o1 %= o2.
*/

PyObject *PyNumber_InPlacePower(PyObject *o1, PyObject *o2, PyObject *o3);

/*
Returns the result of raising o1 to the power of o2, possibly
in-place, or null on failure.  This is the equivalent of the Python
expression: o1 **= o2, or pow(o1, o2, o3) if o3 is present.
*/

PyObject *PyNumber_InPlaceLshift(PyObject *o1, PyObject *o2);

/*
Returns the result of left shifting o1 by o2, possibly in-place, or
null on failure.  This is the equivalent of the Python expression:
o1 <<= o2.
*/

PyObject *PyNumber_InPlaceRshift(PyObject *o1, PyObject *o2);

/*
Returns the result of right shifting o1 by o2, possibly in-place or
null on failure.  This is the equivalent of the Python expression:
o1 >>= o2.
*/

PyObject *PyNumber_InPlaceAnd(PyObject *o1, PyObject *o2);

/*
Returns the result of bitwise and of o1 and o2, possibly in-place,
or null on failure. This is the equivalent of the Python
expression: o1 &= o2.
*/

PyObject *PyNumber_InPlaceXor(PyObject *o1, PyObject *o2);

/*
Returns the bitwise exclusive or of o1 by o2, possibly in-place, or
null on failure.  This is the equivalent of the Python expression:
o1 ^= o2.
*/

PyObject *PyNumber_InPlaceOr(PyObject *o1, PyObject *o2);

/*
Returns the result of bitwise or of o1 and o2, possibly in-place,
or null on failure.  This is the equivalent of the Python
expression: o1 |= o2.
*/

PyObject *PyNumber_ToBase(PyObject *n, int base);

/*
Returns the integer n converted to a string with a base, with a base
marker of 0b, 0o or 0x prefixed if applicable.
If n is not an int object, it is converted with PyNumber_Index first.
*/

/*  Sequence protocol:*/

int PySequence_Check(PyObject *o);

/*
Return 1 if the object provides sequence protocol, and zero
otherwise.

This function always succeeds.
*/

Py_ssize_t PySequence_Size(PyObject *o);

/*
Return the size of sequence object o, or -1 on failure.
*/

/* For DLL compatibility */
#undef PySequence_Length
Py_ssize_t PySequence_Length(PyObject *o);
#define PySequence_Length PySequence_Size

PyObject *PySequence_Concat(PyObject *o1, PyObject *o2);

/*
Return the concatenation of o1 and o2 on success, and NULL on
failure.   This is the equivalent of the Python
expression: o1+o2.
*/

PyObject *PySequence_Repeat(PyObject *o, Py_ssize_t count);

/*
Return the result of repeating sequence object o count times,
or NULL on failure.  This is the equivalent of the Python
expression: o1*count.
*/

PyObject *PySequence_GetItem(PyObject *o, Py_ssize_t i);

/*
Return the ith element of o, or NULL on failure. This is the
equivalent of the Python expression: o[i].
*/

PyObject *PySequence_GetSlice(PyObject *o, Py_ssize_t i1, Py_ssize_t i2);

/*
Return the slice of sequence object o between i1 and i2, or
NULL on failure. This is the equivalent of the Python
expression: o[i1:i2].
*/

int PySequence_SetItem(PyObject *o, Py_ssize_t i, PyObject *v);

/*
Assign object v to the ith element of o.  Raise an exception and return
-1 on failure; return 0 on success.  This is the equivalent of the
Python statement o[i]=v.
*/

int PySequence_DelItem(PyObject *o, Py_ssize_t i);

/*
Delete the ith element of object v.  Returns
-1 on failure.  This is the equivalent of the Python
statement: del o[i].
*/

int PySequence_SetSlice(PyObject *o, Py_ssize_t i1, Py_ssize_t i2, PyObject *v);

/*
Assign the sequence object, v, to the slice in sequence
object, o, from i1 to i2.  Returns -1 on failure. This is the
equivalent of the Python statement: o[i1:i2]=v.
*/

int PySequence_DelSlice(PyObject *o, Py_ssize_t i1, Py_ssize_t i2);

/*
Delete the slice in sequence object, o, from i1 to i2.
Returns -1 on failure. This is the equivalent of the Python
statement: del o[i1:i2].
*/

PyObject *PySequence_Tuple(PyObject *o);

/*
Returns the sequence, o, as a tuple on success, and NULL on failure.
This is equivalent to the Python expression: tuple(o)
*/

PyObject *PySequence_List(PyObject *o);
/*
Returns the sequence, o, as a list on success, and NULL on failure.
This is equivalent to the Python expression: list(o)
*/

PyObject *PySequence_Fast(PyObject *o, const char *m);
/*
Return the sequence, o, as a list, unless it's already a
tuple or list.  Use PySequence_Fast_GET_ITEM to access the
members of this list, and PySequence_Fast_GET_SIZE to get its length.

Returns NULL on failure.  If the object does not support iteration,
raises a TypeError exception with m as the message text.
*/

#define PySequence_Fast_GET_SIZE(o) \
  (PyList_Check(o) ? PyList_GET_SIZE(o) : PyTuple_GET_SIZE(o))
/*
Return the size of o, assuming that o was returned by
PySequence_Fast and is not NULL.
*/

#define PySequence_Fast_GET_ITEM(o, i) \
  (PyList_Check(o) ? PyList_GET_ITEM(o, i) : PyTuple_GET_ITEM(o, i))
/*
Return the ith element of o, assuming that o was returned by
PySequence_Fast, and that i is within bounds.
*/

#define PySequence_ITEM(o, i) (Py_TYPE(o)->tp_as_sequence->sq_item(o, i))
/* Assume tp_as_sequence and sq_item exist and that i does not
need to be corrected for a negative index
*/

#define PySequence_Fast_ITEMS(sf)                     \
  (PyList_Check(sf) ? ((PyListObject *)(sf))->ob_item \
                    : ((PyTupleObject *)(sf))->ob_item)
/* Return a pointer to the underlying item array for
   an object retured by PySequence_Fast */

Py_ssize_t PySequence_Count(PyObject *o, PyObject *value);

/*
Return the number of occurrences on value on o, that is,
return the number of keys for which o[key]==value.  On
failure, return -1.  This is equivalent to the Python
expression: o.count(value).
*/

int PySequence_Contains(PyObject *seq, PyObject *ob);
/*
Return -1 if error; 1 if ob in seq; 0 if ob not in seq.
Use __contains__ if possible, else _PySequence_IterSearch().
*/

#ifndef Py_LIMITED_API
#define PY_ITERSEARCH_COUNT    1
#define PY_ITERSEARCH_INDEX    2
#define PY_ITERSEARCH_CONTAINS 3
Py_ssize_t _PySequence_IterSearch(PyObject *seq, PyObject *obj, int operation);
#endif
/*
  Iterate over seq.  Result depends on the operation:
  PY_ITERSEARCH_COUNT:  return # of times obj appears in seq; -1 if
    error.
  PY_ITERSEARCH_INDEX:  return 0-based index of first occurrence of
    obj in seq; set ValueError and return -1 if none found;
    also return -1 on error.
  PY_ITERSEARCH_CONTAINS:  return 1 if obj in seq, else 0; -1 on
    error.
*/

/* For DLL-level backwards compatibility */
#undef PySequence_In
int PySequence_In(PyObject *o, PyObject *value);

/* For source-level backwards compatibility */
#define PySequence_In PySequence_Contains

/*
Determine if o contains value.  If an item in o is equal to
X, return 1, otherwise return 0.  On error, return -1.  This
is equivalent to the Python expression: value in o.
*/

Py_ssize_t PySequence_Index(PyObject *o, PyObject *value);

/*
Return the first index for which o[i]=value.  On error,
return -1.    This is equivalent to the Python
expression: o.index(value).
*/

/* In-place versions of some of the above Sequence functions. */

PyObject *PySequence_InPlaceConcat(PyObject *o1, PyObject *o2);

/*
Append o2 to o1, in-place when possible. Return the resulting
object, which could be o1, or NULL on failure.  This is the
equivalent of the Python expression: o1 += o2.

*/

PyObject *PySequence_InPlaceRepeat(PyObject *o, Py_ssize_t count);

/*
Repeat o1 by count, in-place when possible. Return the resulting
object, which could be o1, or NULL on failure.  This is the
equivalent of the Python expression: o1 *= count.

*/

/*  Mapping protocol:*/

int PyMapping_Check(PyObject *o);

/*
Return 1 if the object provides mapping protocol, and zero
otherwise.

This function always succeeds.
*/

Py_ssize_t PyMapping_Size(PyObject *o);

/*
Returns the number of keys in object o on success, and -1 on
failure.  For objects that do not provide sequence protocol,
this is equivalent to the Python expression: len(o).
*/

/* For DLL compatibility */
#undef PyMapping_Length
Py_ssize_t PyMapping_Length(PyObject *o);
#define PyMapping_Length PyMapping_Size

/* implemented as a macro:

int PyMapping_DelItemString(PyObject *o, const char *key);

Remove the mapping for object, key, from the object *o.
Returns -1 on failure.  This is equivalent to
the Python statement: del o[key].
  */
#define PyMapping_DelItemString(O, K) PyObject_DelItemString((O), (K))

/* implemented as a macro:

int PyMapping_DelItem(PyObject *o, PyObject *key);

Remove the mapping for object, key, from the object *o.
Returns -1 on failure.  This is equivalent to
the Python statement: del o[key].
  */
#define PyMapping_DelItem(O, K) PyObject_DelItem((O), (K))

int PyMapping_HasKeyString(PyObject *o, const char *key);

/*
On success, return 1 if the mapping object has the key, key,
and 0 otherwise.  This is equivalent to the Python expression:
key in o.

This function always succeeds.
*/

int PyMapping_HasKey(PyObject *o, PyObject *key);

/*
Return 1 if the mapping object has the key, key,
and 0 otherwise.  This is equivalent to the Python expression:
key in o.

This function always succeeds.

*/

PyObject *PyMapping_Keys(PyObject *o);

/*
On success, return a list or tuple of the keys in object o.
On failure, return NULL.
*/

PyObject *PyMapping_Values(PyObject *o);

/*
On success, return a list or tuple of the values in object o.
On failure, return NULL.
*/

PyObject *PyMapping_Items(PyObject *o);

/*
On success, return a list or tuple of the items in object o,
where each item is a tuple containing a key-value pair.
On failure, return NULL.

*/

PyObject *PyMapping_GetItemString(PyObject *o, const char *key);

/*
Return element of o corresponding to the object, key, or NULL
on failure. This is the equivalent of the Python expression:
o[key].
*/

int PyMapping_SetItemString(PyObject *o, const char *key, PyObject *value);

/*
Map the object, key, to the value, v.  Returns
-1 on failure.  This is the equivalent of the Python
statement: o[key]=v.
*/

int PyObject_IsInstance(PyObject *object, PyObject *typeorclass);
/* isinstance(object, typeorclass) */

int PyObject_IsSubclass(PyObject *object, PyObject *typeorclass);
/* issubclass(object, typeorclass) */

#ifndef Py_LIMITED_API
int _PyObject_RealIsInstance(PyObject *inst, PyObject *cls);

int _PyObject_RealIsSubclass(PyObject *derived, PyObject *cls);

char *const *_PySequence_BytesToCharpArray(PyObject *self);

void _Py_FreeCharPArray(char *const array[]);

/* For internal use by buffer API functions */
void _Py_add_one_to_index_F(int nd, Py_ssize_t *index, const Py_ssize_t *shape);
void _Py_add_one_to_index_C(int nd, Py_ssize_t *index, const Py_ssize_t *shape);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* Py_ABSTRACTOBJECT_H */
