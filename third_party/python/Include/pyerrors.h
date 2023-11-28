#ifndef Py_ERRORS_H
#define Py_ERRORS_H
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/unicodeobject.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
/* PyException_HEAD defines the initial segment of every exception class. */
#define PyException_HEAD PyObject_HEAD PyObject *dict;\
             PyObject *args; PyObject *traceback;\
             PyObject *context; PyObject *cause;\
             char suppress_context;

typedef struct {
    PyException_HEAD
} PyBaseExceptionObject;

typedef struct {
    PyException_HEAD
    PyObject *msg;
    PyObject *filename;
    PyObject *lineno;
    PyObject *offset;
    PyObject *text;
    PyObject *print_file_and_line;
} PySyntaxErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *msg;
    PyObject *name;
    PyObject *path;
} PyImportErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *encoding;
    PyObject *object;
    Py_ssize_t start;
    Py_ssize_t end;
    PyObject *reason;
} PyUnicodeErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *code;
} PySystemExitObject;

typedef struct {
    PyException_HEAD
    PyObject *myerrno;
    PyObject *strerror;
    PyObject *filename;
    PyObject *filename2;
#ifdef MS_WINDOWS
    PyObject *winerror;
#endif
    Py_ssize_t written;   /* only for BlockingIOError, -1 otherwise */
} PyOSErrorObject;

typedef struct {
    PyException_HEAD
    PyObject *value;
} PyStopIterationObject;

/* Compatibility typedefs */
typedef PyOSErrorObject PyEnvironmentErrorObject;
#ifdef MS_WINDOWS
typedef PyOSErrorObject PyWindowsErrorObject;
#endif
#endif /* !Py_LIMITED_API */

/* Error handling definitions */

void PyErr_SetNone(PyObject *);
void PyErr_SetObject(PyObject *, PyObject *);
#ifndef Py_LIMITED_API
void _PyErr_SetKeyError(PyObject *);
#endif
void PyErr_SetString(
    PyObject *exception,
    const char *string   /* decoded from utf-8 */
    );
PyObject * PyErr_Occurred(void);
void PyErr_Clear(void);
void PyErr_Fetch(PyObject **, PyObject **, PyObject **);
void PyErr_Restore(PyObject *, PyObject *, PyObject *);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
void PyErr_GetExcInfo(PyObject **, PyObject **, PyObject **);
void PyErr_SetExcInfo(PyObject *, PyObject *, PyObject *);
#endif

#if defined(__clang__) || \
    (defined(__GNUC__) && \
     ((__GNUC__ >= 3) || \
      (__GNUC__ == 2) && (__GNUC_MINOR__ >= 5)))
#define _Py_NO_RETURN __attribute__((__noreturn__))
#else
#define _Py_NO_RETURN
#endif

/* Defined in Python/pylifecycle.c */
void Py_FatalError(const char *message) relegated _Py_NO_RETURN;

#if defined(Py_DEBUG) || defined(Py_LIMITED_API)
#define _PyErr_OCCURRED() PyErr_Occurred()
#else
#define _PyErr_OCCURRED() (PyThreadState_GET()->curexc_type)
#endif

/* Error testing and normalization */
int PyErr_GivenExceptionMatches(PyObject *, PyObject *);
int PyErr_ExceptionMatches(PyObject *);
void PyErr_NormalizeException(PyObject**, PyObject**, PyObject**);

/* Traceback manipulation (PEP 3134) */
int PyException_SetTraceback(PyObject *, PyObject *);
PyObject * PyException_GetTraceback(PyObject *);

/* Cause manipulation (PEP 3134) */
PyObject * PyException_GetCause(PyObject *);
void PyException_SetCause(PyObject *, PyObject *);

/* Context manipulation (PEP 3134) */
PyObject * PyException_GetContext(PyObject *);
void PyException_SetContext(PyObject *, PyObject *);
#ifndef Py_LIMITED_API
void _PyErr_ChainExceptions(PyObject *, PyObject *, PyObject *);
#endif

/* */

#define PyExceptionClass_Check(x)                                       \
    (PyType_Check((x)) &&                                               \
     PyType_FastSubclass((PyTypeObject*)(x), Py_TPFLAGS_BASE_EXC_SUBCLASS))

#define PyExceptionInstance_Check(x)                    \
    PyType_FastSubclass((x)->ob_type, Py_TPFLAGS_BASE_EXC_SUBCLASS)

#define PyExceptionClass_Name(x) \
     ((char *)(((PyTypeObject*)(x))->tp_name))

#define PyExceptionInstance_Class(x) ((PyObject*)((x)->ob_type))


/* Predefined exceptions */

extern PyObject * PyExc_BaseException;
extern PyObject * PyExc_Exception;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
extern PyObject * PyExc_StopAsyncIteration;
#endif
extern PyObject * PyExc_StopIteration;
extern PyObject * PyExc_GeneratorExit;
extern PyObject * PyExc_ArithmeticError;
extern PyObject * PyExc_LookupError;

extern PyObject * PyExc_AssertionError;
extern PyObject * PyExc_AttributeError;
extern PyObject * PyExc_BufferError;
extern PyObject * PyExc_EOFError;
extern PyObject * PyExc_FloatingPointError;
extern PyObject * PyExc_OSError;
extern PyObject * PyExc_ImportError;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
extern PyObject * PyExc_ModuleNotFoundError;
#endif
extern PyObject * PyExc_IndexError;
extern PyObject * PyExc_KeyError;
extern PyObject * PyExc_KeyboardInterrupt;
extern PyObject * PyExc_MemoryError;
extern PyObject * PyExc_NameError;
extern PyObject * PyExc_OverflowError;
extern PyObject * PyExc_RuntimeError;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
extern PyObject * PyExc_RecursionError;
#endif
extern PyObject * PyExc_NotImplementedError;
extern PyObject * PyExc_SyntaxError;
extern PyObject * PyExc_IndentationError;
extern PyObject * PyExc_TabError;
extern PyObject * PyExc_ReferenceError;
extern PyObject * PyExc_SystemError;
extern PyObject * PyExc_SystemExit;
extern PyObject * PyExc_TypeError;
extern PyObject * PyExc_UnboundLocalError;
extern PyObject * PyExc_UnicodeError;
extern PyObject * PyExc_UnicodeEncodeError;
extern PyObject * PyExc_UnicodeDecodeError;
extern PyObject * PyExc_UnicodeTranslateError;
extern PyObject * PyExc_ValueError;
extern PyObject * PyExc_ZeroDivisionError;

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
extern PyObject * PyExc_BlockingIOError;
extern PyObject * PyExc_BrokenPipeError;
extern PyObject * PyExc_ChildProcessError;
extern PyObject * PyExc_ConnectionError;
extern PyObject * PyExc_ConnectionAbortedError;
extern PyObject * PyExc_ConnectionRefusedError;
extern PyObject * PyExc_ConnectionResetError;
extern PyObject * PyExc_FileExistsError;
extern PyObject * PyExc_FileNotFoundError;
extern PyObject * PyExc_InterruptedError;
extern PyObject * PyExc_IsADirectoryError;
extern PyObject * PyExc_NotADirectoryError;
extern PyObject * PyExc_PermissionError;
extern PyObject * PyExc_ProcessLookupError;
extern PyObject * PyExc_TimeoutError;
#endif


/* Compatibility aliases */
extern PyObject * PyExc_EnvironmentError;
extern PyObject * PyExc_IOError;
#ifdef MS_WINDOWS
extern PyObject * PyExc_WindowsError;
#endif

/* Predefined warning categories */
extern PyObject * PyExc_Warning;
extern PyObject * PyExc_UserWarning;
extern PyObject * PyExc_DeprecationWarning;
extern PyObject * PyExc_PendingDeprecationWarning;
extern PyObject * PyExc_SyntaxWarning;
extern PyObject * PyExc_RuntimeWarning;
extern PyObject * PyExc_FutureWarning;
extern PyObject * PyExc_ImportWarning;
extern PyObject * PyExc_UnicodeWarning;
extern PyObject * PyExc_BytesWarning;
extern PyObject * PyExc_ResourceWarning;


/* Convenience functions */

int PyErr_BadArgument(void);
PyObject * PyErr_NoMemory(void);
PyObject * PyErr_SetFromErrno(PyObject *);
PyObject * PyErr_SetFromErrnoWithFilenameObject(
    PyObject *, PyObject *);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03040000
PyObject * PyErr_SetFromErrnoWithFilenameObjects(
    PyObject *, PyObject *, PyObject *);
#endif
PyObject * PyErr_SetFromErrnoWithFilename(
    PyObject *exc,
    const char *filename   /* decoded from the filesystem encoding */
    );
#if !defined(Py_LIMITED_API)
PyObject * PyErr_SetFromErrnoWithUnicodeFilename(
    PyObject *, const Py_UNICODE *);
#endif /* MS_WINDOWS */

PyObject * PyErr_Format(
    PyObject *exception,
    const char *format,   /* ASCII-encoded string  */
    ...
    );
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
PyObject * PyErr_FormatV(
    PyObject *exception,
    const char *format,
    va_list vargs);
#endif

#ifndef Py_LIMITED_API
/* Like PyErr_Format(), but saves current exception as __context__ and
   __cause__.
 */
PyObject * _PyErr_FormatFromCause(
    PyObject *exception,
    const char *format,   /* ASCII-encoded string  */
    ...
    );
#endif

PyObject * PyErr_SetFromWindowsErrWithFilename(
    int ierr,
    const char *filename        /* decoded from the filesystem encoding */
    );
#ifndef Py_LIMITED_API
/* XXX redeclare to use WSTRING */
PyObject * PyErr_SetFromWindowsErrWithUnicodeFilename(
    int, const Py_UNICODE *);
#endif
PyObject * PyErr_SetFromWindowsErr(int);
PyObject * PyErr_SetExcFromWindowsErrWithFilenameObject(
    PyObject *,int, PyObject *);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03040000
PyObject * PyErr_SetExcFromWindowsErrWithFilenameObjects(
    PyObject *,int, PyObject *, PyObject *);
#endif
PyObject * PyErr_SetExcFromWindowsErrWithFilename(
    PyObject *exc,
    int ierr,
    const char *filename        /* decoded from the filesystem encoding */
    );
#ifndef Py_LIMITED_API
PyObject * PyErr_SetExcFromWindowsErrWithUnicodeFilename(
    PyObject *,int, const Py_UNICODE *);
#endif
PyObject * PyErr_SetExcFromWindowsErr(PyObject *, int);

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
PyObject * PyErr_SetImportErrorSubclass(PyObject *, PyObject *,
    PyObject *, PyObject *);
#endif
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyObject * PyErr_SetImportError(PyObject *, PyObject *,
    PyObject *);
#endif

/* Export the old function so that the existing API remains available: */
void PyErr_BadInternalCall(void);
void _PyErr_BadInternalCall(const char *filename, int lineno);
/* Mask the old API with a call to the new API for code compiled under
   Python 2.0: */
#if IsModeDbg()
#define PyErr_BadInternalCall() _PyErr_BadInternalCall(__FILE__, __LINE__)
#else
#define PyErr_BadInternalCall()
#endif
/* Function to create a new exception */
PyObject * PyErr_NewException(
    const char *name, PyObject *base, PyObject *dict);
PyObject * PyErr_NewExceptionWithDoc(
    const char *name, const char *doc, PyObject *base, PyObject *dict);
void PyErr_WriteUnraisable(PyObject *);

/* In exceptions.c */
#ifndef Py_LIMITED_API
/* Helper that attempts to replace the current exception with one of the
 * same type but with a prefix added to the exception text. The resulting
 * exception description looks like:
 *
 *     prefix (exc_type: original_exc_str)
 *
 * Only some exceptions can be safely replaced. If the function determines
 * it isn't safe to perform the replacement, it will leave the original
 * unmodified exception in place.
 *
 * Returns a borrowed reference to the new exception (if any), NULL if the
 * existing exception was left in place.
 */
PyObject * _PyErr_TrySetFromCause(
    const char *prefix_format,   /* ASCII-encoded string  */
    ...
    );
#endif

/* In sigcheck.c or signalmodule.c */
int PyErr_CheckSignals(void);
void PyErr_SetInterrupt(void);

/* In signalmodule.c */
#ifndef Py_LIMITED_API
int PySignal_SetWakeupFd(int fd);
#endif

/* Support for adding program text to SyntaxErrors */
void PyErr_SyntaxLocation(
    const char *filename,       /* decoded from the filesystem encoding */
    int lineno);
void PyErr_SyntaxLocationEx(
    const char *filename,       /* decoded from the filesystem encoding */
    int lineno,
    int col_offset);
#ifndef Py_LIMITED_API
void PyErr_SyntaxLocationObject(
    PyObject *filename,
    int lineno,
    int col_offset);
#endif
PyObject * PyErr_ProgramText(
    const char *filename,       /* decoded from the filesystem encoding */
    int lineno);
#ifndef Py_LIMITED_API
PyObject * PyErr_ProgramTextObject(
    PyObject *filename,
    int lineno);
#endif

/* The following functions are used to create and modify unicode
   exceptions from C */

/* create a UnicodeDecodeError object */
PyObject * PyUnicodeDecodeError_Create(
    const char *encoding,       /* UTF-8 encoded string */
    const char *object,
    Py_ssize_t length,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    );

/* create a UnicodeEncodeError object */
#ifndef Py_LIMITED_API
PyObject * PyUnicodeEncodeError_Create(
    const char *encoding,       /* UTF-8 encoded string */
    const Py_UNICODE *object,
    Py_ssize_t length,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    );
#endif

/* create a UnicodeTranslateError object */
#ifndef Py_LIMITED_API
PyObject * PyUnicodeTranslateError_Create(
    const Py_UNICODE *object,
    Py_ssize_t length,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    );
PyObject * _PyUnicodeTranslateError_Create(
    PyObject *object,
    Py_ssize_t start,
    Py_ssize_t end,
    const char *reason          /* UTF-8 encoded string */
    );
#endif

/* get the encoding attribute */
PyObject * PyUnicodeEncodeError_GetEncoding(PyObject *);
PyObject * PyUnicodeDecodeError_GetEncoding(PyObject *);

/* get the object attribute */
PyObject * PyUnicodeEncodeError_GetObject(PyObject *);
PyObject * PyUnicodeDecodeError_GetObject(PyObject *);
PyObject * PyUnicodeTranslateError_GetObject(PyObject *);

/* get the value of the start attribute (the int * may not be NULL)
   return 0 on success, -1 on failure */
int PyUnicodeEncodeError_GetStart(PyObject *, Py_ssize_t *);
int PyUnicodeDecodeError_GetStart(PyObject *, Py_ssize_t *);
int PyUnicodeTranslateError_GetStart(PyObject *, Py_ssize_t *);

/* assign a new value to the start attribute
   return 0 on success, -1 on failure */
int PyUnicodeEncodeError_SetStart(PyObject *, Py_ssize_t);
int PyUnicodeDecodeError_SetStart(PyObject *, Py_ssize_t);
int PyUnicodeTranslateError_SetStart(PyObject *, Py_ssize_t);

/* get the value of the end attribute (the int *may not be NULL)
 return 0 on success, -1 on failure */
int PyUnicodeEncodeError_GetEnd(PyObject *, Py_ssize_t *);
int PyUnicodeDecodeError_GetEnd(PyObject *, Py_ssize_t *);
int PyUnicodeTranslateError_GetEnd(PyObject *, Py_ssize_t *);

/* assign a new value to the end attribute
   return 0 on success, -1 on failure */
int PyUnicodeEncodeError_SetEnd(PyObject *, Py_ssize_t);
int PyUnicodeDecodeError_SetEnd(PyObject *, Py_ssize_t);
int PyUnicodeTranslateError_SetEnd(PyObject *, Py_ssize_t);

/* get the value of the reason attribute */
PyObject * PyUnicodeEncodeError_GetReason(PyObject *);
PyObject * PyUnicodeDecodeError_GetReason(PyObject *);
PyObject * PyUnicodeTranslateError_GetReason(PyObject *);

/* assign a new value to the reason attribute
   return 0 on success, -1 on failure */
int PyUnicodeEncodeError_SetReason(
    PyObject *exc,
    const char *reason          /* UTF-8 encoded string */
    );
int PyUnicodeDecodeError_SetReason(
    PyObject *exc,
    const char *reason          /* UTF-8 encoded string */
    );
int PyUnicodeTranslateError_SetReason(
    PyObject *exc,
    const char *reason          /* UTF-8 encoded string */
    );

int PyOS_snprintf(char *str, size_t size, const char  *format, ...)
                        Py_GCC_ATTRIBUTE((format(printf, 3, 4)));
int PyOS_vsnprintf(char *str, size_t size, const char  *format, va_list va)
                        Py_GCC_ATTRIBUTE((format(printf, 3, 0)));

COSMOPOLITAN_C_END_
#endif /* !Py_ERRORS_H */
