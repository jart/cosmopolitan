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
#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/likely.h"
#include "libc/mem/mem.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

dontinline static int pthread_setspecific_slow(pthread_key_t k,
                                               const void *val) {
  if (k >= PTHREAD_KEYS_MAX)
    return EINVAL;
  struct CosmoTib *tib = __get_tls();
  if (!tib->tib_keys_dynamic)
    if (!(tib->tib_keys_dynamic =
              calloc(PTHREAD_KEYS_MAX - PTHREAD_KEYS_FAST, sizeof(void *))))
      return ENOMEM;
  k -= PTHREAD_KEYS_FAST;
  tib->tib_keys_dynamic[k] = (void *)val;
  return 0;
}

/**
 * Sets value of TLS slot for current thread.
 *
 * If `k` wasn't created by pthread_key_create() then the behavior is
 * undefined. If `k` was unregistered earlier by pthread_key_delete()
 * then the behavior is undefined.
 *
 * @return 0 on success, or errno on error
 * @raise ENOMEM if we ran out of memory
 */
int pthread_setspecific(pthread_key_t k, const void *val) {
#ifdef MODE_DBG
  // "The effect of calling pthread_getspecific() or
  //  pthread_setspecific() with a key value not obtained from
  //  pthread_key_create() or after key has been deleted with
  //  pthread_key_delete() is undefined."
  //                                  ──Quoth POSIX.1-2017
  unassert(k < PTHREAD_KEYS_MAX);
  unassert(atomic_load_explicit(&_pthread_key_dtor[k], memory_order_acquire));
#endif
  if (LIKELY(k < PTHREAD_KEYS_FAST)) {
    __get_tls()->tib_keys_static[k] = (void *)val;
    return 0;
  }
  return pthread_setspecific_slow(k, val);
}
