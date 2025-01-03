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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/maps.h"
#include "libc/macros.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/lock.h"
#include "libc/thread/tls.h"

#ifdef __x86_64__
__static_yoink("_init_maps");
#endif

#define ABI privileged optimizespeed

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
  __maps.stack.hand = MAPS_SUBREGION;
  __maps.stack.flags = MAP_PRIVATE | MAP_ANONYMOUS;
  __maps_adder(&__maps.stack, pagesz);
  if (guardsize) {
    __maps.guard.addr = stackaddr;
    __maps.guard.size = guardsize;
    __maps.guard.prot = PROT_NONE | PROT_GUARD;
    __maps.guard.hand = stackhand;
    __maps.guard.flags = MAP_PRIVATE | MAP_ANONYMOUS;
    __maps_adder(&__maps.guard, pagesz);
  } else {
    __maps.stack.hand = stackhand;
  }
}

void __maps_init(void) {
  int pagesz = __pagesize;

  // initialize lemur64
  __maps.rand = 2131259787901769494;
  __maps.rand ^= kStartTsc;

  // these static map objects avoid mandatory mmap() in __maps_alloc()
  // they aren't actually needed for bootstrapping this memory manager
  for (int i = 0; i < ARRAYLEN(__maps.spool); ++i)
    __maps_free(&__maps.spool[i]);

  // record _start() stack mapping
  if (!IsWindows()) {

    // linux v4.12+ reserves 1mb of guard space beneath rlimit_stack
    // https://lwn.net/Articles/725832/. if we guess too small, then
    // slackmap will create a bunch of zombie stacks in __print_maps
    // to coverup the undisclosed memory but no cost if we guess big
    size_t guardsize = 1024 * 1024;
    guardsize += __pagesize - 1;
    guardsize &= -__pagesize;

    // track the main stack region that the os gave to start earlier
    struct AddrSize stack = __get_main_stack();
    __maps_stack(stack.addr - guardsize, pagesz, guardsize,
                 guardsize + stack.size, (uintptr_t)ape_stack_prot, 0);
  }

  // record .text and .data mappings
  __maps_track((char *)__executable_start, _etext - __executable_start,
               PROT_READ | PROT_EXEC, MAP_NOFORK);
  uintptr_t ds = ((uintptr_t)_etext + pagesz - 1) & -pagesz;
  if (ds < (uintptr_t)_end)
    __maps_track((char *)ds, (uintptr_t)_end - ds, PROT_READ | PROT_WRITE,
                 MAP_NOFORK);
}

bool __maps_held(void) {
  return !__tls_enabled || (__get_tls()->tib_flags & TIB_FLAG_VFORKED) ||
         MUTEX_OWNER(
             atomic_load_explicit(&__maps.lock.word, memory_order_relaxed)) ==
             atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
}

bool __maps_reentrant(void) {
  return __tls_enabled && !(__get_tls()->tib_flags & TIB_FLAG_VFORKED) &&
         MUTEX_OWNER(
             atomic_load_explicit(&__maps.lock.word, memory_order_relaxed)) ==
             atomic_load_explicit(&__get_tls()->tib_ptid, memory_order_relaxed);
}

ABI void __maps_lock(void) {
  int me;
  uint64_t word, lock;
  struct CosmoTib *tib;
  if (!__tls_enabled)
    return;
  if (!(tib = __get_tls_privileged()))
    return;
  if (tib->tib_flags & TIB_FLAG_VFORKED)
    return;
  me = atomic_load_explicit(&tib->tib_ptid, memory_order_relaxed);
  word = 0;
  lock = MUTEX_LOCK(word);
  lock = MUTEX_SET_OWNER(lock, me);
  if (atomic_compare_exchange_strong_explicit(&__maps.lock.word, &word, lock,
                                              memory_order_acquire,
                                              memory_order_relaxed))
    return;
  word = atomic_load_explicit(&__maps.lock.word, memory_order_relaxed);
  for (;;) {
    if (MUTEX_OWNER(word) == me) {
      if (atomic_compare_exchange_weak_explicit(
              &__maps.lock.word, &word, MUTEX_INC_DEPTH(word),
              memory_order_relaxed, memory_order_relaxed))
        return;
      continue;
    }
    word = 0;
    lock = MUTEX_LOCK(word);
    lock = MUTEX_SET_OWNER(lock, me);
    if (atomic_compare_exchange_weak_explicit(&__maps.lock.word, &word, lock,
                                              memory_order_acquire,
                                              memory_order_relaxed))
      return;
    for (;;) {
      word = atomic_load_explicit(&__maps.lock.word, memory_order_relaxed);
      if (MUTEX_OWNER(word) == me)
        break;
      if (!word)
        break;
    }
  }
}

ABI void __maps_unlock(void) {
  uint64_t word;
  struct CosmoTib *tib;
  if (!__tls_enabled)
    return;
  if (!(tib = __get_tls_privileged()))
    return;
  if (tib->tib_flags & TIB_FLAG_VFORKED)
    return;
  word = atomic_load_explicit(&__maps.lock.word, memory_order_relaxed);
  for (;;) {
    if (MUTEX_DEPTH(word))
      if (atomic_compare_exchange_weak_explicit(
              &__maps.lock.word, &word, MUTEX_DEC_DEPTH(word),
              memory_order_relaxed, memory_order_relaxed))
        break;
    if (atomic_compare_exchange_weak_explicit(&__maps.lock.word, &word, 0,
                                              memory_order_release,
                                              memory_order_relaxed))
      break;
  }
}
