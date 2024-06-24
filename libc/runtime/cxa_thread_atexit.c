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
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/wait_s.internal.h"

struct Dtor {
  void *fun;
  void *arg;
  struct Dtor *next;
};

static _Thread_local struct Dtor *__cxa_thread_atexit_list;

static void _pthread_unwind(struct CosmoTib *tib) {
  struct PosixThread *pt;
  struct _pthread_cleanup_buffer *cb;
  pt = (struct PosixThread *)tib->tib_pthread;
  while ((cb = pt->pt_cleanup)) {
    pt->pt_cleanup = cb->__prev;
    cb->__routine(cb->__arg);
  }
}

static void __cxa_thread_unkey(struct CosmoTib *tib) {
  void *val;
  int i, j, gotsome;
  pthread_key_dtor dtor;
  for (j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; ++j) {
    for (gotsome = i = 0; i < PTHREAD_KEYS_MAX; ++i) {
      if ((val = tib->tib_keys[i]) &&
          (dtor = atomic_load_explicit(_pthread_key_dtor + i,
                                       memory_order_relaxed)) &&
          dtor != (pthread_key_dtor)-1) {
        gotsome = 1;
        tib->tib_keys[i] = 0;
        dtor(val);
      }
    }
    if (!gotsome) {
      break;
    }
  }
}

static void _pthread_ungarbage(struct CosmoTib *tib) {
  struct Garbages *g;
  while ((g = tib->tib_garbages)) {
    tib->tib_garbages = 0;
    while (g->i--) {
      ((void (*)(intptr_t))g->p[g->i].fn)(g->p[g->i].arg);
    }
    _weaken(free)(g->p);
    _weaken(free)(g);
  }
}

void __cxa_thread_finalize(void) {
  struct Dtor *dtor;
  struct CosmoTib *tib;
  tib = __get_tls();
  _pthread_unwind(tib);
  if (tib->tib_nsync)
    _weaken(nsync_waiter_destroy)(tib->tib_nsync);
  __cxa_thread_unkey(tib);
  _pthread_ungarbage(tib);
  while ((dtor = __cxa_thread_atexit_list)) {
    __cxa_thread_atexit_list = dtor->next;
    ((void (*)(void *))dtor->fun)(dtor->arg);
    _weaken(free)(dtor);
  }
}

int __cxa_thread_atexit_impl(void *fun, void *arg, void *dso_symbol) {
  struct Dtor *dtor;
  if (!_weaken(malloc))
    return -1;
  if (!(dtor = _weaken(malloc)(sizeof(struct Dtor))))
    return -1;
  dtor->fun = fun;
  dtor->arg = arg;
  dtor->next = __cxa_thread_atexit_list;
  __cxa_thread_atexit_list = dtor;
  return 0;
}
