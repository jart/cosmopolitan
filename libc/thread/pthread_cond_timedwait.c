/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/lock.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/time.h"

__static_yoink("nsync_mu_lock");
__static_yoink("nsync_mu_unlock");
__static_yoink("nsync_mu_trylock");

struct PthreadWait {
  pthread_cond_t *cond;
  pthread_mutex_t *mutex;
};

static bool can_use_nsync(uint64_t muword) {
  return !IsXnuSilicon() &&  //
         MUTEX_TYPE(muword) != PTHREAD_MUTEX_RECURSIVE &&
         MUTEX_PSHARED(muword) == PTHREAD_PROCESS_PRIVATE;
}

static void pthread_cond_leave(void *arg) {
  struct PthreadWait *wait = (struct PthreadWait *)arg;
  if (_pthread_mutex_lock(wait->mutex))
    __builtin_trap();
  atomic_fetch_sub_explicit(&wait->cond->_waiters, 1, memory_order_acq_rel);
}

static errno_t pthread_cond_timedwait_impl(pthread_cond_t *cond,
                                           pthread_mutex_t *mutex,
                                           const struct timespec *abstime) {

  // this is a cancelation point
  // check the cancelation status before we begin waiting
  if (pthread_testcancel_np() == ECANCELED)
    return ECANCELED;

  // get original monotonic sequence while lock is held
  uint32_t seq1 = atomic_load_explicit(&cond->_sequence, memory_order_relaxed);

  // start waiting on condition variable
  atomic_fetch_add_explicit(&cond->_waiters, 1, memory_order_acq_rel);
  if (_pthread_mutex_unlock(mutex))
    __builtin_trap();

  // wait for sequence change, timeout, or cancelation
  int rc;
  struct PthreadWait waiter = {cond, mutex};
  pthread_cleanup_push(pthread_cond_leave, &waiter);
  rc = cosmo_futex_wait((atomic_int *)&cond->_sequence, seq1, cond->_pshared,
                        cond->_clock, abstime);
  pthread_cleanup_pop(true);
  if (rc == -EAGAIN)
    rc = 0;

  // turn linux syscall status into posix errno
  return -rc;
}

/**
 * Waits for condition with optional time limit, e.g.
 *
 *     struct timespec ts;  // one second timeout
 *     ts = timespec_add(timespec_real(), timespec_frommillis(1000));
 *     if (pthread_cond_timedwait(cond, mutex, &ts) == ETIMEDOUT) {
 *       // handle timeout...
 *     }
 *
 * @param mutex needs to be held by thread when calling this function
 * @param abstime is an absolute timestamp, which may be null to wait
 *     forever; it's relative to `clock_gettime(CLOCK_REALTIME)` by
 *     default; pthread_condattr_setclock() may be used to customize
 *     which system clock is used
 * @return 0 on success, or errno on error
 * @raise ETIMEDOUT if `abstime` was specified and the current time
 *     exceeded its value
 * @raise EPERM if `mutex` is `PTHREAD_MUTEX_ERRORCHECK` and the lock
 *     isn't owned by the current thread
 * @raise EINVAL if `0 ≤ abstime->tv_nsec < 1000000000` wasn't the case
 * @raise ECANCELED if calling thread was cancelled in masked mode
 * @see pthread_cond_broadcast()
 * @see pthread_cond_signal()
 * @cancelationpoint
 */
errno_t _pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                                const struct timespec *abstime) {

  // validate arguments
  struct PosixThread *pt;
  if (!(pt = _pthread_self()))
    return EINVAL;
  if (abstime && !(0 <= abstime->tv_nsec && abstime->tv_nsec < 1000000000))
    return EINVAL;

  // look at the mutex argument
  uint64_t muword = atomic_load_explicit(&mutex->_word, memory_order_relaxed);

  // check that mutex is held by caller
  if (IsModeDbg() || MUTEX_TYPE(muword) == PTHREAD_MUTEX_ERRORCHECK)
    if (__deadlock_tracked(mutex) == 0)
      return EPERM;

  // if the cond is process shared then the mutex needs to be too
  if ((cond->_pshared == PTHREAD_PROCESS_SHARED) ^
      (MUTEX_PSHARED(muword) == PTHREAD_PROCESS_SHARED))
    return EINVAL;

#if PTHREAD_USE_NSYNC
  // the first time pthread_cond_timedwait() is called we learn if the
  // associated mutex is normal and private. that means *NSYNC is safe
  // this decision is permanent. you can't use a recursive mutex later
  if (!atomic_load_explicit(&cond->_waited, memory_order_acquire)) {
    cond->_footek = !can_use_nsync(muword);
    atomic_store_explicit(&cond->_waited, true, memory_order_release);
  } else if (!cond->_footek) {
    if (!can_use_nsync(muword))
      return EINVAL;
  }
#endif

  // now perform the actual wait
  errno_t err;
  BEGIN_CANCELATION_POINT;
#if PTHREAD_USE_NSYNC
  // favor *NSYNC if this is a process private condition variable
  // if using Mike Burrows' code isn't possible, use a naive impl
  if (!cond->_footek) {
    err = nsync_cv_wait_with_deadline(
        (nsync_cv *)cond->_nsync, (nsync_mu *)mutex->_nsync, cond->_clock,
        abstime ? *abstime : nsync_time_no_deadline, 0);
  } else {
    err = pthread_cond_timedwait_impl(cond, mutex, abstime);
  }
#else
  err = pthread_cond_timedwait_impl(cond, mutex, abstime);
#endif
  END_CANCELATION_POINT;
  return err;
}

__weak_reference(_pthread_cond_timedwait, pthread_cond_timedwait);
