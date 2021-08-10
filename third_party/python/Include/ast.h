#ifndef Py_AST_H
#define Py_AST_H
COSMOPOLITAN_C_START_
/* clang-format off */

extern int foo;
PyAPI_FUNC(int) PyAST_Validate(mod_ty);
PyAPI_FUNC(mod_ty) PyAST_FromNode(
    const node *n,
    PyCompilerFlags *flags,
    const char *filename,       /* decoded from the filesystem encoding */
    PyArena *arena);
PyAPI_FUNC(mod_ty) PyAST_FromNodeObject(
    const node *n,
    PyCompilerFlags *flags,
    PyObject *filename,
    PyArena *arena);

COSMOPOLITAN_C_END_
#endif /* !Py_AST_H */
