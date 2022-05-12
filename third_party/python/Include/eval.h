#ifndef Py_EVAL_H
#define Py_EVAL_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_
/* clang-format off */

PyObject * PyEval_EvalCode(PyObject *, PyObject *, PyObject *);

PyObject * PyEval_EvalCodeEx(PyObject *co,
					PyObject *globals,
					PyObject *locals,
					PyObject **args, int argc,
					PyObject **kwds, int kwdc,
					PyObject **defs, int defc,
					PyObject *kwdefs, PyObject *closure);

#ifndef Py_LIMITED_API
PyObject * _PyEval_CallTracing(PyObject *func, PyObject *args);
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_EVAL_H */
