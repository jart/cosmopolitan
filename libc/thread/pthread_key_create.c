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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

/**
 * Allocates TLS slot.
 *
 * This function creates a thread-local storage registration, that will
 * apply to all threads. The new identifier is written to `key`, and it
 * can be passed to the pthread_setspecific() and pthread_getspecific()
 * functions to set and get its associated value. Each thread will have
 * its key value initialized to zero upon creation. It is also possible
 * to use pthread_key_delete() to unregister a key.
 *
 * If `dtor` is non-null, then it'll be called upon pthread_exit() when
 * the key's value is nonzero. The key's value is set to zero before it
 * is called. The ordering of multiple destructor calls is unspecified.
 * The same key can be destroyed `PTHREAD_DESTRUCTOR_ITERATIONS` times,
 * in cases where it gets set again by a destructor.
 *
 * @param key is set to the allocated key on success
 * @param dtor specifies an optional destructor callback
 * @return 0 on success, or errno on error
 * @raise EAGAIN if `PTHREAD_KEYS_MAX` keys exist
 */
int pthread_key_create(pthread_key_t *key, pthread_key_dtor dtor) {
  int i;
  pthread_key_dtor expect;
  if (!dtor) dtor = (pthread_key_dtor)-1;
  for (i = 0; i < PTHREAD_KEYS_MAX; ++i) {
    if (!(expect = atomic_load_explicit(_pthread_key_dtor + i,
                                        memory_order_acquire)) &&
        atomic_compare_exchange_strong_explicit(_pthread_key_dtor + i, &expect,
                                                dtor, memory_order_release,
                                                memory_order_relaxed)) {
      *key = i;
      return 0;
    }
  }
  return EAGAIN;
}
