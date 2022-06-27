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
#include "libc/bits/atomic.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/thread.h"

static struct Zombie {
  struct Zombie *next;
  cthread_t td;
} * cthread_zombies;

void cthread_zombies_add(cthread_t td) {
  struct Zombie *z;
  if ((z = malloc(sizeof(struct Zombie)))) {
    z->td = td;
    z->next = atomic_load(&cthread_zombies);
    for (;;) {
      if (atomic_compare_exchange_weak(&cthread_zombies, &z->next, z)) {
        break;
      }
    }
  }
}

void cthread_zombies_reap(void) {
  struct Zombie *z;
  // TODO(jart): Is this right? Update to not use malloc/free?
  while ((z = atomic_load(&cthread_zombies)) && !atomic_load(&z->td->tid)) {
    if (atomic_compare_exchange_weak(&cthread_zombies, &z, z->next)) {
      munmap(z->td->alloc.bottom, z->td->alloc.top - z->td->alloc.bottom);
      free(z);
    }
  }
}
