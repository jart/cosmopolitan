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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"

/**
 * Releases mutex.
 *
 * This function does nothing in vfork() children.
 *
 * @return 0 on success or error number on failure
 * @raises EPERM if in error check mode and not owned by caller
 * @vforksafe
 */
errno_t pthread_mutex_unlock(pthread_mutex_t *mutex) {
  int t;

  LOCKTRACE("pthread_mutex_unlock(%t)", mutex);

  if (mutex->_type == PTHREAD_MUTEX_NORMAL &&        //
      mutex->_pshared == PTHREAD_PROCESS_PRIVATE &&  //
      _weaken(nsync_mu_unlock)) {
    _weaken(nsync_mu_unlock)((nsync_mu *)mutex);
    return 0;
  }

  if (mutex->_type == PTHREAD_MUTEX_NORMAL) {
    atomic_store_explicit(&mutex->_lock, 0, memory_order_release);
    return 0;
  }

  t = gettid();

  // we allow unlocking an initialized lock that wasn't locked, but we
  // don't allow unlocking a lock held by another thread, or unlocking
  // recursive locks from a forked child, since it should be re-init'd
  if (mutex->_owner && (mutex->_owner != t || mutex->_pid != __pid)) {
    return EPERM;
  }

  if (mutex->_depth) {
    --mutex->_depth;
    return 0;
  }

  mutex->_owner = 0;
  atomic_store_explicit(&mutex->_lock, 0, memory_order_release);

  return 0;
}
