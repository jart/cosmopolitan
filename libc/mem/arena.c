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
#include "libc/assert.h"
#include "libc/bits/likely.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/arena.h"
#include "libc/mem/hook/hook.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define BASE  ((char *)0x30000000)
#define LIMIT ((char *)0x50000000)

#define EXCHANGE(HOOK, SLOT) \
  __arena_hook((intptr_t *)weaken(HOOK), (intptr_t *)(&(SLOT)))

static struct Arena {
  bool once;
  uint8_t depth;
  unsigned size;
  unsigned offset[16];
  void (*free)(void *);
  void *(*malloc)(size_t);
  void *(*calloc)(size_t, size_t);
  void *(*memalign)(size_t, size_t);
  void *(*realloc)(void *, size_t);
  void *(*realloc_in_place)(void *, size_t);
  void *(*valloc)(size_t);
  void *(*pvalloc)(size_t);
  int (*malloc_trim)(size_t);
  size_t (*malloc_usable_size)(const void *);
  size_t (*bulk_free)(void *[], size_t);
} __arena;

static wontreturn void __arena_die(void) {
  if (weaken(__die)) weaken(__die)();
  _exit(83);
}

static wontreturn void __arena_not_implemented(void) {
  __printf("not implemented");
  __arena_die();
}

static void __arena_free(void *p) {
  if (!p) return;
  assert(__arena.depth);
  assert((intptr_t)BASE + __arena.offset[__arena.depth - 1] <= (intptr_t)p &&
         (intptr_t)p < (intptr_t)BASE + __arena.offset[__arena.depth]);
}

static size_t __arena_bulk_free(void *p[], size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (p[i]) __arena_free(p[i]);
  }
  bzero(p, n * sizeof(void *));
  return 0;
}

static void *__arena_malloc(size_t n) {
  char *ptr;
  size_t need, greed;
  assert(__arena.depth);
  if (!n) n = 1;
  if (n < LIMIT - BASE) {
    need = __arena.offset[__arena.depth] + n;
    need = ROUNDUP(need, __BIGGEST_ALIGNMENT__);
    if (UNLIKELY(need > __arena.size)) {
      greed = __arena.size + 1;
      do {
        greed += greed >> 1;
        greed = ROUNDUP(greed, FRAMESIZE);
      } while (need > greed);
      if (greed < LIMIT - BASE &&
          mmap(BASE + __arena.size, greed - __arena.size,
               PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED,
               -1, 0) != MAP_FAILED) {
        __arena.size = greed;
      } else {
        return 0;
      }
    }
    ptr = BASE + __arena.offset[__arena.depth];
    __arena.offset[__arena.depth] = need;
    return ptr;
  } else {
    return 0;
  }
}

static void *__arena_calloc(size_t n, size_t z) {
  if (__builtin_mul_overflow(n, z, &n)) n = -1;
  return __arena_malloc(n);
}

static void *__arena_memalign(size_t a, size_t n) {
  if (a <= __BIGGEST_ALIGNMENT__) {
    return __arena_malloc(n);
  } else {
    __arena_not_implemented();
  }
}

static void *__arena_realloc(void *p, size_t n) {
  if (p) {
    if (n) {
      __arena_not_implemented();
    } else {
      __arena_free(p);
      return 0;
    }
  } else {
    return __arena_malloc(n);
  }
}

static int __arena_malloc_trim(size_t n) {
  return 0;
}

static void *__arena_realloc_in_place(void *p, size_t n) {
  __arena_not_implemented();
}

static void *__arena_valloc(size_t n) {
  __arena_not_implemented();
}

static void *__arena_pvalloc(size_t n) {
  __arena_not_implemented();
}

static size_t __arena_malloc_usable_size(const void *p) {
  __arena_not_implemented();
}

static void __arena_hook(intptr_t *h, intptr_t *f) {
  intptr_t t;
  if (h) {
    t = *h;
    *h = *f;
    *f = t;
  }
}

static void __arena_install(void) {
  EXCHANGE(hook_free, __arena.free);
  EXCHANGE(hook_realloc, __arena.realloc);
  EXCHANGE(hook_realloc, __arena.realloc);
  EXCHANGE(hook_malloc, __arena.malloc);
  EXCHANGE(hook_calloc, __arena.calloc);
  EXCHANGE(hook_memalign, __arena.memalign);
  EXCHANGE(hook_realloc_in_place, __arena.realloc_in_place);
  EXCHANGE(hook_valloc, __arena.valloc);
  EXCHANGE(hook_pvalloc, __arena.pvalloc);
  EXCHANGE(hook_malloc_trim, __arena.malloc_trim);
  EXCHANGE(hook_malloc_usable_size, __arena.malloc_usable_size);
  EXCHANGE(hook_bulk_free, __arena.bulk_free);
}

static void __arena_destroy(void) {
  if (__arena.depth) {
    __arena_install();
  }
  if (__arena.size) {
    munmap(BASE, __arena.size);
  }
  bzero(&__arena, sizeof(__arena));
}

static void __arena_init(void) {
  __arena.free = __arena_free;
  __arena.realloc = __arena_realloc;
  __arena.realloc = __arena_realloc;
  __arena.malloc = __arena_malloc;
  __arena.calloc = __arena_calloc;
  __arena.memalign = __arena_memalign;
  __arena.realloc_in_place = __arena_realloc_in_place;
  __arena.valloc = __arena_valloc;
  __arena.pvalloc = __arena_pvalloc;
  __arena.malloc_trim = __arena_malloc_trim;
  __arena.malloc_usable_size = __arena_malloc_usable_size;
  __arena.bulk_free = __arena_bulk_free;
  atexit(__arena_destroy);
}

void __arena_push(void) {
  if (UNLIKELY(!__arena.once)) {
    __arena_init();
    __arena.once = true;
  }
  if (!__arena.depth) {
    __arena_install();
  } else if (__arena.depth == ARRAYLEN(__arena.offset) - 1) {
    __printf("too many arenas");
    __arena_die();
  }
  __arena.offset[__arena.depth + 1] = __arena.offset[__arena.depth];
  ++__arena.depth;
}

void __arena_pop(void) {
  unsigned greed;
  assert(__arena.depth);
  bzero(BASE + __arena.offset[__arena.depth - 1],
        __arena.offset[__arena.depth] - __arena.offset[__arena.depth - 1]);
  if (!--__arena.depth) __arena_install();
  greed = __arena.offset[__arena.depth];
  greed += FRAMESIZE;
  greed <<= 1;
  if (__arena.size > greed) {
    munmap(BASE + greed, __arena.size - greed);
  }
}
