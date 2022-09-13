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
#include "libc/intrin/likely.h"
#include "libc/intrin/weaken.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"

/**
 * Releases mutex.
 *
 * @return 0 on success or error number on failure
 * @raises EPERM if in error check mode and not owned by caller
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  int c, t;

  if (LIKELY(__tls_enabled &&                               //
             mutex->_type == PTHREAD_MUTEX_NORMAL &&        //
             mutex->_pshared == PTHREAD_PROCESS_PRIVATE &&  //
             _weaken(nsync_mu_unlock))) {
    _weaken(nsync_mu_unlock)((nsync_mu *)mutex);
    return 0;
  }

  if (mutex->_type == PTHREAD_MUTEX_NORMAL) {
    atomic_store_explicit(&mutex->_lock, 0, memory_order_release);
    return 0;
  }

  t = gettid();
  if (mutex->_type == PTHREAD_MUTEX_ERRORCHECK) {
    c = atomic_load_explicit(&mutex->_lock, memory_order_relaxed);
    if ((c & 0x000fffff) != t) {
      return EPERM;
    }
  }

  c = atomic_fetch_sub(&mutex->_lock, 0x00100000);
  if ((c & 0x0ff00000) == 0x00100000) {
    atomic_store_explicit(&mutex->_lock, 0, memory_order_release);
    if ((c & 0xf0000000) == 0x20000000) {
      // _futex_wake(&mutex->_lock, 1, mutex->_pshared);
      pthread_yield();
    }
  }

  return 0;
}
