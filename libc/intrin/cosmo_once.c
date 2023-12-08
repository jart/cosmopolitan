/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/thread.h"

#define INIT     0
#define CALLING  1
#define FINISHED 2

/**
 * Ensures initialization function is called exactly once.
 *
 * This is the same as `pthread_once` except that it always uses a tiny
 * spinlock implementation and won't make any system calls. It's needed
 * since this function is an upstream dependency of both pthread_once()
 * and nsync_once(). Most code should favor calling those functions.
 *
 * @return 0 on success, or errno on error
 */
errno_t cosmo_once(atomic_uint *once, void init(void)) {
  uint32_t old;
  switch ((old = atomic_load_explicit(once, memory_order_acquire))) {
    case INIT:
      if (atomic_compare_exchange_strong_explicit(once, &old, CALLING,
                                                  memory_order_acquire,
                                                  memory_order_relaxed)) {
        init();
        atomic_store_explicit(once, FINISHED, memory_order_release);
        return 0;
      }
      // fallthrough
    case CALLING:
      for (;;) {
        if (atomic_load_explicit(once, memory_order_acquire) != CALLING) {
          break;
        }
      }
      return 0;
    case FINISHED:
      return 0;
    default:
      return EINVAL;
  }
}
