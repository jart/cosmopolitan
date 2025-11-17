/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/lock.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Determines if mutex is held by calling thread.
 *
 * This check is only possible for recursive mutexes. It's also usually
 * possible in `cosmocc -mdbg` mode.
 *
 * @param mutex is lock object
 * @param result receives boolean result of check on success
 * @return 0 on success, or errno on error
 * @raise EINVAL if `mutex` doesn't support this check
 */
errno_t pthread_mutex_held_np(pthread_mutex_t *mutex, int *result) {
  uint64_t word = atomic_load_explicit(&mutex->_word, memory_order_relaxed);
  if (MUTEX_TYPE(word) == PTHREAD_MUTEX_RECURSIVE) {
    int me = atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
    *result = MUTEX_OWNER(word) == me;
    return 0;
  } else if (IsModeDbg()) {
    switch (__deadlock_tracked(mutex)) {
      case 1:
        *result = true;
        return 0;
      case 0:
        *result = false;
        return 0;
      default:
        return EINVAL;
    }
  } else {
    return EINVAL;
  }
}
