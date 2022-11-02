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
#include "libc/intrin/atomic.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

void _pthread_key_destruct(void) {
  int i, j, gotsome;
  void *val, **keys;
  pthread_key_dtor dtor;
  if (!__tls_enabled) return;
  keys = __get_tls()->tib_keys;
  for (j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; ++j) {
    for (gotsome = i = 0; i < PTHREAD_KEYS_MAX; ++i) {
      if ((val = keys[i]) &&
          (dtor = atomic_load_explicit(_pthread_key_dtor + i,
                                       memory_order_relaxed)) &&
          dtor != (pthread_key_dtor)-1) {
        gotsome = 1;
        keys[i] = 0;
        dtor(val);
      }
    }
    if (!gotsome) {
      break;
    }
  }
}
