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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/arena.h"
#include "libc/mem/hook/hook.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#define BASE 0x50040000
#define SIZE 0x2ff80000
#define P(i) ((void *)(intptr_t)(i))
#define EXCHANGE(HOOK, SLOT) \
  __arena_hook((intptr_t *)_weaken(HOOK), (intptr_t *)(&(SLOT)))

static struct Arena {
  bool once;
  size_t size;
  size_t depth;
  size_t offset[16];
  void (*free)(void *);
  void *(*malloc)(size_t);
  void *(*calloc)(size_t, size_t);
  void *(*memalign)(size_t, size_t);
  void *(*realloc)(void *, size_t);
  void *(*realloc_in_place)(void *, size_t);
  size_t (*malloc_usable_size)(const void *);
  size_t (*bulk_free)(void *[], size_t);
  int (*malloc_trim)(size_t);
} __arena;

static wontreturn void __arena_die(void) {
  if (_weaken(__die)) _weaken(__die)();
  _exit(83);
}

forceinline void __arena_check(void) {
  _unassert(__arena.depth);
}

forceinline void __arena_check_pointer(void *p) {
  _unassert(BASE + __arena.offset[__arena.depth - 1] <= (uintptr_t)p &&
            (uintptr_t)p < BASE + __arena.offset[__arena.depth]);
}

forceinline bool __arena_is_arena_pointer(void *p) {
  return BASE <= (uintptr_t)p && (uintptr_t)p < BASE + SIZE;
}

forceinline size_t __arena_get_size(void *p) {
  return *(const size_t *)((const char *)p - sizeof(size_t));
}

static void __arena_free(void *p) {
  __arena_check();
  if (p) {
    __arena_check_pointer(p);
    if (!(BASE <= (uintptr_t)p && (uintptr_t)p < BASE + SIZE)) {
      __arena.free(p);
    }
  }
}

static size_t __arena_bulk_free(void *p[], size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    __arena_free(p[i]);
    p[i] = 0;
  }
  return 0;
}

static dontinline bool __arena_grow(size_t offset, size_t request) {
  size_t greed;
  greed = __arena.size + 1;
  do {
    greed += greed >> 1;
    greed = ROUNDUP(greed, FRAMESIZE);
  } while (greed < offset + request);
  if (greed <= SIZE) {
    if (mmap(P(BASE + __arena.size), greed - __arena.size,
             PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED,
             -1, 0) != MAP_FAILED) {
      __arena.size = greed;
      return true;
    }
  } else {
    enomem();
  }
  if (_weaken(__oom_hook)) {
    _weaken(__oom_hook)(request);
  }
  return false;
}

static inline void *__arena_alloc(size_t a, size_t n) {
  size_t o;
  if (!n) n = 1;
  o = ROUNDUP(__arena.offset[__arena.depth] + sizeof(size_t), a);
  if (o + n >= n) {
    if (n <= sizeof(size_t)) {
      n = sizeof(size_t);
    } else {
      n = ROUNDUP(n, sizeof(size_t));
    }
    if (o + n <= SIZE) {
      if (UNLIKELY(o + n > __arena.size)) {
        if (!__arena_grow(o, n)) return 0;
      }
      __arena.offset[__arena.depth] = o + n;
      *(size_t *)(BASE + o - sizeof(size_t)) = n;
      return (void *)(BASE + o);
    }
  }
  enomem();
  return 0;
}

static void *__arena_malloc(size_t n) {
  __arena_check();
  return __arena_alloc(16, n);
}

static void *__arena_calloc(size_t n, size_t z) {
  __arena_check();
  if (__builtin_mul_overflow(n, z, &n)) n = -1;
  return __arena_alloc(16, n);
}

static void *__arena_memalign(size_t a, size_t n) {
  __arena_check();
  if (a <= sizeof(size_t)) {
    return __arena_alloc(8, n);
  } else {
    return __arena_alloc(2ul << _bsrl(a - 1), n);
  }
}

static size_t __arena_malloc_usable_size(const void *p) {
  __arena_check();
  __arena_check_pointer(p);
  if (__arena_is_arena_pointer(p)) {
    return __arena_get_size(p);
  } else {
    return __arena.malloc_usable_size(p);
  }
}

static void *__arena_realloc(void *p, size_t n) {
  char *q;
  size_t m, o, z;
  __arena_check();
  if (p) {
    __arena_check_pointer(p);
    if (__arena_is_arena_pointer(p)) {
      if (n) {
        if ((m = __arena_get_size(p)) >= n) {
          return p;
        } else if (n <= SIZE) {
          z = 2ul << _bsrl(n - 1);
          if (__arena.offset[__arena.depth] - m == (o = (intptr_t)p - BASE)) {
            if (UNLIKELY(o + z > __arena.size)) {
              if (o + z <= SIZE) {
                if (!__arena_grow(o, z)) {
                  return 0;
                }
              } else {
                enomem();
                return 0;
              }
            }
            __arena.offset[__arena.depth] = o + z;
            *(size_t *)((char *)p - sizeof(size_t)) = z;
            return p;
          } else if ((q = __arena_alloc(1ul << _bsfl((intptr_t)p), z))) {
            memmove(q, p, m);
            return q;
          } else {
            return 0;
          }
        } else {
          enomem();
          return 0;
        }
      } else {
        return 0;
      }
    } else {
      return __arena.realloc(p, n);
    }
  } else {
    if (n <= 16) {
      n = 16;
    } else {
      n = 2ul << _bsrl(n - 1);
    }
    return __arena_alloc(16, n);
  }
}

static void *__arena_realloc_in_place(void *p, size_t n) {
  char *q;
  size_t m, z;
  __arena_check();
  if (p) {
    __arena_check_pointer(p);
    if (__arena_is_arena_pointer(p)) {
      if (n) {
        if ((m = __arena_get_size(p)) >= n) {
          return p;
        } else {
          return 0;
        }
      } else {
        return 0;
      }
    } else {
      return __arena.realloc_in_place(p, n);
    }
  } else {
    return 0;
  }
}

static int __arena_malloc_trim(size_t n) {
  return 0;
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
  EXCHANGE(hook_malloc, __arena.malloc);
  EXCHANGE(hook_calloc, __arena.calloc);
  EXCHANGE(hook_realloc, __arena.realloc);
  EXCHANGE(hook_memalign, __arena.memalign);
  EXCHANGE(hook_bulk_free, __arena.bulk_free);
  EXCHANGE(hook_malloc_trim, __arena.malloc_trim);
  EXCHANGE(hook_realloc_in_place, __arena.realloc_in_place);
  EXCHANGE(hook_malloc_usable_size, __arena.malloc_usable_size);
}

static void __arena_destroy(void) {
  if (__arena.depth) __arena_install();
  if (__arena.size) munmap(P(BASE), __arena.size);
  bzero(&__arena, sizeof(__arena));
}

static void __arena_init(void) {
  __arena.free = __arena_free;
  __arena.malloc = __arena_malloc;
  __arena.calloc = __arena_calloc;
  __arena.realloc = __arena_realloc;
  __arena.memalign = __arena_memalign;
  __arena.bulk_free = __arena_bulk_free;
  __arena.malloc_trim = __arena_malloc_trim;
  __arena.realloc_in_place = __arena_realloc_in_place;
  __arena.malloc_usable_size = __arena_malloc_usable_size;
  atexit(__arena_destroy);
}

/**
 * Pushes memory arena.
 *
 * This allocator gives a ~3x performance boost over dlmalloc, mostly
 * because it isn't thread safe and it doesn't do defragmentation.
 *
 * Calling this function will push a new arena. It may be called
 * multiple times from the main thread recursively. The first time it's
 * called, it hooks all the regular memory allocation functions. Any
 * allocations that were made previously outside the arena, will be
 * passed on to the previous hooks. Then, the basic idea, is rather than
 * bothering with free() you can just call __arena_pop() to bulk free.
 *
 * Arena allocations also have a slight size advantage, since 32-bit
 * pointers are always used. The maximum amount of arena memory is
 * 805,175,296 bytes.
 *
 * @see __arena_pop()
 */
void __arena_push(void) {
  if (UNLIKELY(!__arena.once)) {
    __arena_init();
    __arena.once = true;
  }
  if (!__arena.depth) {
    __arena_install();
  } else {
    _unassert(__arena.depth < ARRAYLEN(__arena.offset) - 1);
  }
  __arena.offset[__arena.depth + 1] = __arena.offset[__arena.depth];
  ++__arena.depth;
}

/**
 * Pops memory arena.
 *
 * This pops the most recently created arena, freeing all the memory
 * that was allocated between the push and pop arena calls. If this is
 * the last arena on the stack, then the old malloc hooks are restored.
 *
 * @see __arena_push()
 */
void __arena_pop(void) {
  size_t a, b, greed;
  __arena_check();
  if (!--__arena.depth) __arena_install();
  a = __arena.offset[__arena.depth];
  b = __arena.offset[__arena.depth + 1];
  greed = a;
  greed += FRAMESIZE;
  greed <<= 1;
  if (__arena.size > greed) {
    munmap(P(BASE + greed), __arena.size - greed);
    __arena.size = greed;
    b = MIN(b, greed);
    a = MIN(b, a);
  }
  bzero(P(BASE + a), b - a);
}
