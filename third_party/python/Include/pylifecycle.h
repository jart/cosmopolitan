#ifndef Py_PYLIFECYCLE_H
#define Py_PYLIFECYCLE_H
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pystate.h"
COSMOPOLITAN_C_START_

void Py_SetProgramName(wchar_t *);
wchar_t * Py_GetProgramName(void);

void Py_SetPythonHome(wchar_t *);
wchar_t * Py_GetPythonHome(void);

#ifndef Py_LIMITED_API
/* Only used by applications that embed the interpreter and need to
 * override the standard encoding determination mechanism
 */
int Py_SetStandardStreamEncoding(const char *encoding,
                                             const char *errors);
#endif

void Py_Initialize(void);
void Py_InitializeEx(int);
#ifndef Py_LIMITED_API
void _Py_InitializeEx_Private(int, int);
#endif
void Py_Finalize(void);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
int Py_FinalizeEx(void);
#endif
int Py_IsInitialized(void);
PyThreadState * Py_NewInterpreter(void);
void Py_EndInterpreter(PyThreadState *);


/* Py_PyAtExit is for the atexit module, Py_AtExit is for low-level
 * exit functions.
 */
#ifndef Py_LIMITED_API
void _Py_PyAtExit(void (*func)(void));
#endif
int Py_AtExit(void (*func)(void));

void Py_Exit(int);

/* Restore signals that the interpreter has called SIG_IGN on to SIG_DFL. */
#ifndef Py_LIMITED_API
void _Py_RestoreSignals(void);

int Py_FdIsInteractive(FILE *, const char *);
#endif

/* Bootstrap __main__ (defined in Modules/main.c) */
int Py_Main(int argc, wchar_t **argv);

/* In getpath.c */
wchar_t * Py_GetProgramFullPath(void);
wchar_t * Py_GetPrefix(void);
wchar_t * Py_GetExecPrefix(void);
wchar_t * Py_GetPath(void);
void      Py_SetPath(const wchar_t *);
void      Py_LimitedPath(void);
#ifdef MS_WINDOWS
int _Py_CheckPython3();
#endif

/* In their own files */
const char * Py_GetVersion(void);
const char * Py_GetPlatform(void);
const char * Py_GetCopyright(void);
const char * Py_GetCompiler(void);
const char * Py_GetBuildInfo(void);
#ifndef Py_LIMITED_API
const char * _Py_gitidentifier(void);
const char * _Py_gitversion(void);
#endif

/* Internal -- various one-time initializations */
#ifndef Py_LIMITED_API
extern int _Py_initialized;
extern char *_Py_StandardStreamEncoding;
extern char *_Py_StandardStreamErrors;
PyObject * _PyBuiltin_Init(void);
PyObject * _PySys_Init(void);
void _PyImport_Init(void);
void _PyExc_Init(PyObject * bltinmod);
void _PyImportHooks_Init(void);
int _PyFrame_Init(void);
int _PyFloat_Init(void);
int PyByteArray_Init(void);
void _PyRandom_Init(void);
void _Py_InitSite(void);
int _Py_InitStdio(void);
void _Py_ReadyTypes(void);
void _Py_InitImport(PyInterpreterState *, PyObject *);
int _Py_InitFsEncoding(PyInterpreterState *);
void _Py_InitSigs(void);
void _Py_InitMain(PyInterpreterState *);
int _PyUnicode_Init(void);
int _PyStructSequence_Init(void);
int _PyLong_Init(void);
int _PyFaulthandler_Init(void);
int _PyTraceMalloc_Init(void);
#endif

/* Various internal finalizers */
#ifndef Py_LIMITED_API
void _PyExc_Fini(void);
void _PyImport_Fini(void);
void PyMethod_Fini(void);
void PyFrame_Fini(void);
void PyCFunction_Fini(void);
void PyDict_Fini(void);
void PyTuple_Fini(void);
void PyList_Fini(void);
void PySet_Fini(void);
void PyBytes_Fini(void);
void PyByteArray_Fini(void);
void PyFloat_Fini(void);
void PyOS_FiniInterrupts(void);
void _PyGC_DumpShutdownStats(void);
void _PyGC_Fini(void);
void PySlice_Fini(void);
void _PyType_Fini(void);
void _PyRandom_Fini(void);
void PyAsyncGen_Fini(void);
int _Py_FlushStdFiles(void);
void _Py_CallExitFuncs(void);
void _Py_CallLlExitFuncs(void);
void _PyFaulthandler_Fini(void);
void _PyHash_Fini(void);
void PyLong_Fini(void);
void _PyUnicode_Fini(void);
int _PyTraceMalloc_Fini(void);
void _PyMem_DumpTraceback(int, const void *);
extern PyThreadState * _Py_Finalizing;
#endif

/* Signals */
typedef void (*PyOS_sighandler_t)(int);
PyOS_sighandler_t PyOS_getsig(int);
PyOS_sighandler_t PyOS_setsig(int, PyOS_sighandler_t);

#ifndef Py_LIMITED_API
/* Random */
int _PyOS_URandom(void *buffer, Py_ssize_t size);
int _PyOS_URandomNonblock(void *buffer, Py_ssize_t size);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_PYLIFECYCLE_H */
