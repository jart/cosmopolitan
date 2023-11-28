#ifndef Py_DESCROBJECT_H
#define Py_DESCROBJECT_H
#include "third_party/python/Include/methodobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyport.h"
COSMOPOLITAN_C_START_

typedef PyObject *(*getter)(PyObject *, void *);
typedef int (*setter)(PyObject *, PyObject *, void *);

typedef struct PyGetSetDef {
    char *name;
    getter get;
    setter set;
    char *doc;
    void *closure;
} PyGetSetDef;

#ifndef Py_LIMITED_API
typedef PyObject *(*wrapperfunc)(PyObject *self, PyObject *args,
                                 void *wrapped);

typedef PyObject *(*wrapperfunc_kwds)(PyObject *self, PyObject *args,
                                      void *wrapped, PyObject *kwds);

struct wrapperbase {
    char *name;
    int offset;
    void *function;
    wrapperfunc wrapper;
    char *doc;
    int flags;
    PyObject *name_strobj;
};

/* Flags for above struct */
#define PyWrapperFlag_KEYWORDS 1 /* wrapper function takes keyword args */

/* Various kinds of descriptor objects */

typedef struct {
    PyObject_HEAD
    PyTypeObject *d_type;
    PyObject *d_name;
    PyObject *d_qualname;
} PyDescrObject;

#define PyDescr_COMMON PyDescrObject d_common

#define PyDescr_TYPE(x) (((PyDescrObject *)(x))->d_type)
#define PyDescr_NAME(x) (((PyDescrObject *)(x))->d_name)

typedef struct {
    PyDescr_COMMON;
    PyMethodDef *d_method;
} PyMethodDescrObject;

typedef struct {
    PyDescr_COMMON;
    struct PyMemberDef *d_member;
} PyMemberDescrObject;

typedef struct {
    PyDescr_COMMON;
    PyGetSetDef *d_getset;
} PyGetSetDescrObject;

typedef struct {
    PyDescr_COMMON;
    struct wrapperbase *d_base;
    void *d_wrapped; /* This can be any function pointer */
} PyWrapperDescrObject;
#endif /* Py_LIMITED_API */

extern PyTypeObject PyClassMethodDescr_Type;
extern PyTypeObject PyGetSetDescr_Type;
extern PyTypeObject PyMemberDescr_Type;
extern PyTypeObject PyMethodDescr_Type;
extern PyTypeObject PyWrapperDescr_Type;
extern PyTypeObject PyDictProxy_Type;
#ifndef Py_LIMITED_API
extern PyTypeObject _PyMethodWrapper_Type;
#endif /* Py_LIMITED_API */

PyObject * PyDescr_NewMethod(PyTypeObject *, PyMethodDef *);
PyObject * PyDescr_NewClassMethod(PyTypeObject *, PyMethodDef *);
struct PyMemberDef; /* forward declaration for following prototype */
PyObject * PyDescr_NewMember(PyTypeObject *,
                                               struct PyMemberDef *);
PyObject * PyDescr_NewGetSet(PyTypeObject *,
                                               struct PyGetSetDef *);
#ifndef Py_LIMITED_API
PyObject * _PyMethodDescr_FastCallKeywords(
        PyObject *descrobj, PyObject *const *stack, Py_ssize_t nargs, PyObject *kwnames);
PyObject * PyDescr_NewWrapper(PyTypeObject *,
                                                struct wrapperbase *, void *);
#define PyDescr_IsData(d) (Py_TYPE(d)->tp_descr_set != NULL)
#endif

PyObject * PyDictProxy_New(PyObject *);
PyObject * PyWrapper_New(PyObject *, PyObject *);


extern PyTypeObject PyProperty_Type;

COSMOPOLITAN_C_END_
#endif /* !Py_DESCROBJECT_H */
