#ifndef Py_LIMITED_API
#ifndef Py_FUNCOBJECT_H
#define Py_FUNCOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

/* Function objects and code objects should not be confused with each other:
 *
 * Function objects are created by the execution of the 'def' statement.
 * They reference a code object in their __code__ attribute, which is a
 * purely syntactic object, i.e. nothing more than a compiled version of some
 * source code lines.  There is one code object per source code "fragment",
 * but each code object can be referenced by zero or many function objects
 * depending only on how many times the 'def' statement in the source was
 * executed so far.
 */

typedef struct {
    PyObject_HEAD
    PyObject *func_code;	/* A code object, the __code__ attribute */
    PyObject *func_globals;	/* A dictionary (other mappings won't do) */
    PyObject *func_defaults;	/* NULL or a tuple */
    PyObject *func_kwdefaults;	/* NULL or a dict */
    PyObject *func_closure;	/* NULL or a tuple of cell objects */
    PyObject *func_doc;		/* The __doc__ attribute, can be anything */
    PyObject *func_name;	/* The __name__ attribute, a string object */
    PyObject *func_dict;	/* The __dict__ attribute, a dict or NULL */
    PyObject *func_weakreflist;	/* List of weak references */
    PyObject *func_module;	/* The __module__ attribute, can be anything */
    PyObject *func_annotations;	/* Annotations, a dict or NULL */
    PyObject *func_qualname;    /* The qualified name */

    /* Invariant:
     *     func_closure contains the bindings for func_code->co_freevars, so
     *     PyTuple_Size(func_closure) == PyCode_GetNumFree(func_code)
     *     (func_closure may be NULL if PyCode_GetNumFree(func_code) == 0).
     */
} PyFunctionObject;

extern PyTypeObject PyFunction_Type;

#define PyFunction_Check(op) (Py_TYPE(op) == &PyFunction_Type)

PyObject * PyFunction_New(PyObject *, PyObject *);
PyObject * PyFunction_NewWithQualName(PyObject *, PyObject *, PyObject *);
PyObject * PyFunction_GetCode(PyObject *);
PyObject * PyFunction_GetGlobals(PyObject *);
PyObject * PyFunction_GetModule(PyObject *);
PyObject * PyFunction_GetDefaults(PyObject *);
int PyFunction_SetDefaults(PyObject *, PyObject *);
PyObject * PyFunction_GetKwDefaults(PyObject *);
int PyFunction_SetKwDefaults(PyObject *, PyObject *);
PyObject * PyFunction_GetClosure(PyObject *);
int PyFunction_SetClosure(PyObject *, PyObject *);
PyObject * PyFunction_GetAnnotations(PyObject *);
int PyFunction_SetAnnotations(PyObject *, PyObject *);

#ifndef Py_LIMITED_API
PyObject * _PyFunction_FastCallDict(
    PyObject *func,
    PyObject **args,
    Py_ssize_t nargs,
    PyObject *kwargs);

PyObject * _PyFunction_FastCallKeywords(
    PyObject *func,
    PyObject **stack,
    Py_ssize_t nargs,
    PyObject *kwnames);
#endif

/* Macros for direct access to these values. Type checks are *not*
   done, so use with care. */
#define PyFunction_GET_CODE(func) \
        (((PyFunctionObject *)func) -> func_code)
#define PyFunction_GET_GLOBALS(func) \
	(((PyFunctionObject *)func) -> func_globals)
#define PyFunction_GET_MODULE(func) \
	(((PyFunctionObject *)func) -> func_module)
#define PyFunction_GET_DEFAULTS(func) \
	(((PyFunctionObject *)func) -> func_defaults)
#define PyFunction_GET_KW_DEFAULTS(func) \
	(((PyFunctionObject *)func) -> func_kwdefaults)
#define PyFunction_GET_CLOSURE(func) \
	(((PyFunctionObject *)func) -> func_closure)
#define PyFunction_GET_ANNOTATIONS(func) \
	(((PyFunctionObject *)func) -> func_annotations)

/* The classmethod and staticmethod types lives here, too */
extern PyTypeObject PyClassMethod_Type;
extern PyTypeObject PyStaticMethod_Type;

PyObject * PyClassMethod_New(PyObject *);
PyObject * PyStaticMethod_New(PyObject *);

COSMOPOLITAN_C_END_
#endif /* !Py_FUNCOBJECT_H */
#endif /* Py_LIMITED_API */
