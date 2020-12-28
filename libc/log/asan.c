/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/safemacros.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/fmt/itoa.h"
#include "libc/log/asan.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/log.h"
#include "libc/mem/hook/hook.h"
#include "libc/runtime/directmap.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

STATIC_YOINK("_init_asan");

/**
 * @fileoverview Cosmopolitan Address Sanitizer Runtime.
 *
 * Someone brilliant at Google figured out a way to improve upon memory
 * protection. Rather than invent another Java or Rust they changed GCC
 * so it can emit fast code, that checks the validity of each memory op
 * with byte granularity, by probing shadow memory.
 *
 * - AddressSanitizer dedicates one-eighth of the virtual address space
 *   to its shadow memory and uses a direct mapping with a scale and
 *   offset to translate an application address to its corresponding
 *   shadow address. Given the application memory address Addr, the
 *   address of the shadow byte is computed as (Addr>>3)+Offset."
 *
 * - We use the following encoding for each shadow byte: 0 means that
 *   all 8 bytes of the corresponding application memory region are
 *   addressable; k (1 ≤ k ≤ 7) means that the first k bytes are
 *   addressible; any negative value indicates that the entire 8-byte
 *   word is unaddressable. We use different negative values to
 *   distinguish between different kinds of unaddressable memory (heap
 *   redzones, stack redzones, global redzones, freed memory).
 *
 * Here's what the generated code looks like for 64-bit reads:
 *
 *     movq %addr,%tmp
 *     shrq $3,%tmp
 *     cmpb $0,0x7fff8000(%tmp)
 *     jnz  abort
 *     movq (%addr),%dst
 */

#define HOOK(HOOK, IMPL)  \
  if (weaken(HOOK)) {     \
    *weaken(HOOK) = IMPL; \
  }

struct AsanSourceLocation {
  const char *filename;
  int line;
  int column;
};

struct AsanAccessInfo {
  const char *addr;
  const char *first_bad_addr;
  size_t size;
  bool iswrite;
  unsigned long ip;
};

struct AsanGlobal {
  const char *addr;
  size_t size;
  size_t size_with_redzone;
  const void *name;
  const void *module_name;
  unsigned long has_cxx_init;
  struct AsanSourceLocation *location;
  char *odr_indicator;
};

struct AsanMorgue {
  unsigned i;
  void *p[16];
};

static struct AsanMorgue __asan_morgue;

static const char *__asan_dscribe_free_poison(int c) {
  switch (c) {
    case kAsanHeapFree:
      return "heap double free";
    case kAsanRelocated:
      return "free after relocate";
    case kAsanStackFree:
      return "stack double free";
    default:
      return "invalid pointer";
  }
}

static const char *__asan_describe_access_poison(int c) {
  switch (c) {
    case kAsanHeapFree:
      return "heap use after free";
    case kAsanStackFree:
      return "stack use after release";
    case kAsanRelocated:
      return "heap use after relocate";
    case kAsanHeapUnderrun:
      return "heap underrun";
    case kAsanHeapOverrun:
      return "heap overrun";
    case kAsanGlobalOverrun:
      return "global overrun";
    case kAsanGlobalUnregistered:
      return "global unregistered";
    case kAsanStackUnderrun:
      return "stack underflow";
    case kAsanStackOverrun:
      return "stack overflow";
    case kAsanAllocaOverrun:
      return "alloca overflow";
    case kAsanUnscoped:
      return "unscoped";
    default:
      return "poisoned";
  }
}

static wontreturn void __asan_die(const char *msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  __die();
}

static char *__asan_report_start(char *p) {
  bool ansi;
  const char *term;
  term = getenv("TERM");
  ansi = !term || strcmp(term, "dumb") != 0;
  if (ansi) p = stpcpy(p, "\r\e[J\e[1;91m");
  p = stpcpy(p, "asan error");
  if (ansi) p = stpcpy(p, "\e[0m");
  return stpcpy(p, ": ");
}

static wontreturn void __asan_report_deallocate_fault(void *addr, int c) {
  char *p, ibuf[21], buf[256];
  p = __asan_report_start(buf);
  p = stpcpy(p, __asan_dscribe_free_poison(c));
  p = stpcpy(p, " ");
  p = mempcpy(p, ibuf, int64toarray_radix10(c, ibuf));
  p = stpcpy(p, " at 0x");
  p = mempcpy(p, ibuf, uint64toarray_fixed16((intptr_t)addr, ibuf, 48));
  p = stpcpy(p, "\r\n");
  __asan_die(buf, p - buf);
}

static wontreturn void __asan_report_memory_fault(uint8_t *addr, int size,
                                                  const char *kind) {
  char *p, ibuf[21], buf[256];
  p = __asan_report_start(buf);
  p = stpcpy(p, __asan_describe_access_poison(*(char *)SHADOW((intptr_t)addr)));
  p = stpcpy(p, " ");
  p = mempcpy(p, ibuf, uint64toarray_radix10(size, ibuf));
  p = stpcpy(p, "-byte ");
  p = stpcpy(p, kind);
  p = stpcpy(p, " at 0x");
  p = mempcpy(p, ibuf, uint64toarray_fixed16((intptr_t)addr, ibuf, 48));
  p = stpcpy(p, "\r\n");
  __asan_die(buf, p - buf);
}

static const void *__asan_morgue_add(void *p) {
  void *r;
  r = __asan_morgue.p[__asan_morgue.i];
  __asan_morgue.p[__asan_morgue.i] = p;
  __asan_morgue.i += 1;
  __asan_morgue.i &= ARRAYLEN(__asan_morgue.p) - 1;
  return r;
}

static void __asan_morgue_flush(void) {
  void *p;
  unsigned i;
  for (i = 0; i < ARRAYLEN(__asan_morgue.p); ++i) {
    p = __asan_morgue.p[i];
    __asan_morgue.p[i] = NULL;
    dlfree(p);
  }
}

static void *__asan_allocate(size_t align, size_t size, int underrun,
                             int overrun) {
  char *p, *s;
  size_t q, r, i;
  if (!(p = dlmemalign(align, ROUNDUP(size, 8) + 16))) return NULL;
  s = (char *)SHADOW((intptr_t)p - 16);
  q = size / 8;
  r = size % 8;
  *s++ = underrun;
  *s++ = underrun;
  memset(s, 0, q);
  s += q;
  if (r) *s++ = r;
  *s++ = overrun;
  *s++ = overrun;
  return p;
}

static void __asan_deallocate(char *p, int kind) {
  char *s;
  s = (char *)SHADOW((intptr_t)p);
  if ((*s < 0 && *s != kAsanHeapOverrun) || *s >= 8) {
    __asan_report_deallocate_fault(p, *s);
  }
  memset(s, kind, dlmalloc_usable_size(p) >> 3);
  dlfree(__asan_morgue_add(p));
}

static void __asan_poison_redzone(intptr_t addr, size_t size, size_t redsize,
                                  int kind) {
  char *s;
  intptr_t p;
  size_t a, b, w;
  w = (intptr_t)addr & 7;
  p = (intptr_t)addr - w;
  a = w + size;
  b = w + redsize;
  s = (char *)SHADOW(p + a);
  if (a & 7) *s++ = a & 7;
  memset(s, kind, (b - ROUNDUP(a, 8)) >> 3);
}

static size_t __asan_malloc_usable_size(const void *vp) {
  char *s;
  size_t n;
  for (n = 0, s = (char *)SHADOW((intptr_t)vp);; ++s) {
    if (!*s) {
      n += 8;
    } else if (*s > 0) {
      n += *s & 7;
    } else {
      break;
    }
  }
  return n;
}

static void __asan_free(void *p) {
  if (!p) return;
  __asan_deallocate(p, kAsanHeapFree);
}

static void *__asan_memalign(size_t align, size_t size) {
  return __asan_allocate(align, size, kAsanHeapUnderrun, kAsanHeapOverrun);
}

static void *__asan_malloc(size_t size) {
  return __asan_memalign(16, size);
}

static void *__asan_calloc(size_t n, size_t m) {
  char *p;
  size_t size;
  if (__builtin_mul_overflow(n, m, &size)) size = -1;
  if ((p = __asan_malloc(size))) memset(p, 0, size);
  return p;
}

static void *__asan_realloc(void *p, size_t n) {
  char *p2;
  if (p) {
    if (n) {
      if ((p2 = __asan_malloc(n))) {
        memcpy(p2, p, min(n, dlmalloc_usable_size(p)));
        __asan_deallocate(p, kAsanRelocated);
      }
    } else {
      __asan_free(p);
      p2 = NULL;
    }
  } else {
    p2 = __asan_malloc(n);
  }
  return p2;
}

static void *__asan_valloc(size_t n) {
  return __asan_memalign(PAGESIZE, n);
}

static void *__asan_pvalloc(size_t n) {
  return __asan_valloc(ROUNDUP(n, PAGESIZE));
}

void __asan_register_globals(struct AsanGlobal g[], int n) {
  unsigned i;
  for (i = 0; i < n; ++i) {
    __asan_poison_redzone((intptr_t)g[i].addr, g[i].size,
                          g[i].size_with_redzone, kAsanGlobalOverrun);
  }
}

void __asan_unregister_globals(struct AsanGlobal g[], int n) {
  unsigned i;
  intptr_t a, b;
  for (i = 0; i < n; ++i) {
    a = ROUNDUP((intptr_t)g[i].addr, 8);
    b = ROUNDDOWN((intptr_t)g[i].addr + g[i].size_with_redzone, 8);
    if (b > a) {
      memset((char *)SHADOW(a), kAsanGlobalUnregistered, (b - a) >> 3);
    }
  }
}

void *__asan_stack_malloc(size_t size, int classid) {
  return __asan_allocate(32, size, kAsanStackUnderrun, kAsanStackOverrun);
}

void __asan_stack_free(char *p, size_t size, int classid) {
  dlfree(p);
}

void __asan_report_load_n(uint8_t *addr, int size) {
  __asan_report_memory_fault(addr, size, "load");
}

void __asan_report_store_n(uint8_t *addr, int size) {
  __asan_report_memory_fault(addr, size, "store");
}

void __asan_poison_stack_memory(uintptr_t p, size_t n) {
  memset((char *)SHADOW(p), kAsanUnscoped, n >> 3);
  if (n & 7) *(char *)SHADOW(p + n) = 8 - (n & 7);
}

void __asan_unpoison_stack_memory(uintptr_t p, size_t n) {
  memset((char *)SHADOW(p), 0, n >> 3);
  if (n & 7) *(char *)SHADOW(p + n) = n & 7;
}

void __asan_alloca_poison(intptr_t addr, size_t size) {
  __asan_poison_redzone(addr, size, size + 32, kAsanAllocaOverrun);
}

void __asan_allocas_unpoison(uintptr_t top, uintptr_t bottom) {
  memset((char *)SHADOW(top), 0, (bottom - top) >> 3);
}

void *__asan_addr_is_in_fake_stack(void *fakestack, void *addr, void **beg,
                                   void **end) {
  return NULL;
}

void *__asan_get_current_fake_stack(void) {
  return NULL;
}

void __asan_install_malloc_hooks(void) {
  HOOK(hook$free, __asan_free);
  HOOK(hook$malloc, __asan_malloc);
  HOOK(hook$calloc, __asan_calloc);
  HOOK(hook$valloc, __asan_valloc);
  HOOK(hook$pvalloc, __asan_pvalloc);
  HOOK(hook$realloc, __asan_realloc);
  HOOK(hook$memalign, __asan_memalign);
  HOOK(hook$malloc_usable_size, __asan_malloc_usable_size);
}

static bool __asan_is_mapped(int x) {
  int i = FindMemoryInterval(&_mmi, x);
  return i < _mmi.i && x >= _mmi.p[i].x && x <= _mmi.p[i].y;
}

void __asan_map_shadow(void *p, size_t n) {
  int i, x, a, b;
  struct DirectMap sm;
  a = SHADOW((uintptr_t)p) >> 16;
  b = ROUNDUP(SHADOW(ROUNDUP((uintptr_t)p + n, 8)), 1 << 16) >> 16;
  for (; a < b; ++a) {
    if (!__asan_is_mapped(a)) {
      sm = __mmap((void *)((uintptr_t)a << 16), 1 << 16, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
      if (sm.addr == MAP_FAILED ||
          TrackMemoryInterval(&_mmi, a, a, sm.maphandle, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED) == -1) {
        abort();
      }
    }
  }
}

static char *__asan_get_stack_base(void) {
  uintptr_t rsp;
  asm("mov\t%%rsp,%0" : "=r"(rsp));
  return (char *)ROUNDDOWN(ROUNDDOWN(rsp, STACKSIZE), FRAMESIZE);
}

static textstartup size_t __asan_get_auxv_size(intptr_t *auxv) {
  unsigned i;
  for (i = 0;; i += 2) {
    if (!auxv[i]) break;
  }
  return (i + 2) * sizeof(intptr_t);
}

static textstartup void __asan_shadow_string_list(char **list) {
  for (; *list; ++list) {
    __asan_map_shadow(*list, strlen(*list) + 1);
  }
}

textstartup void __asan_init(int argc, char **argv, char **envp,
                             intptr_t *auxv) {
  static bool once;
  if (once) return;
  __asan_map_shadow(_base, _end - _base);
  __asan_map_shadow(__asan_get_stack_base(), STACKSIZE);
  __asan_shadow_string_list(argv);
  __asan_shadow_string_list(envp);
  __asan_map_shadow(auxv, __asan_get_auxv_size(auxv));
  __asan_install_malloc_hooks();
}

static textstartup void __asan_ctor(void) {
  __cxa_atexit(__asan_morgue_flush, NULL, NULL);
}

const void *const g_asan_ctor[] initarray = {__asan_ctor};
