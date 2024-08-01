/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/maps.h"
#include "ape/sections.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/lock.h"

#ifdef __x86_64__
__static_yoink("_init_maps");
#endif

struct Maps __maps;

void __maps_add(struct Map *map) {
  tree_insert(&__maps.maps, &map->tree, __maps_compare);
  ++__maps.count;
}

static void __maps_adder(struct Map *map, int pagesz) {
  __maps.pages += ((map->size + pagesz - 1) & -pagesz) / pagesz;
  __maps_add(map);
}

void __maps_stack(char *stackaddr, int pagesz, int guardsize, size_t stacksize,
                  int stackprot, intptr_t stackhand) {
  __maps.stack.addr = stackaddr + guardsize;
  __maps.stack.size = stacksize - guardsize;
  __maps.stack.prot = stackprot;
  __maps.stack.hand = -1;
  __maps_adder(&__maps.stack, pagesz);
  if (guardsize) {
    __maps.guard.addr = stackaddr;
    __maps.guard.size = guardsize;
    __maps.guard.prot = PROT_NONE;
    __maps.guard.hand = stackhand;
    __maps_adder(&__maps.guard, pagesz);
  }
}

void __maps_init(void) {
  int pagesz = __pagesize;

  // record _start() stack mapping
  if (!IsWindows()) {
    struct AddrSize stack;
    stack = __get_main_stack();
    __maps_stack(stack.addr, pagesz, 0, stack.size, (uintptr_t)ape_stack_prot,
                 0);
  }

  // record .text and .data mappings
  static struct Map text, data;
  text.addr = (char *)__executable_start;
  text.size = _etext - __executable_start;
  text.prot = PROT_READ | PROT_EXEC;
  uintptr_t ds = ((uintptr_t)_etext + pagesz - 1) & -pagesz;
  if (ds < (uintptr_t)_end) {
    data.addr = (char *)ds;
    data.size = (uintptr_t)_end - ds;
    data.prot = PROT_READ | PROT_WRITE;
    __maps_adder(&data, pagesz);
  }
  __maps_adder(&text, pagesz);
}

privileged bool __maps_lock(void) {
  int me;
  uint64_t word, lock;
  struct CosmoTib *tib;
  if (!__tls_enabled)
    return false;
  if (!(tib = __get_tls_privileged()))
    return false;
  if (tib->tib_flags & TIB_FLAG_VFORKED)
    return false;
  me = atomic_load_explicit(&tib->tib_tid, memory_order_acquire);
  if (me <= 0)
    return false;
  word = atomic_load_explicit(&__maps.lock, memory_order_relaxed);
  for (;;) {
    if (MUTEX_OWNER(word) == me) {
      if (atomic_compare_exchange_weak_explicit(
              &__maps.lock, &word, MUTEX_INC_DEPTH(word), memory_order_relaxed,
              memory_order_relaxed))
        return true;
      continue;
    }
    word = 0;
    lock = MUTEX_LOCK(word);
    lock = MUTEX_SET_OWNER(lock, me);
    if (atomic_compare_exchange_weak_explicit(&__maps.lock, &word, lock,
                                              memory_order_acquire,
                                              memory_order_relaxed))
      return false;
    for (;;) {
      word = atomic_load_explicit(&__maps.lock, memory_order_relaxed);
      if (MUTEX_OWNER(word) == me)
        break;
      if (!word)
        break;
    }
  }
}

privileged void __maps_unlock(void) {
  int me;
  uint64_t word;
  struct CosmoTib *tib;
  if (!__tls_enabled)
    return;
  if (!(tib = __get_tls_privileged()))
    return;
  if (tib->tib_flags & TIB_FLAG_VFORKED)
    return;
  me = atomic_load_explicit(&tib->tib_tid, memory_order_acquire);
  if (me <= 0)
    return;
  word = atomic_load_explicit(&__maps.lock, memory_order_relaxed);
  for (;;) {
    if (MUTEX_DEPTH(word)) {
      if (atomic_compare_exchange_weak_explicit(
              &__maps.lock, &word, MUTEX_DEC_DEPTH(word), memory_order_relaxed,
              memory_order_relaxed))
        break;
    }
    if (atomic_compare_exchange_weak_explicit(
            &__maps.lock, &word, 0, memory_order_release, memory_order_relaxed))
      break;
  }
}
