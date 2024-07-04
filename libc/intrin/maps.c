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
#include "libc/dce.h"
#include "libc/intrin/dll.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/prot.h"

#ifdef __x86_64__
__static_yoink("_init_maps");
#endif

struct Maps __maps;

void __maps_add(struct Map *map) {
  dll_init(&map->elem);
  dll_make_first(&__maps.used, &map->elem);
  map->next = __maps.maps;
  __maps.maps = map;
  ++__maps.count;
}

static void __maps_adder(struct Map *map, int pagesz) {
  __maps.pages += ((map->size + pagesz - 1) & -pagesz) / pagesz;
  __maps_add(map);
}

void __maps_stack(void *stackaddr, int pagesz, size_t stacksize, int stackprot,
                  intptr_t stackhand) {
  __maps.stack.addr = stackaddr;
  __maps.stack.size = stacksize;
  __maps.stack.prot = stackprot;
  __maps.stack.h = stackhand;
  __maps_adder(&__maps.stack, pagesz);
}

void __maps_init(void) {
  int pagesz = getauxval(AT_PAGESZ);

  // record _start() stack mapping
  if (!IsWindows()) {
    struct AddrSize stack;
    stack = __get_main_stack();
    __maps_stack(stack.addr, pagesz, stack.size, (uintptr_t)ape_stack_prot, 0);
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

privileged void __maps_lock(void) {
  struct CosmoTib *tib;
  if (!__threaded)
    return;
  if (!__tls_enabled)
    return;
  tib = __get_tls_privileged();
  if (tib->tib_relock_maps++)
    return;
  while (atomic_exchange_explicit(&__maps.lock, 1, memory_order_acquire)) {
#if defined(__GNUC__) && defined(__aarch64__)
    __asm__ volatile("yield");
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile("pause");
#endif
  }
}

privileged void __maps_unlock(void) {
  struct CosmoTib *tib;
  if (!__threaded)
    return;
  if (!__tls_enabled)
    return;
  tib = __get_tls_privileged();
  if (!--tib->tib_relock_maps)
    atomic_store_explicit(&__maps.lock, 0, memory_order_release);
}
