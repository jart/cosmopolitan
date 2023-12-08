/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/Python-ast.h"
#include "third_party/python/Include/code.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/symtable.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("_symtable");
PYTHON_PROVIDE("_symtable.CELL");
PYTHON_PROVIDE("_symtable.DEF_ANNOT");
PYTHON_PROVIDE("_symtable.DEF_BOUND");
PYTHON_PROVIDE("_symtable.DEF_FREE");
PYTHON_PROVIDE("_symtable.DEF_FREE_CLASS");
PYTHON_PROVIDE("_symtable.DEF_GLOBAL");
PYTHON_PROVIDE("_symtable.DEF_IMPORT");
PYTHON_PROVIDE("_symtable.DEF_LOCAL");
PYTHON_PROVIDE("_symtable.DEF_PARAM");
PYTHON_PROVIDE("_symtable.FREE");
PYTHON_PROVIDE("_symtable.GLOBAL_EXPLICIT");
PYTHON_PROVIDE("_symtable.GLOBAL_IMPLICIT");
PYTHON_PROVIDE("_symtable.LOCAL");
PYTHON_PROVIDE("_symtable.SCOPE_MASK");
PYTHON_PROVIDE("_symtable.SCOPE_OFF");
PYTHON_PROVIDE("_symtable.TYPE_CLASS");
PYTHON_PROVIDE("_symtable.TYPE_FUNCTION");
PYTHON_PROVIDE("_symtable.TYPE_MODULE");
PYTHON_PROVIDE("_symtable.USE");
PYTHON_PROVIDE("_symtable.symtable");

static PyObject *
symtable_symtable(PyObject *self, PyObject *args)
{
    struct symtable *st;
    PyObject *t;

    char *str;
    PyObject *filename;
    char *startstr;
    int start;

    if (!PyArg_ParseTuple(args, "sO&s:symtable",
                          &str, PyUnicode_FSDecoder, &filename, &startstr))
        return NULL;
    if (strcmp(startstr, "exec") == 0)
        start = Py_file_input;
    else if (strcmp(startstr, "eval") == 0)
        start = Py_eval_input;
    else if (strcmp(startstr, "single") == 0)
        start = Py_single_input;
    else {
        PyErr_SetString(PyExc_ValueError,
           "symtable() arg 3 must be 'exec' or 'eval' or 'single'");
        Py_DECREF(filename);
        return NULL;
    }
    st = Py_SymtableStringObject(str, filename, start);
    Py_DECREF(filename);
    if (st == NULL)
        return NULL;
    t = (PyObject *)st->st_top;
    Py_INCREF(t);
    PyMem_Free((void *)st->st_future);
    PySymtable_Free(st);
    return t;
}

static PyMethodDef symtable_methods[] = {
    {"symtable",        symtable_symtable,      METH_VARARGS,
     PyDoc_STR("Return symbol and scope dictionaries"
               " used internally by compiler.")},
    {NULL,              NULL}           /* sentinel */
};

static struct PyModuleDef symtablemodule = {
    PyModuleDef_HEAD_INIT,
    "_symtable",
    NULL,
    -1,
    symtable_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__symtable(void)
{
    PyObject *m;

    if (PyType_Ready(&PySTEntry_Type) < 0)
        return NULL;

    m = PyModule_Create(&symtablemodule);
    if (m == NULL)
        return NULL;
    PyModule_AddIntMacro(m, USE);
    PyModule_AddIntMacro(m, DEF_GLOBAL);
    PyModule_AddIntMacro(m, DEF_LOCAL);
    PyModule_AddIntMacro(m, DEF_PARAM);
    PyModule_AddIntMacro(m, DEF_FREE);
    PyModule_AddIntMacro(m, DEF_FREE_CLASS);
    PyModule_AddIntMacro(m, DEF_IMPORT);
    PyModule_AddIntMacro(m, DEF_BOUND);
    PyModule_AddIntMacro(m, DEF_ANNOT);

    PyModule_AddIntConstant(m, "TYPE_FUNCTION", FunctionBlock);
    PyModule_AddIntConstant(m, "TYPE_CLASS", ClassBlock);
    PyModule_AddIntConstant(m, "TYPE_MODULE", ModuleBlock);

    PyModule_AddIntMacro(m, LOCAL);
    PyModule_AddIntMacro(m, GLOBAL_EXPLICIT);
    PyModule_AddIntMacro(m, GLOBAL_IMPLICIT);
    PyModule_AddIntMacro(m, FREE);
    PyModule_AddIntMacro(m, CELL);

    PyModule_AddIntConstant(m, "SCOPE_OFF", SCOPE_OFFSET);
    PyModule_AddIntMacro(m, SCOPE_MASK);

    if (PyErr_Occurred()) {
        Py_DECREF(m);
        m = 0;
    }
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab__symtable = {
    "_symtable",
    PyInit__symtable,
};
