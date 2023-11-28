#ifndef Py_PYTHONRUN_H
#define Py_PYTHONRUN_H
#include "third_party/python/Include/code.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyarena.h"
#include "third_party/python/Include/pystate.h"
COSMOPOLITAN_C_START_

#define PyCF_MASK (CO_FUTURE_DIVISION | CO_FUTURE_ABSOLUTE_IMPORT | \
                   CO_FUTURE_WITH_STATEMENT | CO_FUTURE_PRINT_FUNCTION | \
                   CO_FUTURE_UNICODE_LITERALS | CO_FUTURE_BARRY_AS_BDFL | \
                   CO_FUTURE_GENERATOR_STOP)
#define PyCF_MASK_OBSOLETE (CO_NESTED)
#define PyCF_SOURCE_IS_UTF8  0x0100
#define PyCF_DONT_IMPLY_DEDENT 0x0200
#define PyCF_ONLY_AST 0x0400
#define PyCF_IGNORE_COOKIE 0x0800

#ifndef Py_LIMITED_API
typedef struct {
    int cf_flags;  /* bitmask of CO_xxx flags relevant to future */
} PyCompilerFlags;
#endif

#ifndef Py_LIMITED_API
int PyRun_SimpleStringFlags(const char *, PyCompilerFlags *);
int PyRun_AnyFileFlags(FILE *, const char *, PyCompilerFlags *);
int PyRun_AnyFileExFlags(
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    int closeit,
    PyCompilerFlags *flags);
int PyRun_SimpleFileExFlags(
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    int closeit,
    PyCompilerFlags *flags);
int PyRun_InteractiveOneFlags(
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    PyCompilerFlags *flags);
int PyRun_InteractiveOneObject(
    FILE *fp,
    PyObject *filename,
    PyCompilerFlags *flags);
int PyRun_InteractiveLoopFlags(
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    PyCompilerFlags *flags);

struct _mod * PyParser_ASTFromString(
    const char *s,
    const char *filename,       /* decoded from the filesystem encoding */
    int start,
    PyCompilerFlags *flags,
    PyArena *arena);
struct _mod * PyParser_ASTFromStringObject(
    const char *s,
    PyObject *filename,
    int start,
    PyCompilerFlags *flags,
    PyArena *arena);
struct _mod * PyParser_ASTFromFile(
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    const char* enc,
    int start,
    const char *ps1,
    const char *ps2,
    PyCompilerFlags *flags,
    int *errcode,
    PyArena *arena);
struct _mod * PyParser_ASTFromFileObject(
    FILE *fp,
    PyObject *filename,
    const char* enc,
    int start,
    const char *ps1,
    const char *ps2,
    PyCompilerFlags *flags,
    int *errcode,
    PyArena *arena);
#endif

#ifndef PyParser_SimpleParseString
#define PyParser_SimpleParseString(S, B) \
    PyParser_SimpleParseStringFlags(S, B, 0)
#define PyParser_SimpleParseFile(FP, S, B) \
    PyParser_SimpleParseFileFlags(FP, S, B, 0)
#endif
struct _node * PyParser_SimpleParseStringFlags(const char *, int,
                                                           int);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
struct _node * PyParser_SimpleParseStringFlagsFilename(const char *,
                                                                   const char *,
                                                                   int, int);
#endif
struct _node * PyParser_SimpleParseFileFlags(FILE *, const char *,
                                                         int, int);

#ifndef Py_LIMITED_API
PyObject * PyRun_StringFlags(const char *, int, PyObject *,
                                         PyObject *, PyCompilerFlags *);

PyObject * PyRun_FileExFlags(
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    int start,
    PyObject *globals,
    PyObject *locals,
    int closeit,
    PyCompilerFlags *flags);
#endif

#ifdef Py_LIMITED_API
PyObject * Py_CompileString(const char *, const char *, int);
#else
#define Py_CompileString(str, p, s) Py_CompileStringExFlags(str, p, s, NULL, -1)
#define Py_CompileStringFlags(str, p, s, f) Py_CompileStringExFlags(str, p, s, f, -1)
PyObject * Py_CompileStringExFlags(
    const char *str,
    const char *filename,       /* decoded from the filesystem encoding */
    int start,
    PyCompilerFlags *flags,
    int optimize);
PyObject * Py_CompileStringObject(
    const char *str,
    PyObject *filename, int start,
    PyCompilerFlags *flags,
    int optimize);
#endif
struct symtable * Py_SymtableString(
    const char *str,
    const char *filename,       /* decoded from the filesystem encoding */
    int start);
#ifndef Py_LIMITED_API
struct symtable * Py_SymtableStringObject(
    const char *str,
    PyObject *filename,
    int start);
#endif

void PyErr_Print(void);
void PyErr_PrintEx(int);
void PyErr_Display(PyObject *, PyObject *, PyObject *);

#ifndef Py_LIMITED_API
/* Use macros for a bunch of old variants */
#define PyRun_String(str, s, g, l) PyRun_StringFlags(str, s, g, l, NULL)
#define PyRun_AnyFile(fp, name) PyRun_AnyFileExFlags(fp, name, 0, NULL)
#define PyRun_AnyFileEx(fp, name, closeit) \
    PyRun_AnyFileExFlags(fp, name, closeit, NULL)
#define PyRun_AnyFileFlags(fp, name, flags) \
    PyRun_AnyFileExFlags(fp, name, 0, flags)
#define PyRun_SimpleString(s) PyRun_SimpleStringFlags(s, NULL)
#define PyRun_SimpleFile(f, p) PyRun_SimpleFileExFlags(f, p, 0, NULL)
#define PyRun_SimpleFileEx(f, p, c) PyRun_SimpleFileExFlags(f, p, c, NULL)
#define PyRun_InteractiveOne(f, p) PyRun_InteractiveOneFlags(f, p, NULL)
#define PyRun_InteractiveLoop(f, p) PyRun_InteractiveLoopFlags(f, p, NULL)
#define PyRun_File(fp, p, s, g, l) \
    PyRun_FileExFlags(fp, p, s, g, l, 0, NULL)
#define PyRun_FileEx(fp, p, s, g, l, c) \
    PyRun_FileExFlags(fp, p, s, g, l, c, NULL)
#define PyRun_FileFlags(fp, p, s, g, l, flags) \
    PyRun_FileExFlags(fp, p, s, g, l, 0, flags)
#endif

/* Stuff with no proper home (yet) */
#ifndef Py_LIMITED_API
char * PyOS_Readline(FILE *, FILE *, const char *);
#endif
extern int (*PyOS_InputHook)(void);
extern char *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, const char *);
#ifndef Py_LIMITED_API
extern PyThreadState* _PyOS_ReadlineTState;
#endif

/* Stack size, in "pointers" (so we get extra safety margins
   on 64-bit platforms).  On a 32-bit platform, this translates
   to an 8k margin. */
#define PYOS_STACK_MARGIN 2048

#if defined(WIN32) && !defined(MS_WIN64) && defined(_MSC_VER) && _MSC_VER >= 1300
/* Enable stack checking under Microsoft C */
#define USE_STACKCHECK
#endif

#ifdef USE_STACKCHECK
/* Check that we aren't overflowing our stack */
int PyOS_CheckStack(void);
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_PYTHONRUN_H */
