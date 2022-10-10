/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/weaken.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/once.h"

#define INIT     0
#define CALLING  1
#define FINISHED 2

/**
 * Ensures initialization function is called exactly once, e.g.
 *
 *     static void *g_factory;
 *
 *     static void InitFactory(void) {
 *       g_factory = expensive();
 *     }
 *
 *     void *GetFactory(void) {
 *       static pthread_once_t once = PTHREAD_ONCE_INIT;
 *       pthread_once(&once, InitFactory);
 *       return g_factory;
 *     }
 *
 * @return 0 on success, or errno on error
 */
errno_t pthread_once(pthread_once_t *once, void init(void)) {
  uint32_t old;
  if (_weaken(nsync_run_once)) {
    _weaken(nsync_run_once)((nsync_once *)once, init);
    return 0;
  }
  switch ((old = atomic_load_explicit(&once->_lock, memory_order_relaxed))) {
    case INIT:
      if (atomic_compare_exchange_strong_explicit(&once->_lock, &old, CALLING,
                                                  memory_order_acquire,
                                                  memory_order_relaxed)) {
        init();
        atomic_store_explicit(&once->_lock, FINISHED, memory_order_release);
        return 0;
      }
      // fallthrough
    case CALLING:
      do {
        pthread_yield();
      } while (atomic_load_explicit(&once->_lock, memory_order_acquire) ==
               CALLING);
      return 0;
    case FINISHED:
      return 0;
    default:
      return EINVAL;
  }
}
