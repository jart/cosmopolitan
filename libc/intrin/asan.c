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
#include "libc/alg/reverse.internal.h"
#include "libc/assert.h"
#include "libc/bits/likely.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/lockcmpxchg.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/spinlock.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/hook/hook.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "third_party/dlmalloc/dlmalloc.h"

STATIC_YOINK("_init_asan");

#if IsModeDbg()
// MODE=dbg
// O(32mb) of morgue memory
// Θ(64) bytes of malloc overhead
#define ASAN_MORGUE_ITEMS     512
#define ASAN_MORGUE_THRESHOLD 65536
#define ASAN_TRACE_ITEMS      16
#else
// MODE=asan
// O(32mb) of morgue memory
// Θ(32) bytes of malloc overhead
#define ASAN_MORGUE_ITEMS     512
#define ASAN_MORGUE_THRESHOLD 65536
#define ASAN_TRACE_ITEMS      4
#endif

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

#define HOOK(HOOK, IMPL)    \
  do {                      \
    if (weaken(HOOK)) {     \
      *weaken(HOOK) = IMPL; \
    }                       \
  } while (0)

#define REQUIRE(FUNC)                           \
  do {                                          \
    if (!weaken(FUNC)) {                        \
      kprintf("error: asan needs %s\n", #FUNC); \
      __asan_die()();                           \
      __asan_unreachable();                     \
    }                                           \
  } while (0)

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

struct AsanTrace {
  uint32_t p[ASAN_TRACE_ITEMS];  // assumes linkage into 32-bit space
};

struct AsanExtra {
  uint64_t size;
  struct AsanTrace bt;
};

struct AsanSourceLocation {
  const char *filename;
  int line;
  int column;
};

struct AsanAccessInfo {
  const char *addr;
  const uintptr_t first_bad_addr;
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
  void *p[ASAN_MORGUE_ITEMS];
};

struct ReportOriginHeap {
  const unsigned char *a;
  int z;
};

static int __asan_noreentry;
static pthread_mutex_t __asan_lock;
static struct AsanMorgue __asan_morgue;

#define __asan_unreachable()   \
  do {                         \
    for (;;) __builtin_trap(); \
  } while (0)

static int __asan_bsf(uint64_t x) {
  _Static_assert(sizeof(long long) == sizeof(uint64_t), "");
  return __builtin_ctzll(x);
}

static int __asan_bsr(uint64_t x) {
  _Static_assert(sizeof(long long) == sizeof(uint64_t), "");
  return __builtin_clzll(x) ^ 63;
}

static uint64_t __asan_roundup2pow(uint64_t x) {
  return 2ull << __asan_bsr(x - 1);
}

static char *__asan_utf8cpy(char *p, unsigned c) {
  uint64_t z;
  z = tpenc(c);
  do *p++ = z;
  while ((z >>= 8));
  return p;
}

static char *__asan_stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

static void __asan_memset(void *p, char c, size_t n) {
  char *b;
  size_t i;
  uint64_t x;
  b = p;
  x = 0x0101010101010101ul * (c & 255);
  switch (n) {
    case 0:
      break;
    case 1:
      __builtin_memcpy(b, &x, 1);
      break;
    case 2:
      __builtin_memcpy(b, &x, 2);
      break;
    case 3:
      __builtin_memcpy(b, &x, 2);
      __builtin_memcpy(b + 1, &x, 2);
      break;
    case 4:
      __builtin_memcpy(b, &x, 4);
      break;
    case 5:
    case 6:
    case 7:
      __builtin_memcpy(b, &x, 4);
      __builtin_memcpy(b + n - 4, &x, 4);
      break;
    case 8:
      __builtin_memcpy(b, &x, 8);
      break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      __builtin_memcpy(b, &x, 8);
      __builtin_memcpy(b + n - 8, &x, 8);
      break;
    default:
      if (n <= 64) {
        i = 0;
        do {
          __builtin_memcpy(b + i, &x, 8);
          asm volatile("" ::: "memory");
          __builtin_memcpy(b + i + 8, &x, 8);
        } while ((i += 16) + 16 <= n);
        for (; i < n; ++i) b[i] = x;
      } else {
        __repstosb(p, c, n);
      }
      break;
  }
}

static void *__asan_mempcpy(void *dst, const void *src, size_t n) {
  size_t i;
  char *d;
  const char *s;
  uint64_t a, b;
  d = dst;
  s = src;
  switch (n) {
    case 0:
      return d;
    case 1:
      *d = *s;
      return d + 1;
    case 2:
      __builtin_memcpy(&a, s, 2);
      __builtin_memcpy(d, &a, 2);
      return d + 2;
    case 3:
      __builtin_memcpy(&a, s, 2);
      __builtin_memcpy(&b, s + 1, 2);
      __builtin_memcpy(d, &a, 2);
      __builtin_memcpy(d + 1, &b, 2);
      return d + 3;
    case 4:
      __builtin_memcpy(&a, s, 4);
      __builtin_memcpy(d, &a, 4);
      return d + 4;
    case 5:
    case 6:
    case 7:
      __builtin_memcpy(&a, s, 4);
      __builtin_memcpy(&b, s + n - 4, 4);
      __builtin_memcpy(d, &a, 4);
      __builtin_memcpy(d + n - 4, &b, 4);
      return d + n;
    case 8:
      __builtin_memcpy(&a, s, 8);
      __builtin_memcpy(d, &a, 8);
      return d + 8;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      __builtin_memcpy(&a, s, 8);
      __builtin_memcpy(&b, s + n - 8, 8);
      __builtin_memcpy(d, &a, 8);
      __builtin_memcpy(d + n - 8, &b, 8);
      return d + n;
    default:
      if (n <= 64) {
        i = 0;
        do {
          __builtin_memcpy(&a, s + i, 8);
          asm volatile("" ::: "memory");
          __builtin_memcpy(d + i, &a, 8);
        } while ((i += 8) + 8 <= n);
        for (; i < n; ++i) d[i] = s[i];
        return d + i;
      } else {
        return __repmovsb(d, s, n);
      }
  }
}

static void *__asan_memcpy(void *dst, const void *src, size_t n) {
  __asan_mempcpy(dst, src, n);
  return dst;
}

static char *__asan_hexcpy(char *p, uint64_t x, uint8_t k) {
  while (k) *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  return p;
}

static void __asan_exit(void) {
  kprintf("your asan runtime needs\n"
          "\tSTATIC_YOINK(\"__die\");\n"
          "in order to show you backtraces\n");
  __restorewintty();
  _Exit(99);
}

dontdiscard static __asan_die_f *__asan_die(void) {
  if (weaken(__die)) {
    return weaken(__die);
  } else {
    return __asan_exit;
  }
}

void __asan_poison(void *p, long n, signed char t) {
  signed char k, *s;
  s = (signed char *)(((intptr_t)p >> 3) + 0x7fff8000);
  if ((k = (intptr_t)p & 7)) {
    if ((!*s && n >= 8 - k) || *s > k) *s = k;
    n -= MIN(8 - k, n);
    s += 1;
  }
  __asan_memset(s, t, n >> 3);
  if ((k = n & 7)) {
    s += n >> 3;
    if (*s < 0 || (*s > 0 && *s <= k)) *s = t;
  }
}

void __asan_unpoison(void *p, long n) {
  signed char k, *s;
  k = (intptr_t)p & 7;
  s = (signed char *)(((intptr_t)p >> 3) + 0x7fff8000);
  if (UNLIKELY(k)) {
    if (k + n < 8) {
      if (n > 0) *s = MAX(*s, k + n);
      return;
    }
    n -= MIN(8 - k, n);
    *s++ = 0;
  }
  __asan_memset(s, 0, n >> 3);
  if ((k = n & 7)) {
    s += n >> 3;
    if (*s < 0) *s = k;
    if (*s > 0) *s = MAX(*s, k);
  }
}

static bool __asan_is_mapped(int x) {
  // xxx: we can't lock because no reentrant locks yet
  int i;
  bool res;
  struct MemoryIntervals *m;
  __mmi_lock();
  m = weaken(_mmi);
  i = FindMemoryInterval(m, x);
  res = i < m->i && x >= m->p[i].x;
  __mmi_unlock();
  return res;
}

static bool __asan_is_image(const unsigned char *p) {
  return _base <= p && p < _end;
}

static bool __asan_exists(const void *x) {
  return !kisdangerous(x);
}

static struct AsanFault __asan_fault(const signed char *s, signed char dflt) {
  struct AsanFault r;
  if (s[0] < 0) {
    r.kind = s[0];
  } else if (((uintptr_t)(s + 1) & (PAGESIZE - 1)) && s[1] < 0) {
    r.kind = s[1];
  } else {
    r.kind = dflt;
  }
  r.shadow = s;
  return r;
}

static struct AsanFault __asan_checka(const signed char *s, long ndiv8) {
  intptr_t a;
  uint64_t w;
  const signed char *e = s + ndiv8;
  for (; ((intptr_t)s & 7) && s < e; ++s) {
    if (*s) return __asan_fault(s - 1, kAsanHeapOverrun);
  }
  for (; s + 8 <= e; s += 8) {
    if (UNLIKELY(!((a = (intptr_t)s) & 0xffff))) {
      if (kisdangerous((void *)a)) {
        return (struct AsanFault){kAsanUnmapped, s};
      }
    }
    if ((w = ((uint64_t)(255 & s[0]) << 000 | (uint64_t)(255 & s[1]) << 010 |
              (uint64_t)(255 & s[2]) << 020 | (uint64_t)(255 & s[3]) << 030 |
              (uint64_t)(255 & s[4]) << 040 | (uint64_t)(255 & s[5]) << 050 |
              (uint64_t)(255 & s[6]) << 060 | (uint64_t)(255 & s[7]) << 070))) {
      s += __asan_bsf(w) >> 3;
      return __asan_fault(s, kAsanHeapOverrun);
    }
  }
  for (; s < e; ++s) {
    if (*s) return __asan_fault(s - 1, kAsanHeapOverrun);
  }
  return (struct AsanFault){0};
}

/**
 * Checks validity of memory range.
 *
 * Normally this is abstracted by the compiler.
 *
 * @param p is starting virtual address
 * @param n is number of bytes to check
 * @return kind is 0 on success or <0 on invalid
 * @return shadow points to first poisoned shadow byte
 * @note this takes 6 picoseconds per byte
 */
struct AsanFault __asan_check(const void *p, long n) {
  intptr_t a;
  struct AsanFault f;
  signed char c, k, *s;
  if (n > 0) {
    k = (intptr_t)p & 7;
    a = ((intptr_t)p >> 3) + 0x7fff8000;
    s = (signed char *)a;
    if (OverlapsShadowSpace(p, n)) {
      return (struct AsanFault){kAsanProtected, s};
    } else if (kisdangerous((void *)a)) {
      return (struct AsanFault){kAsanUnmapped, s};
    }
    if (UNLIKELY(k)) {
      if (!(c = *s)) {
        n -= MIN(8 - k, n);
        s += 1;
      } else if (c > 0 && n < 8 && c >= k + n) {
        return (struct AsanFault){0};
      } else {
        return __asan_fault(s, kAsanHeapOverrun);
      }
    }
    k = n & 7;
    n >>= 3;
    if ((f = __asan_checka(s, n)).kind) {
      return f;
    } else if (!k || !(c = s[n]) || k <= c) {
      return (struct AsanFault){0};
    } else {
      return __asan_fault(s, kAsanHeapOverrun);
    }
  } else if (!n) {
    return (struct AsanFault){0};
  } else {
    return (struct AsanFault){kAsanNullPage, 0};
  }
}

bool __asan_is_valid(const void *p, long n) {
  struct AsanFault f;
  f = __asan_check(p, n);
  return !f.kind;
}

bool __asan_is_valid_iov(const struct iovec *iov, int iovlen) {
  int i;
  size_t size;
  if (iovlen >= 0 &&
      !__builtin_mul_overflow(iovlen, sizeof(struct iovec), &size) &&
      __asan_is_valid(iov, size)) {
    for (i = 0; i < iovlen; ++i) {
      if (!__asan_is_valid(iov[i].iov_base, iov[i].iov_len)) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

bool __asan_is_valid_strlist(char *const *p) {
  for (;; ++p) {
    if (!__asan_is_valid(p, sizeof(char *))) return false;
    if (!*p) return true;
    if (!__asan_is_valid(*p, 1)) return false;
  }
}

wint_t __asan_symbolize_access_poison(signed char kind) {
  switch (kind) {
    case kAsanNullPage:
      return L'∅';
    case kAsanProtected:
      return L'P';
    case kAsanHeapFree:
      return L'F';
    case kAsanHeapRelocated:
      return L'R';
    case kAsanAllocaOverrun:
      return L'𝑂';
    case kAsanHeapUnderrun:
      return L'U';
    case kAsanHeapOverrun:
      return L'O';
    case kAsanStackUnscoped:
      return L's';
    case kAsanStackOverflow:
      return L'!';
    case kAsanGlobalOrder:
      return L'I';
    case kAsanStackFree:
      return L'r';
    case kAsanStackPartial:
      return L'p';
    case kAsanStackOverrun:
      return L'o';
    case kAsanStackMiddle:
      return L'm';
    case kAsanStackUnderrun:
      return L'u';
    case kAsanAllocaUnderrun:
      return L'𝑈';
    case kAsanUnmapped:
      return L'M';
    case kAsanGlobalRedzone:
      return L'G';
    case kAsanGlobalGone:
      return L'𝐺';
    case kAsanGlobalUnderrun:
      return L'μ';
    case kAsanGlobalOverrun:
      return L'Ω';
    default:
      return L'?';
  }
}

const char *__asan_describe_access_poison(signed char kind) {
  switch (kind) {
    case kAsanNullPage:
      return "null pointer dereference";
    case kAsanProtected:
      return "protected";
    case kAsanHeapFree:
      return "heap use after free";
    case kAsanHeapRelocated:
      return "heap use after relocate";
    case kAsanAllocaOverrun:
      return "alloca overflow";
    case kAsanHeapUnderrun:
      return "heap underrun";
    case kAsanHeapOverrun:
      return "heap overrun";
    case kAsanStackUnscoped:
      return "stack use after scope";
    case kAsanStackOverflow:
      return "stack overflow";
    case kAsanGlobalOrder:
      return "global init order";
    case kAsanStackFree:
      return "stack use after return";
    case kAsanStackPartial:
      return "stack partial";
    case kAsanStackOverrun:
      return "stack overrun";
    case kAsanStackMiddle:
      return "stack middle";
    case kAsanStackUnderrun:
      return "stack underflow";
    case kAsanAllocaUnderrun:
      return "alloca underflow";
    case kAsanUnmapped:
      return "unmapped";
    case kAsanGlobalRedzone:
      return "global redzone";
    case kAsanGlobalGone:
      return "global gone";
    case kAsanGlobalUnderrun:
      return "global underrun";
    case kAsanGlobalOverrun:
      return "global overrun";
    default:
      return "poisoned";
  }
}

dontdiscard static __asan_die_f *__asan_report_invalid_pointer(
    const void *addr) {
  kprintf("\n\e[J\e[1;31masan error\e[0m: this corruption at %p shadow %p\n",
          addr, SHADOW(addr));
  return __asan_die();
}

static char *__asan_format_interval(char *p, intptr_t a, intptr_t b) {
  p = __asan_hexcpy(p, a, 48), *p++ = '-';
  p = __asan_hexcpy(p, b, 48);
  return p;
}

static char *__asan_format_section(char *p, const void *p1, const void *p2,
                                   const char *name, const void *addr) {
  intptr_t a, b;
  if ((a = (intptr_t)p1) < (b = (intptr_t)p2)) {
    p = __asan_format_interval(p, a, b), *p++ = ' ';
    p = __asan_stpcpy(p, name);
    if (a <= (intptr_t)addr && (intptr_t)addr <= b) {
      p = __asan_stpcpy(p, " ←address");
    }
    *p++ = '\n';
  }
  return p;
}

static void __asan_report_memory_origin_image(intptr_t a, int z) {
  unsigned l, m, r, n, k;
  struct SymbolTable *st;
  kprintf("\nthe memory belongs to image symbols\n");
  if (weaken(GetSymbolTable)) {
    if ((st = weaken(GetSymbolTable)())) {
      l = 0;
      r = n = st->count;
      k = a - st->addr_base;
      while (l < r) {
        m = (l + r) >> 1;
        if (st->symbols[m].y < k) {
          l = m + 1;
        } else {
          r = m;
        }
      }
      for (; l < n; ++l) {
        if ((st->symbols[l].x <= k && k <= st->symbols[l].y) ||
            (st->symbols[l].x <= k + z && k + z <= st->symbols[l].y) ||
            (k < st->symbols[l].x && st->symbols[l].y < k + z)) {
          kprintf("\t%s [%#x,%#x] size %'d\n", st->name_base + st->names[l],
                  st->addr_base + st->symbols[l].x,
                  st->addr_base + st->symbols[l].y,
                  st->symbols[l].y - st->symbols[l].x + 1);
        } else {
          break;
        }
      }
    } else {
      kprintf("\tunknown please supply .com.dbg symbols or set COMDBG\n");
    }
  } else {
    kprintf("\tunknown please STATIC_YOINK(\"GetSymbolTable\");\n");
  }
}

static noasan void OnMemory(void *x, void *y, size_t n, void *a) {
  const unsigned char *p = x;
  struct ReportOriginHeap *t = a;
  if ((p <= t->a && t->a < p + n) ||
      (p <= t->a + t->z && t->a + t->z < p + n) ||
      (t->a < p && p + n <= t->a + t->z)) {
    kprintf("%p %,lu bytes [dlmalloc]", x, n);
    __asan_print_trace(x);
    kprintf("\n");
  }
}

static void __asan_report_memory_origin_heap(const unsigned char *a, int z) {
  struct ReportOriginHeap t;
  kprintf("\nthe memory was allocated by\n");
  if (weaken(malloc_inspect_all)) {
    t.a = a;
    t.z = z;
    weaken(malloc_inspect_all)(OnMemory, &t);
  } else {
    kprintf("\tunknown please STATIC_YOINK(\"malloc_inspect_all\");\n");
  }
}

static void __asan_report_memory_origin(const unsigned char *addr, int size,
                                        signed char kind) {
  switch (kind) {
    case kAsanStackOverrun:
    case kAsanGlobalOverrun:
    case kAsanAllocaOverrun:
    case kAsanHeapOverrun:
      addr -= 1;
      size += 1;
      break;
    case kAsanHeapUnderrun:
    case kAsanStackUnderrun:
    case kAsanAllocaUnderrun:
    case kAsanGlobalUnderrun:
      size += 1;
      break;
    case kAsanGlobalRedzone:
      addr -= 1;
      size += 2;
      break;
    default:
      break;
  }
  if (_base <= addr && addr < _end) {
    __asan_report_memory_origin_image((intptr_t)addr, size);
  } else if (IsAutoFrame((intptr_t)addr >> 16)) {
    __asan_report_memory_origin_heap(addr, size);
  }
}

dontdiscard static __asan_die_f *__asan_report(const void *addr, int size,
                                               const char *message,
                                               signed char kind) {
  int i;
  wint_t c;
  signed char t;
  uint64_t x, y, z;
  char *p, *q, *base;
  struct MemoryIntervals *m;
  --__ftrace;
  p = __fatalbuf;
  kprintf("\n\e[J\e[1;31masan error\e[0m: %s %d-byte %s at %p shadow %p\n",
          __asan_describe_access_poison(kind), size, message, addr,
          SHADOW(addr));
  if (0 < size && size < 80) {
    base = (char *)addr - ((80 >> 1) - (size >> 1));
    for (i = 0; i < 80; ++i) {
      if ((char *)addr <= base + i && base + i < (char *)addr + size) {
        if (__asan_is_valid(base + i, 1)) {
          *p++ = '*';
        } else {
          *p++ = 'x';
        }
      } else {
        *p++ = ' ';
      }
    }
    *p++ = '\n';
    for (c = i = 0; i < 80; ++i) {
      if (!(t = __asan_check(base + i, 1).kind)) {
        if (c != 32) {
          *p++ = '\e', *p++ = '[', *p++ = '3', *p++ = '2', *p++ = 'm';
          c = 32;
        }
        *p++ = '.';
      } else {
        if (c != 31) {
          *p++ = '\e', *p++ = '[', *p++ = '3', *p++ = '1', *p++ = 'm';
          c = 31;
        }
        p = __asan_utf8cpy(p, __asan_symbolize_access_poison(t));
      }
    }
    *p++ = '\e', *p++ = '[', *p++ = '3', *p++ = '9', *p++ = 'm';
    *p++ = '\n';
    for (i = 0; (intptr_t)(base + i) & 7; ++i) *p++ = ' ';
    for (; i + 8 <= 80; i += 8) {
      q = p + 8;
      *p++ = '|';
      z = ((intptr_t)(base + i) >> 3) + 0x7fff8000;
      if (!kisdangerous((void *)z)) {
        p = __intcpy(p, *(signed char *)z);
      } else {
        *p++ = '!';
      }
      while (p < q) {
        *p++ = ' ';
      }
    }
    for (; i < 80; ++i) *p++ = ' ';
    *p++ = '\n';
    for (i = 0; i < 80; ++i) {
      p = __asan_utf8cpy(p, __asan_exists(base + i)
                                ? kCp437[((unsigned char *)base)[i]]
                                : L'⋅');
    }
    *p++ = '\n';
  }
  p = __asan_format_section(p, _base, _etext, ".text", addr);
  p = __asan_format_section(p, _etext, _edata, ".data", addr);
  p = __asan_format_section(p, _end, _edata, ".bss", addr);
  __mmi_lock();
  for (m = weaken(_mmi), i = 0; i < m->i; ++i) {
    x = m->p[i].x;
    y = m->p[i].y;
    p = __asan_format_interval(p, x << 16, (y << 16) + (FRAMESIZE - 1));
    z = (intptr_t)addr >> 16;
    if (x <= z && z <= y) p = __asan_stpcpy(p, " ←address");
    z = (((intptr_t)addr >> 3) + 0x7fff8000) >> 16;
    if (x <= z && z <= y) p = __asan_stpcpy(p, " ←shadow");
    *p++ = '\n';
  }
  __mmi_unlock();
  *p = 0;
  kprintf("%s", __fatalbuf);
  __asan_report_memory_origin(addr, size, kind);
  kprintf("\nthe crash was caused by\n");
  ++__ftrace;
  return __asan_die();
}

void __asan_verify(const void *p, size_t n) {
  const char *q;
  struct AsanFault f;
  if (!(f = __asan_check(p, n)).kind) return;
  q = UNSHADOW(f.shadow);
  if ((uintptr_t)q != ((uintptr_t)p & -8) && (uintptr_t)q - (uintptr_t)p < n) {
    n -= (uintptr_t)q - (uintptr_t)p;
    p = q;
  }
  __asan_report(p, n, "verify", f.kind)();
  __asan_unreachable();
}

dontdiscard __asan_die_f *__asan_report_memory_fault(void *addr, int size,
                                                     const char *message) {
  return __asan_report(addr, size, message,
                       __asan_fault(SHADOW(addr), -128).kind);
}

void *__asan_morgue_add(void *p) {
  int i;
  void *r;
  if (__threaded) pthread_mutex_lock(&__asan_lock);
  i = __asan_morgue.i++ & (ARRAYLEN(__asan_morgue.p) - 1);
  r = __asan_morgue.p[i];
  __asan_morgue.p[i] = p;
  if (__threaded) pthread_mutex_unlock(&__asan_lock);
  return r;
}

static void __asan_morgue_flush(void) {
  int i;
  void *p;
  if (__threaded) pthread_mutex_lock(&__asan_lock);
  for (i = 0; i < ARRAYLEN(__asan_morgue.p); ++i) {
    if (__asan_morgue.p[i] && weaken(dlfree)) {
      weaken(dlfree)(__asan_morgue.p[i]);
    }
    __asan_morgue.p[i] = 0;
  }
  if (__threaded) pthread_mutex_unlock(&__asan_lock);
}

static size_t __asan_user_size(size_t n) {
  if (n) {
    return n;
  } else {
    return 1;
  }
}

static size_t __asan_heap_size(size_t n) {
  if (n < 0x7fffffff0000) {
    n = ROUNDUP(n, alignof(struct AsanExtra));
    return __asan_roundup2pow(n + sizeof(struct AsanExtra));
  } else {
    return -1;
  }
}

static void __asan_write48(uint64_t *value, uint64_t x) {
  uint64_t cookie;
  cookie = 'J' | 'T' << 8;
  cookie ^= x & 0xffff;
  *value = (x & 0xffffffffffff) | cookie << 48;
}

static bool __asan_read48(uint64_t value, uint64_t *x) {
  uint64_t cookie;
  cookie = value >> 48;
  cookie ^= value & 0xffff;
  *x = (int64_t)(value << 16) >> 16;
  return cookie == ('J' | 'T' << 8);
}

static void __asan_rawtrace(struct AsanTrace *bt, const struct StackFrame *bp) {
  size_t i;
  for (i = 0; bp && i < ARRAYLEN(bt->p); ++i, bp = bp->next) {
    if (kisdangerous(bp)) break;
    bt->p[i] = bp->addr;
  }
  for (; i < ARRAYLEN(bt->p); ++i) {
    bt->p[i] = 0;
  }
}

static void __asan_trace(struct AsanTrace *bt, const struct StackFrame *bp) {
  int f1, f2;
  size_t i, gi;
  intptr_t addr;
  struct Garbages *garbage;
  garbage = weaken(__garbage);
  gi = garbage ? garbage->i : 0;
  for (f1 = -1, i = 0; bp && i < ARRAYLEN(bt->p); ++i, bp = bp->next) {
    if (f1 != (f2 = ((intptr_t)bp >> 16))) {
      if (kisdangerous(bp)) break;
      f1 = f2;
    }
    if (!__asan_checka(SHADOW(bp), sizeof(*bp) >> 3).kind) {
      addr = bp->addr;
      if (addr == weakaddr("__gc") && weakaddr("__gc")) {
        do --gi;
        while ((addr = garbage->p[gi].ret) == weakaddr("__gc"));
      }
      bt->p[i] = addr;
    } else {
      break;
    }
  }
  for (; i < ARRAYLEN(bt->p); ++i) {
    bt->p[i] = 0;
  }
}

#define __asan_trace __asan_rawtrace

void *__asan_allocate(size_t a, size_t n, int underrun, int overrun,
                      struct AsanTrace *bt) {
  char *p;
  size_t c;
  struct AsanExtra *e;
  n = __asan_user_size(n);
  if ((p = weaken(dlmemalign)(a, __asan_heap_size(n)))) {
    c = weaken(dlmalloc_usable_size)(p);
    e = (struct AsanExtra *)(p + c - sizeof(*e));
    __asan_unpoison(p, n);
    __asan_poison(p - 16, 16, underrun); /* see dlmalloc design */
    __asan_poison(p + n, c - n, overrun);
    __asan_memset(p, 0xF9, n);
    __asan_write48(&e->size, n);
    __asan_memcpy(&e->bt, bt, sizeof(*bt));
  }
  return p;
}

static struct AsanExtra *__asan_get_extra(const void *p, size_t *c) {
  int f;
  long x, n;
  struct AsanExtra *e;
  f = (intptr_t)p >> 16;
  if (!kisdangerous(p) && (n = weaken(dlmalloc_usable_size)(p)) > sizeof(*e) &&
      !__builtin_add_overflow((intptr_t)p, n, &x) && x <= 0x800000000000 &&
      (LIKELY(f == (int)((x - 1) >> 16)) || !kisdangerous((void *)(x - 1))) &&
      (LIKELY(f == (int)((x = x - sizeof(*e)) >> 16)) ||
       __asan_is_mapped(x >> 16)) &&
      !(x & (alignof(struct AsanExtra) - 1))) {
    *c = n;
    return (struct AsanExtra *)x;
  } else {
    return 0;
  }
}

size_t __asan_get_heap_size(const void *p) {
  size_t n, c;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c))) {
    if (__asan_read48(e->size, &n)) {
      return n;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

static size_t __asan_malloc_usable_size(const void *p) {
  size_t n, c;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c))) {
    if (__asan_read48(e->size, &n)) {
      return n;
    } else {
      __asan_report_invalid_pointer(p)();
      __asan_unreachable();
    }
  } else {
    __asan_report_invalid_pointer(p)();
    __asan_unreachable();
  }
}

int __asan_print_trace(void *p) {
  size_t c, i, n;
  struct AsanExtra *e;
  if (!(e = __asan_get_extra(p, &c))) {
    kprintf(" bad pointer");
    return einval();
  }
  if (!__asan_read48(e->size, &n)) {
    kprintf(" bad cookie");
    return -1;
  }
  kprintf("\n%p %,lu bytes [asan]", (char *)p, n);
  if (!__asan_is_mapped((((intptr_t)p >> 3) + 0x7fff8000) >> 16)) {
    kprintf(" (shadow not mapped?!)");
  }
  for (i = 0; i < ARRAYLEN(e->bt.p) && e->bt.p[i]; ++i) {
    kprintf("\n%*lx %s", 12, e->bt.p[i],
            weaken(__get_symbol_by_addr)
                ? weaken(__get_symbol_by_addr)(e->bt.p[i])
                : "please STATIC_YOINK(\"__get_symbol_by_addr\")");
  }
  return 0;
}

static void __asan_deallocate(char *p, long kind) {
  size_t c, n;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c))) {
    if (__asan_read48(e->size, &n)) {
      __asan_poison(p, c, kind);
      if (c <= ASAN_MORGUE_THRESHOLD) {
        p = __asan_morgue_add(p);
      }
      weaken(dlfree)(p);
    } else {
      __asan_report_invalid_pointer(p)();
      __asan_unreachable();
    }
  } else {
    __asan_report_invalid_pointer(p)();
    __asan_unreachable();
  }
}

void __asan_free(void *p) {
  if (!p) return;
  __asan_deallocate(p, kAsanHeapFree);
}

size_t __asan_bulk_free(void *p[], size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (p[i]) {
      __asan_deallocate(p[i], kAsanHeapFree);
      p[i] = 0;
    }
  }
  return 0;
}

static void *__asan_realloc_nogrow(void *p, size_t n, size_t m,
                                   struct AsanTrace *bt) {
  return 0;
}

static void *__asan_realloc_grow(void *p, size_t n, size_t m,
                                 struct AsanTrace *bt) {
  char *q;
  if ((q = __asan_allocate(16, n, kAsanHeapUnderrun, kAsanHeapOverrun, bt))) {
    __asan_memcpy(q, p, m);
    __asan_deallocate(p, kAsanHeapRelocated);
  }
  return q;
}

static void *__asan_realloc_impl(void *p, size_t n,
                                 void *grow(void *, size_t, size_t,
                                            struct AsanTrace *)) {
  size_t c, m;
  struct AsanExtra *e;
  if ((e = __asan_get_extra(p, &c))) {
    if (__asan_read48(e->size, &m)) {
      if (n <= m) { /* shrink */
        __asan_poison((char *)p + n, m - n, kAsanHeapOverrun);
        __asan_write48(&e->size, n);
        return p;
      } else if (n <= c - sizeof(struct AsanExtra)) { /* small growth */
        __asan_unpoison((char *)p + m, n - m);
        __asan_write48(&e->size, n);
        return p;
      } else { /* exponential growth */
        return grow(p, n, m, &e->bt);
      }
    } else {
      __asan_report_invalid_pointer(p)();
      __asan_unreachable();
    }
  } else {
    __asan_report_invalid_pointer(p)();
    __asan_unreachable();
  }
}

void *__asan_malloc(size_t size) {
  struct AsanTrace bt;
  __asan_trace(&bt, __builtin_frame_address(0));
  return __asan_allocate(16, size, kAsanHeapUnderrun, kAsanHeapOverrun, &bt);
}

void *__asan_memalign(size_t align, size_t size) {
  struct AsanTrace bt;
  __asan_trace(&bt, __builtin_frame_address(0));
  return __asan_allocate(align, size, kAsanHeapUnderrun, kAsanHeapOverrun, &bt);
}

void *__asan_calloc(size_t n, size_t m) {
  char *p;
  struct AsanTrace bt;
  __asan_trace(&bt, __builtin_frame_address(0));
  if (__builtin_mul_overflow(n, m, &n)) n = -1;
  if ((p = __asan_allocate(16, n, kAsanHeapUnderrun, kAsanHeapOverrun, &bt))) {
    __asan_memset(p, 0, n);
  }
  return p;
}

void *__asan_realloc(void *p, size_t n) {
  struct AsanTrace bt;
  if (p) {
    if (n) {
      return __asan_realloc_impl(p, n, __asan_realloc_grow);
    } else {
      __asan_free(p);
      return 0;
    }
  } else {
    __asan_trace(&bt, __builtin_frame_address(0));
    return __asan_allocate(16, n, kAsanHeapUnderrun, kAsanHeapOverrun, &bt);
  }
}

void *__asan_realloc_in_place(void *p, size_t n) {
  if (p) {
    return __asan_realloc_impl(p, n, __asan_realloc_nogrow);
  } else {
    return 0;
  }
}

int __asan_malloc_trim(size_t pad) {
  __asan_morgue_flush();
  if (weaken(dlmalloc_trim)) {
    return weaken(dlmalloc_trim)(pad);
  } else {
    return 0;
  }
}

void *__asan_stack_malloc(size_t size, int classid) {
  struct AsanTrace bt;
  __asan_trace(&bt, __builtin_frame_address(0));
  return __asan_allocate(16, size, kAsanStackUnderrun, kAsanStackOverrun, &bt);
}

void __asan_stack_free(char *p, size_t size, int classid) {
  __asan_deallocate(p, kAsanStackFree);
}

void __asan_handle_no_return(void) {
  __asan_unpoison((void *)GetStackAddr(), GetStackSize());
}

void __asan_register_globals(struct AsanGlobal g[], int n) {
  int i;
  __asan_poison(g, sizeof(*g) * n, kAsanProtected);
  for (i = 0; i < n; ++i) {
    __asan_poison(g[i].addr + g[i].size, g[i].size_with_redzone - g[i].size,
                  kAsanGlobalRedzone);
    if (g[i].location) {
      __asan_poison(g[i].location, sizeof(*g[i].location), kAsanProtected);
    }
  }
}

void __asan_unregister_globals(struct AsanGlobal g[], int n) {
  int i;
  for (i = 0; i < n; ++i) {
    __asan_poison(g[i].addr, g[i].size, kAsanGlobalGone);
  }
}

void __asan_evil(uint8_t *addr, int size, const char *s) {
  struct AsanTrace tr;
  __asan_rawtrace(&tr, __builtin_frame_address(0));
  kprintf("WARNING: ASAN bad %d byte %s at %x bt %x %x %x\n", size, s, addr,
          tr.p[0], tr.p[1], tr.p[2], tr.p[3]);
}

void __asan_report_load(uint8_t *addr, int size) {
  __asan_evil(addr, size, "load");
  if (!__vforked && _lockcmpxchg(&__asan_noreentry, 0, 1)) {
    __asan_report_memory_fault(addr, size, "load")();
    __asan_unreachable();
  }
}

void __asan_report_store(uint8_t *addr, int size) {
  __asan_evil(addr, size, "store");
  if (!__vforked && _lockcmpxchg(&__asan_noreentry, 0, 1)) {
    __asan_report_memory_fault(addr, size, "store")();
    __asan_unreachable();
  }
}

void __asan_poison_stack_memory(char *addr, size_t size) {
  __asan_poison(addr, size, kAsanStackFree);
}

void __asan_unpoison_stack_memory(char *addr, size_t size) {
  __asan_unpoison(addr, size);
}

void __asan_alloca_poison(char *addr, uintptr_t size) {
  __asan_poison(addr - 32, 32, kAsanAllocaUnderrun);
  __asan_poison(addr + size, 32, kAsanAllocaOverrun);
}

void __asan_allocas_unpoison(uintptr_t x, uintptr_t y) {
  if (!x || x > y) return;
  __asan_memset((void *)((x >> 3) + 0x7fff8000), 0, (y - x) / 8);
}

void *__asan_addr_is_in_fake_stack(void *fakestack, void *addr, void **beg,
                                   void **end) {
  return 0;
}

void *__asan_get_current_fake_stack(void) {
  return 0;
}

void __sanitizer_annotate_contiguous_container(char *beg, char *end,
                                               char *old_mid, char *new_mid) {
  // the c++ stl uses this
  // TODO(jart): make me faster
  __asan_unpoison(beg, new_mid - beg);
  __asan_poison(new_mid, end - new_mid, kAsanHeapOverrun);
}

void __asan_before_dynamic_init(const char *module_name) {
}

void __asan_after_dynamic_init(void) {
}

void __asan_install_malloc_hooks(void) {
  HOOK(hook_free, __asan_free);
  HOOK(hook_malloc, __asan_malloc);
  HOOK(hook_calloc, __asan_calloc);
  HOOK(hook_realloc, __asan_realloc);
  HOOK(hook_memalign, __asan_memalign);
  HOOK(hook_bulk_free, __asan_bulk_free);
  HOOK(hook_malloc_trim, __asan_malloc_trim);
  HOOK(hook_realloc_in_place, __asan_realloc_in_place);
  HOOK(hook_malloc_usable_size, __asan_malloc_usable_size);
}

void __asan_map_shadow(uintptr_t p, size_t n) {
  // assume _mmi.lock is held
  void *addr;
  int i, a, b;
  size_t size;
  int prot, flag;
  struct DirectMap sm;
  struct MemoryIntervals *m;
  if (OverlapsShadowSpace((void *)p, n)) {
    kprintf("error: %p size %'zu overlaps shadow space\n", p, n);
    _Exit(1);
  }
  m = weaken(_mmi);
  a = (0x7fff8000 + (p >> 3)) >> 16;
  b = (0x7fff8000 + (p >> 3) + (n >> 3) + 0xffff) >> 16;
  for (; a <= b; a += i) {
    i = 1;
    if (__asan_is_mapped(a)) {
      continue;
    }
    for (; a + i <= b; ++i) {
      if (__asan_is_mapped(a + i)) {
        break;
      }
    }
    size = (size_t)i << 16;
    addr = (void *)(intptr_t)((int64_t)((uint64_t)a << 32) >> 16);
    prot = PROT_READ | PROT_WRITE;
    flag = MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS;
    sm = weaken(sys_mmap)(addr, size, prot, flag, -1, 0);
    if (sm.addr == MAP_FAILED ||
        weaken(TrackMemoryInterval)(m, a, a + i - 1, sm.maphandle,
                                    PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                                    false, false, 0, size) == -1) {
      kprintf("error: could not map asan shadow memory\n");
      __asan_die()();
      __asan_unreachable();
    }
    __repstosb((void *)(intptr_t)((int64_t)((uint64_t)a << 32) >> 16),
               kAsanUnmapped, size);
  }
  __asan_unpoison((char *)p, n);
}

static textstartup void __asan_shadow_string(char *s) {
  __asan_map_shadow((intptr_t)s, __strlen(s) + 1);
}

static textstartup void __asan_shadow_auxv(intptr_t *auxv) {
  size_t i;
  for (i = 0; auxv[i]; i += 2) {
    if (weaken(AT_RANDOM) && auxv[i] == *weaken(AT_RANDOM)) {
      __asan_map_shadow(auxv[i + 1], 16);
    } else if (weaken(AT_EXECFN) && auxv[i] == *weaken(AT_EXECFN)) {
      __asan_shadow_string((char *)auxv[i + 1]);
    } else if (weaken(AT_PLATFORM) && auxv[i] == *weaken(AT_PLATFORM)) {
      __asan_shadow_string((char *)auxv[i + 1]);
    }
  }
  __asan_map_shadow((uintptr_t)auxv, (i + 2) * sizeof(intptr_t));
}

static textstartup void __asan_shadow_string_list(char **list) {
  size_t i;
  for (i = 0; list[i]; ++i) {
    __asan_shadow_string(list[i]);
  }
  __asan_map_shadow((uintptr_t)list, (i + 1) * sizeof(char *));
}

static textstartup void __asan_shadow_mapping(struct MemoryIntervals *m,
                                              size_t i) {
  uintptr_t x, y;
  if (i < m->i) {
    x = m->p[i].x;
    y = m->p[i].y;
    __asan_shadow_mapping(m, i + 1);
    __asan_map_shadow(x << 16, (y - x + 1) << 16);
  }
}

static textstartup void __asan_shadow_existing_mappings(void) {
  __asan_shadow_mapping(&_mmi, 0);
  __asan_map_shadow(GetStackAddr(), GetStackSize());
  __asan_poison((void *)GetStackAddr(), PAGESIZE, kAsanStackOverflow);
}

textstartup void __asan_init(int argc, char **argv, char **envp,
                             intptr_t *auxv) {
  static bool once;
  if (!_lockcmpxchg(&once, false, true)) return;
  if (IsWindows() && NtGetVersion() < kNtVersionWindows10) {
    __write_str("error: asan binaries require windows10\r\n");
    __restorewintty();
    _Exit(0); /* So `make MODE=dbg test` passes w/ Windows7 */
  }
  REQUIRE(_mmi);
  REQUIRE(sys_mmap);
  REQUIRE(TrackMemoryInterval);
  if (weaken(hook_malloc) || weaken(hook_calloc) || weaken(hook_realloc) ||
      weaken(hook_realloc_in_place) || weaken(hook_free) ||
      weaken(hook_malloc_usable_size)) {
    REQUIRE(dlmemalign);
    REQUIRE(dlmalloc_usable_size);
  }
  __asan_shadow_existing_mappings();
  __asan_map_shadow((uintptr_t)_base, _end - _base);
  __asan_map_shadow(0, 4096);
  __asan_poison(0, PAGESIZE, kAsanNullPage);
  if (!IsWindows()) {
    __sysv_mprotect((void *)0x7fff8000, 0x10000, PROT_READ);
  }
  __asan_shadow_string_list(argv);
  __asan_shadow_string_list(envp);
  __asan_shadow_auxv(auxv);
  __asan_install_malloc_hooks();
  STRACE("    _    ____    _    _   _ ");
  STRACE("   / \\  / ___|  / \\  | \\ | |");
  STRACE("  / _ \\ \\___ \\ / _ \\ |  \\| |");
  STRACE(" / ___ \\ ___) / ___ \\| |\\  |");
  STRACE("/_/   \\_\\____/_/   \\_\\_| \\_|");
  STRACE("cosmopolitan memory safety module initialized");
}

static textstartup void __asan_ctor(void) {
  if (weaken(__cxa_atexit)) {
    weaken(__cxa_atexit)(__asan_morgue_flush, NULL, NULL);
  }
}

const void *const g_asan_ctor[] initarray = {
    __asan_ctor,
};
