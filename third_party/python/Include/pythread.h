#ifndef Py_PYTHREAD_H
#define Py_PYTHREAD_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

typedef void *PyThread_type_lock;
typedef void *PyThread_type_sema;

/* Return status codes for Python lock acquisition.  Chosen for maximum
 * backwards compatibility, ie failure -> 0, success -> 1.  */
typedef enum PyLockStatus {
    PY_LOCK_FAILURE = 0,
    PY_LOCK_ACQUIRED = 1,
    PY_LOCK_INTR
} PyLockStatus;

void PyThread_init_thread(void);
long PyThread_start_new_thread(void (*)(void *), void *);
void PyThread_exit_thread(void);
long PyThread_get_thread_ident(void);

PyThread_type_lock PyThread_allocate_lock(void);
void PyThread_free_lock(PyThread_type_lock);
int PyThread_acquire_lock(PyThread_type_lock, int);
#define WAIT_LOCK	1
#define NOWAIT_LOCK	0

/* PY_TIMEOUT_T is the integral type used to specify timeouts when waiting
   on a lock (see PyThread_acquire_lock_timed() below).
   PY_TIMEOUT_MAX is the highest usable value (in microseconds) of that
   type, and depends on the system threading API.

   NOTE: this isn't the same value as `_thread.TIMEOUT_MAX`.  The _thread
   module exposes a higher-level API, with timeouts expressed in seconds
   and floating-point numbers allowed.
*/
#define PY_TIMEOUT_T long long
#define PY_TIMEOUT_MAX PY_LLONG_MAX

/* In the NT API, the timeout is a DWORD and is expressed in milliseconds */
#if defined (NT_THREADS)
#if 0xFFFFFFFFLL * 1000 < PY_TIMEOUT_MAX
#undef PY_TIMEOUT_MAX
#define PY_TIMEOUT_MAX (0xFFFFFFFFLL * 1000)
#endif
#endif

/* If microseconds == 0, the call is non-blocking: it returns immediately
   even when the lock can't be acquired.
   If microseconds > 0, the call waits up to the specified duration.
   If microseconds < 0, the call waits until success (or abnormal failure)

   microseconds must be less than PY_TIMEOUT_MAX. Behaviour otherwise is
   undefined.

   If intr_flag is true and the acquire is interrupted by a signal, then the
   call will return PY_LOCK_INTR.  The caller may reattempt to acquire the
   lock.
*/
PyLockStatus PyThread_acquire_lock_timed(PyThread_type_lock,
                                                     PY_TIMEOUT_T microseconds,
                                                     int intr_flag);

void PyThread_release_lock(PyThread_type_lock);

size_t PyThread_get_stacksize(void);
int PyThread_set_stacksize(size_t);

#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyObject* PyThread_GetInfo(void);
#endif

/* Thread Local Storage (TLS) API */
int PyThread_create_key(void);
void PyThread_delete_key(int);
int PyThread_set_key_value(int, void *);
void * PyThread_get_key_value(int);
void PyThread_delete_key_value(int key);

/* Cleanup after a fork */
void PyThread_ReInitTLS(void);

COSMOPOLITAN_C_END_
#endif /* !Py_PYTHREAD_H */
