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
#include "libc/thread/thread.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/spawn.h"

/**
 * @fileoverview Memory collector for detached threads.
 */

static struct Zombie {
  struct Zombie *next;
  struct PosixThread *pt;
} * _pthread_zombies;

void _pthread_zombies_add(struct PosixThread *pt) {
  struct Zombie *z;
  if ((z = malloc(sizeof(struct Zombie)))) {
    z->pt = pt;
    z->next = atomic_load(&_pthread_zombies);
    for (;;) {
      if (atomic_compare_exchange_weak(&_pthread_zombies, &z->next, z)) {
        break;
      }
    }
  }
}

static void _pthread_zombies_collect(struct Zombie *z) {
  _pthread_wait(z->pt);
  _pthread_free(z->pt);
  free(z);
}

void _pthread_zombies_decimate(void) {
  struct Zombie *z;
  while ((z = atomic_load_explicit(&_pthread_zombies, memory_order_relaxed)) &&
         atomic_load(&z->pt->status) == kPosixThreadZombie) {
    if (atomic_compare_exchange_weak(&_pthread_zombies, &z, z->next)) {
      _pthread_zombies_collect(z);
    }
  }
}

void _pthread_zombies_harvest(void) {
  struct Zombie *z;
  while ((z = atomic_load_explicit(&_pthread_zombies, memory_order_relaxed))) {
    if (atomic_compare_exchange_weak(&_pthread_zombies, &z, z->next)) {
      _pthread_zombies_collect(z);
    }
  }
}

__attribute__((__constructor__)) static void init(void) {
  atexit(_pthread_zombies_harvest);
}
