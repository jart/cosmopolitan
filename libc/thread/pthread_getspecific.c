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
#include "libc/intrin/atomic.h"
#include "libc/mem/mem.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

// this is a legacy api so we avoid making the tib 1024 bytes larger
static void pthread_key_init(void) {
  if (!__get_tls()->tib_keys) {
    __get_tls()->tib_keys = calloc(PTHREAD_KEYS_MAX, sizeof(void *));
  }
}

/**
 * Sets value of TLS slot for current thread.
 *
 * If `k` wasn't created by pthread_key_create() then the behavior is
 * undefined. If `k` was unregistered earlier by pthread_key_delete()
 * then the behavior is undefined.
 */
int pthread_setspecific(pthread_key_t k, const void *val) {
  // "The effect of calling pthread_getspecific() or
  //  pthread_setspecific() with a key value not obtained from
  //  pthread_key_create() or after key has been deleted with
  //  pthread_key_delete() is undefined."
  //                                  ──Quoth POSIX.1-2017
  pthread_key_init();
  unassert(0 <= k && k < PTHREAD_KEYS_MAX);
  unassert(atomic_load_explicit(_pthread_key_dtor + k, memory_order_acquire));
  __get_tls()->tib_keys[k] = (void *)val;
  return 0;
}

/**
 * Gets value of TLS slot for current thread.
 *
 * If `k` wasn't created by pthread_key_create() then the behavior is
 * undefined. If `k` was unregistered earlier by pthread_key_delete()
 * then the behavior is undefined.
 */
void *pthread_getspecific(pthread_key_t k) {
  // "The effect of calling pthread_getspecific() or
  //  pthread_setspecific() with a key value not obtained from
  //  pthread_key_create() or after key has been deleted with
  //  pthread_key_delete() is undefined."
  //                                  ──Quoth POSIX.1-2017
  pthread_key_init();
  unassert(0 <= k && k < PTHREAD_KEYS_MAX);
  unassert(atomic_load_explicit(_pthread_key_dtor + k, memory_order_acquire));
  return __get_tls()->tib_keys[k];
}
