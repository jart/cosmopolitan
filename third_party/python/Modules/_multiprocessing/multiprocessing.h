#ifndef MULTIPROCESSING_H
#define MULTIPROCESSING_H
#define PY_SSIZE_T_CLEAN
#include "libc/thread/semaphore.h"
#include "third_party/python/Include/Python.h"
#include "third_party/python/Include/pythread.h"
#include "third_party/python/Include/structmember.h"

/*
 * Platform includes and definitions
 */
#if defined(HAVE_SEM_OPEN) && !defined(POSIX_SEMAPHORES_NOT_ENABLED)
typedef sem_t *SEM_HANDLE;
#endif
#define HANDLE int
#define SOCKET int
#define BOOL int
#define UINT32 uint32_t
#define INT32 int32_t
#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE (-1)

/*
 * Issue 3110 - Solaris does not define SEM_VALUE_MAX
 */
#ifndef SEM_VALUE_MAX
    #if defined(HAVE_SYSCONF) && defined(_SC_SEM_VALUE_MAX)
        # define SEM_VALUE_MAX sysconf(_SC_SEM_VALUE_MAX)
    #elif defined(_SEM_VALUE_MAX)
        # define SEM_VALUE_MAX _SEM_VALUE_MAX
    #elif defined(_POSIX_SEM_VALUE_MAX)
        # define SEM_VALUE_MAX _POSIX_SEM_VALUE_MAX
    #else
        # define SEM_VALUE_MAX INT_MAX
    #endif
#endif


/*
 * Format codes
 */

#if SIZEOF_VOID_P == SIZEOF_LONG
#  define F_POINTER "k"
#  define T_POINTER T_ULONG
#elif SIZEOF_VOID_P == SIZEOF_LONG_LONG
#  define F_POINTER "K"
#  define T_POINTER T_ULONGLONG
#else
#  error "can't find format code for unsigned integer of same size as void*"
#endif

#ifdef MS_WINDOWS
#  define F_HANDLE F_POINTER
#  define T_HANDLE T_POINTER
#  define F_SEM_HANDLE F_HANDLE
#  define T_SEM_HANDLE T_HANDLE
#  define F_DWORD "k"
#  define T_DWORD T_ULONG
#else
#  define F_HANDLE "i"
#  define T_HANDLE T_INT
#  define F_SEM_HANDLE F_POINTER
#  define T_SEM_HANDLE T_POINTER
#endif

/*
 * Error codes which can be returned by functions called without GIL
 */

#define MP_SUCCESS (0)
#define MP_STANDARD_ERROR (-1)
#define MP_MEMORY_ERROR (-1001)
#define MP_SOCKET_ERROR (-1002)
#define MP_EXCEPTION_HAS_BEEN_SET (-1003)

PyObject *_PyMp_SetError(PyObject *Type, int num);

/*
 * Externs - not all will really exist on all platforms
 */

extern PyTypeObject _PyMp_SemLockType;
extern PyObject *_PyMp_sem_unlink(PyObject *ignore, PyObject *args);

#endif /* MULTIPROCESSING_H */
