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
#include "libc/cxxabi.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/wait_s.internal.h"

void _pthread_unwind(struct PosixThread *pt) {
  struct _pthread_cleanup_buffer *cb;
  while ((cb = pt->pt_cleanup)) {
    pt->pt_cleanup = cb->__prev;
    cb->__routine(cb->__arg);
  }
}

void _pthread_unkey(struct CosmoTib *tib) {
  int i, j, gotsome;
  void *val, **keys;
  pthread_key_dtor dtor;
  if ((keys = tib->tib_keys)) {
    for (j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; ++j) {
      for (gotsome = i = 0; i < PTHREAD_KEYS_MAX; ++i) {
        if ((val = keys[i]) &&
            (dtor = atomic_load_explicit(_pthread_key_dtor + i,
                                         memory_order_relaxed)) &&
            dtor != (pthread_key_dtor)-1) {
          gotsome = 1;
          keys[i] = 0;
          dtor(val);
        }
      }
      if (!gotsome) {
        break;
      }
    }
    free(keys);
  }
}

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
  struct CosmoTib *tib;
  struct PosixThread *pt;
  enum PosixThreadStatus status, transition;

  tib = __get_tls();
  pt = (struct PosixThread *)tib->tib_pthread;
  pt->pt_flags |= PT_NOCANCEL;
  pt->pt_rc = rc;

  STRACE("pthread_exit(%p)", rc);

  // free resources
  _pthread_unwind(pt);
  if (_weaken(__cxa_thread_finalize)) {
    _weaken(__cxa_thread_finalize)();
  }
  _pthread_unkey(tib);
  if (tib->tib_nsync) {
    nsync_waiter_destroy(tib->tib_nsync);
  }
  _pthread_ungarbage();
  _pthread_decimate();

  // run atexit handlers if orphaned thread
  if (pthread_orphan_np()) {
    if (_weaken(__cxa_finalize)) {
      _weaken(__cxa_finalize)(NULL);
    }
  }

  // transition the thread to a terminated state
  status = atomic_load_explicit(&pt->pt_status, memory_order_acquire);
  do {
    switch (status) {
      case kPosixThreadJoinable:
        transition = kPosixThreadTerminated;
        break;
      case kPosixThreadDetached:
        transition = kPosixThreadZombie;
        break;
      default:
        __builtin_unreachable();
    }
  } while (!atomic_compare_exchange_weak_explicit(
      &pt->pt_status, &status, transition, memory_order_release,
      memory_order_relaxed));

  // make this thread a zombie if it was detached
  if (transition == kPosixThreadZombie) {
    _pthread_zombify(pt);
  }

  // check if this is the last survivor
  if (pthread_orphan_np()) {
    for (const uintptr_t *p = __fini_array_end; p > __fini_array_start;) {
      ((void (*)(void))(*--p))();
    }
    _Exit(0);
  }

  // check if the main thread has died whilst children live
  // note that the main thread is joinable by child threads
  if (pt->pt_flags & PT_STATIC) {
    atomic_store_explicit(&tib->tib_tid, 0, memory_order_release);
    nsync_futex_wake_(&tib->tib_tid, INT_MAX, !IsWindows() && !IsXnu());
    _Exit1(0);
  }

  // this is a child thread
  longjmp(pt->pt_exiter, 1);
}
