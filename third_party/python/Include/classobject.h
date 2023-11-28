#ifndef Py_LIMITED_API
#ifndef Py_CLASSOBJECT_H
#define Py_CLASSOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

typedef struct {
    PyObject_HEAD
    PyObject *im_func;   /* The callable object implementing the method */
    PyObject *im_self;   /* The instance it is bound to */
    PyObject *im_weakreflist; /* List of weak references */
} PyMethodObject;

extern PyTypeObject PyMethod_Type;

#define PyMethod_Check(op) ((op)->ob_type == &PyMethod_Type)

PyObject * PyMethod_New(PyObject *, PyObject *);

PyObject * PyMethod_Function(PyObject *);
PyObject * PyMethod_Self(PyObject *);

/* Macros for direct access to these values. Type checks are *not*
   done, so use with care. */
#define PyMethod_GET_FUNCTION(meth) \
        (((PyMethodObject *)meth) -> im_func)
#define PyMethod_GET_SELF(meth) \
	(((PyMethodObject *)meth) -> im_self)

int PyMethod_ClearFreeList(void);

typedef struct {
	PyObject_HEAD
	PyObject *func;
} PyInstanceMethodObject;

extern PyTypeObject PyInstanceMethod_Type;

#define PyInstanceMethod_Check(op) ((op)->ob_type == &PyInstanceMethod_Type)

PyObject * PyInstanceMethod_New(PyObject *);
PyObject * PyInstanceMethod_Function(PyObject *);

/* Macros for direct access to these values. Type checks are *not*
   done, so use with care. */
#define PyInstanceMethod_GET_FUNCTION(meth) \
        (((PyInstanceMethodObject *)meth) -> func)

COSMOPOLITAN_C_END_
#endif /* !Py_CLASSOBJECT_H */
#endif /* Py_LIMITED_API */
