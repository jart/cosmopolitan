#ifndef Py_AST_H
#define Py_AST_H
#include "third_party/python/Include/Python-ast.h"
#include "third_party/python/Include/node.h"
#include "third_party/python/Include/pythonrun.h"
COSMOPOLITAN_C_START_

extern int foo;
int PyAST_Validate(mod_ty);
mod_ty PyAST_FromNode(
    const node *n,
    PyCompilerFlags *flags,
    const char *filename,       /* decoded from the filesystem encoding */
    PyArena *arena);
mod_ty PyAST_FromNodeObject(
    const node *n,
    PyCompilerFlags *flags,
    PyObject *filename,
    PyArena *arena);

COSMOPOLITAN_C_END_
#endif /* !Py_AST_H */
