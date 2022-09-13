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
#include "libc/errno.h"
#include "libc/intrin/bsr.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Allocates TLS slot.
 */
int pthread_key_create(pthread_key_t *key, pthread_key_dtor dtor) {
  int i, j, rc = EAGAIN;
  pthread_spin_lock(&_pthread_keys_lock);
  for (i = 0; i < (PTHREAD_KEYS_MAX + 63) / 64; ++i) {
    if (~_pthread_key_usage[i]) {
      j = _bsrl(~_pthread_key_usage[i]);
      _pthread_key_usage[i] |= 1ul << j;
      _pthread_key_dtor[i * 64 + j] = dtor;
      *key = i * 64 + j;
      rc = 0;
      break;
    }
  }
  pthread_spin_unlock(&_pthread_keys_lock);
  return rc;
}

static textexit void _pthread_key_atexit(void) {
  _pthread_key_destruct(0);
}

__attribute__((__constructor__)) static textstartup void _pthread_key_init() {
  atexit(_pthread_key_atexit);
}
