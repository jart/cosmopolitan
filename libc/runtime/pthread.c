/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"

typedef void *pthread_t;
typedef bool pthread_once_t;
typedef int pthread_mutex_t;

int pthread_once(pthread_once_t *once, void init(void)) {
  if (lockcmpxchg(once, 0, 1)) init();
  return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
  return EINVAL;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
  return EINVAL;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  return EPERM;
}

int pthread_cancel(pthread_t thread) {
  return ESRCH;
}

void *__tls_get_addr(size_t v[2]) {
  return NULL;
}
