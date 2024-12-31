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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/cosmo.h"
#include "libc/cxxabi.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cxaatexit.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/wait_s.internal.h"

/**
 * Terminates current POSIX thread.
 *
 * For example, a thread could terminate early as follows:
 *
 *     pthread_exit((void *)123);
 *
 * The result value could then be obtained when joining the thread:
 *
 *     void *rc;
 *     pthread_join(id, &rc);
 *     assert((intptr_t)rc == 123);
 *
 * Under normal circumstances a thread can exit by simply returning from
 * the callback function that was supplied to pthread_create(). This may
 * be used if the thread wishes to exit at any other point in the thread
 * lifecycle, in which case this function is responsible for ensuring we
 * invoke gc(), _defer(), and pthread_cleanup_push() callbacks, and also
 * pthread_key_create() destructors.
 *
 * If the current thread is an orphaned thread, or is the main thread
 * when no other threads were created, then this will terminated your
 * process with an exit code of zero. It's not possible to supply a
 * non-zero exit status to wait4() via this function.
 *
 * Once a thread has exited, access to its stack memory is undefined.
 * The behavior of calling pthread_exit() from cleanup handlers and key
 * destructors is also undefined.
 *
 * @param rc is reported later to pthread_join()
 * @noreturn
 */
wontreturn void pthread_exit(void *rc) {
  unsigned population;
  struct CosmoTib *tib;
  struct PosixThread *pt;
  enum PosixThreadStatus status, transition;

  STRACE("pthread_exit(%p)", rc);

  // get posix thread object
  tib = __get_tls();
  pt = (struct PosixThread *)tib->tib_pthread;

  // "The behavior of pthread_exit() is undefined if called from a
  //  cancellation cleanup handler or destructor function that was
  //  invoked as a result of either an implicit or explicit call to
  //  pthread_exit()." ──Quoth POSIX.1-2017
  unassert(!(pt->pt_flags & PT_EXITING));

  // set state
  pt->pt_flags |= PT_NOCANCEL | PT_EXITING;
  pt->pt_val = rc;

  // free resources
  __cxa_thread_finalize();

  // run atexit handlers if orphaned thread
  // notice how we avoid acquiring the pthread gil
  if (!(population = atomic_fetch_sub(&_pthread_count, 1) - 1)) {
    // we know for certain we're an orphan. any other threads that
    // exist, will terminate and clear their tid very soon. but some
    // goofball could spawn more threads from atexit() handlers. we'd
    // also like to avoid looping forever here, by auto-joining threads
    // that leaked, because the user forgot to join them or detach them
    for (;;) {
      if (_weaken(__cxa_finalize))
        _weaken(__cxa_finalize)(NULL);
      _pthread_decimate(kPosixThreadTerminated);
      if (pthread_orphan_np()) {
        population = atomic_load(&_pthread_count);
        break;
      }
    }
  }

  // transition the thread to a terminated state
  status = atomic_load_explicit(&pt->pt_status, memory_order_acquire);
  do {
    if (status == kPosixThreadZombie) {
      transition = kPosixThreadZombie;
      break;
    } else if (status == kPosixThreadTerminated) {
      transition = kPosixThreadTerminated;
      break;
    } else if (status == kPosixThreadJoinable) {
      transition = kPosixThreadTerminated;
    } else if (status == kPosixThreadDetached) {
      transition = kPosixThreadZombie;
    } else {
      __builtin_trap();
    }
  } while (!atomic_compare_exchange_weak_explicit(
      &pt->pt_status, &status, transition, memory_order_release,
      memory_order_relaxed));

  // make this thread a zombie if it was detached
  if (transition == kPosixThreadZombie)
    _pthread_zombify(pt);

  // "The process shall exit with an exit status of 0 after the last
  //  thread has been terminated. The behavior shall be as if the
  //  implementation called exit() with a zero argument at thread
  //  termination time." ──Quoth POSIX.1-2017
  if (!population) {
    for (int i = __fini_array_end - __fini_array_start; i--;)
      ((void (*)(void))__fini_array_start[i])();
    _Exit(0);
  }

  // check if the main thread has died whilst children live
  // note that the main thread is joinable by child threads
  if (pt->pt_flags & PT_STATIC) {
    atomic_store_explicit(&tib->tib_ctid, 0, memory_order_release);
    cosmo_futex_wake((atomic_int *)&tib->tib_ctid, INT_MAX,
                     !IsWindows() && !IsXnu());
    _Exit1(0);
  }

  // this is a child thread
  __builtin_longjmp(pt->pt_exiter, 1);
}

__weak_reference(pthread_exit, thr_exit);
