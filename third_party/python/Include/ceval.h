#ifndef Py_CEVAL_H
#define Py_CEVAL_H
#include "libc/dce.h"
#include "libc/intrin/likely.h"
#include "libc/runtime/stack.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pystate.h"
#include "third_party/python/Include/pythonrun.h"
COSMOPOLITAN_C_START_

/* Interface to random parts in ceval.c */

PyObject *PyEval_CallObjectWithKeywords(PyObject *, PyObject *, PyObject *);

/* Inline this */
#define PyEval_CallObject(func,arg) \
    PyEval_CallObjectWithKeywords(func, arg, (PyObject *)NULL)

PyObject *PyEval_CallFunction(PyObject *, const char *, ...);
PyObject *PyEval_CallMethod(PyObject *, const char *, const char *, ...);

#ifndef Py_LIMITED_API
void PyEval_SetProfile(Py_tracefunc, PyObject *);
void PyEval_SetTrace(Py_tracefunc, PyObject *);
void _PyEval_SetCoroutineWrapper(PyObject *);
PyObject *_PyEval_GetCoroutineWrapper(void);
void _PyEval_SetAsyncGenFirstiter(PyObject *);
PyObject *_PyEval_GetAsyncGenFirstiter(void);
void _PyEval_SetAsyncGenFinalizer(PyObject *);
PyObject *_PyEval_GetAsyncGenFinalizer(void);
#endif

struct _frame; /* Avoid including frameobject.h */

PyObject *PyEval_GetBuiltins(void);
PyObject *PyEval_GetGlobals(void);
PyObject *PyEval_GetLocals(void);
struct _frame *PyEval_GetFrame(void);

#ifndef Py_LIMITED_API
/* Helper to look up a builtin object */
PyObject *_PyEval_GetBuiltinId(_Py_Identifier *);
/* Look at the current frame's (if any) code's co_flags, and turn on
   the corresponding compiler flags in cf->cf_flags.  Return 1 if any
   flag was set, else return 0. */
int PyEval_MergeCompilerFlags(PyCompilerFlags *);
#endif

int Py_AddPendingCall(int (*)(void *), void *);
void _PyEval_SignalReceived(void);
int Py_MakePendingCalls(void);

/* Protection against deeply nested recursive calls

   In Python 3.0, this protection has two levels:
   * normal anti-recursion protection is triggered when the recursion level
     exceeds the current recursion limit. It raises a RecursionError, and sets
     the "overflowed" flag in the thread state structure. This flag
     temporarily *disables* the normal protection; this allows cleanup code
     to potentially outgrow the recursion limit while processing the
     RecursionError.
   * "last chance" anti-recursion protection is triggered when the recursion
     level exceeds "current recursion limit + 50". By construction, this
     protection can only be triggered when the "overflowed" flag is set. It
     means the cleanup code has itself gone into an infinite loop, or the
     RecursionError has been mistakingly ignored. When this protection is
     triggered, the interpreter aborts with a Fatal Error.

   In addition, the "overflowed" flag is automatically reset when the
   recursion level drops below "current recursion limit - 50". This heuristic
   is meant to ensure that the normal anti-recursion protection doesn't get
   disabled too long.

   Please note: this scheme has its own limitations. See:
   http://mail.python.org/pipermail/python-dev/2008-August/082106.html
   for some observations.
*/
void Py_SetRecursionLimit(int);
int Py_GetRecursionLimit(void);

#ifdef USE_STACKCHECK
/* With USE_STACKCHECK, we artificially decrement the recursion limit in order
   to trigger regular stack checks in _Py_CheckRecursiveCall(), except if
   the "overflowed" flag is set, in which case we need the true value
   of _Py_CheckRecursionLimit for _Py_MakeEndRecCheck() to function properly.
*/
#  define _Py_MakeRecCheck(x)  \
    (++(x) > (_Py_CheckRecursionLimit += PyThreadState_GET()->overflowed - 1))
#else
#  define _Py_MakeRecCheck(x)  (++(x) > _Py_CheckRecursionLimit)
#endif

/* Compute the "lower-water mark" for a recursion limit. When
 * Py_LeaveRecursiveCall() is called with a recursion depth below this mark,
 * the overflowed flag is reset to 0. ([jart] what) */
#define _Py_RecursionLimitLowerWaterMark(limit) \
    (((limit) > 200) \
        ? ((limit) - 50) \
        : (3 * ((limit) >> 2)))
#define _Py_MakeEndRecCheck(x) \
    (--(x) < _Py_RecursionLimitLowerWaterMark(_Py_CheckRecursionLimit))

int Py_EnterRecursiveCall(const char *);
void Py_LeaveRecursiveCall(void);

#ifndef Py_LIMITED_API
extern int _Py_CheckRecursionLimit;
int _Py_CheckRecursiveCall(const char *);
#define Py_LeaveRecursiveCall() PyThreadState_GET()->recursion_depth--
#define Py_EnterRecursiveCall(where)                                    \
  ({                                                                    \
    int rc = 0;                                                         \
    intptr_t rsp, bot;                                                  \
    if (IsModeDbg()) {                                                  \
      PyThreadState_GET()->recursion_depth++;                           \
      rc = _Py_CheckRecursiveCall(where);                               \
    } else {                                                            \
      rsp = (intptr_t)__builtin_frame_address(0);                       \
      bot = GetStackAddr() + 32768;                                     \
      if (UNLIKELY(rsp < bot)) {                                        \
        PyErr_Format(PyExc_MemoryError, "Stack overflow%s", where);     \
        rc = -1;                                                        \
      }                                                                 \
    }                                                                   \
    rc;                                                                 \
  })
#endif

#define Py_ALLOW_RECURSION                          \
  do {                                              \
    unsigned char _old;                             \
    _old = PyThreadState_GET()->recursion_critical; \
    PyThreadState_GET()->recursion_critical = 1;

#define Py_END_ALLOW_RECURSION                      \
    PyThreadState_GET()->recursion_critical = _old; \
  } while(0);

const char * PyEval_GetFuncName(PyObject *);
const char * PyEval_GetFuncDesc(PyObject *);

PyObject * PyEval_GetCallStats(PyObject *);
PyObject * PyEval_EvalFrame(struct _frame *);
PyObject * PyEval_EvalFrameEx(struct _frame *, int);
#define PyEval_EvalFrameEx(fr,st) PyThreadState_GET()->interp->eval_frame(fr,st)
#ifndef Py_LIMITED_API
PyObject * _PyEval_EvalFrameDefault(struct _frame *, int);
#endif

/* Interface for threads.

   A module that plans to do a blocking system call (or something else
   that lasts a long time and doesn't touch Python data) can allow other
   threads to run as follows:

    ...preparations here...
    Py_BEGIN_ALLOW_THREADS
    ...blocking system call here...
    Py_END_ALLOW_THREADS
    ...interpret result here...

   The Py_BEGIN_ALLOW_THREADS/Py_END_ALLOW_THREADS pair expands to a
   {}-surrounded block.
   To leave the block in the middle (e.g., with return), you must insert
   a line containing Py_BLOCK_THREADS before the return, e.g.

    if (...premature_exit...) {
        Py_BLOCK_THREADS
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

   An alternative is:

    Py_BLOCK_THREADS
    if (...premature_exit...) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }
    Py_UNBLOCK_THREADS

   For convenience, that the value of 'errno' is restored across
   Py_END_ALLOW_THREADS and Py_BLOCK_THREADS.

   WARNING: NEVER NEST CALLS TO Py_BEGIN_ALLOW_THREADS AND
   Py_END_ALLOW_THREADS!!!

   The function PyEval_InitThreads() should be called only from
   init_thread() in "_threadmodule.c".

   Note that not yet all candidates have been converted to use this
   mechanism!
*/

PyThreadState * PyEval_SaveThread(void);
void PyEval_RestoreThread(PyThreadState *);

#ifdef WITH_THREAD

int  PyEval_ThreadsInitialized(void);
void PyEval_InitThreads(void);
#ifndef Py_LIMITED_API
void _PyEval_FiniThreads(void);
#endif /* !Py_LIMITED_API */
void PyEval_AcquireLock(void);
void PyEval_ReleaseLock(void);
void PyEval_AcquireThread(PyThreadState *tstate);
void PyEval_ReleaseThread(PyThreadState *tstate);
void PyEval_ReInitThreads(void);

#ifndef Py_LIMITED_API
void _PyEval_SetSwitchInterval(unsigned long microseconds);
unsigned long _PyEval_GetSwitchInterval(void);
#endif

#ifndef Py_LIMITED_API
Py_ssize_t _PyEval_RequestCodeExtraIndex(freefunc);
#endif

#define Py_BEGIN_ALLOW_THREADS { \
                        PyThreadState *_save; \
                        _save = PyEval_SaveThread();
#define Py_BLOCK_THREADS        PyEval_RestoreThread(_save);
#define Py_UNBLOCK_THREADS      _save = PyEval_SaveThread();
#define Py_END_ALLOW_THREADS    PyEval_RestoreThread(_save); \
                 }

#else /* !WITH_THREAD */

#define Py_BEGIN_ALLOW_THREADS {
#define Py_BLOCK_THREADS
#define Py_UNBLOCK_THREADS
#define Py_END_ALLOW_THREADS }

#endif /* !WITH_THREAD */

#ifndef Py_LIMITED_API
int _PyEval_SliceIndex(PyObject *, Py_ssize_t *);
int _PyEval_SliceIndexNotNone(PyObject *, Py_ssize_t *);
void _PyEval_SignalAsyncExc(void);
#endif

/* Masks and values used by FORMAT_VALUE opcode. */
#define FVC_MASK      0x3
#define FVC_NONE      0x0
#define FVC_STR       0x1
#define FVC_REPR      0x2
#define FVC_ASCII     0x3
#define FVS_MASK      0x4
#define FVS_HAVE_SPEC 0x4

COSMOPOLITAN_C_END_
#endif /* !Py_CEVAL_H */
