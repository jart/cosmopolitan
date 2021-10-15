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
#include "libc/bits/bits.h"
#include "libc/bits/likely.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/hook/hook.internal.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
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

#define HOOK(HOOK, IMPL)    \
  do {                      \
    if (weaken(HOOK)) {     \
      *weaken(HOOK) = IMPL; \
    }                       \
  } while (0)

#define REQUIRE(FUNC)                                \
  do {                                               \
    if (!weaken(FUNC)) {                             \
      __asan_die("error: asan needs " #FUNC "\n")(); \
      __asan_unreachable();                          \
    }                                                \
  } while (0)

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

struct AsanTrace {
  intptr_t p[4];
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
  const uintptr_t addr;
  const uintptr_t first_bad_addr;
  size_t size;
  bool iswrite;
  unsigned long ip;
};

struct AsanGlobal {
  const uintptr_t addr;
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
  void *p[32];
};

bool __asan_noreentry;
static struct AsanMorgue __asan_morgue;

static wontreturn void __asan_unreachable(void) {
  for (;;) __builtin_trap();
}

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

static void *__asan_memset(void *p, char c, size_t n) {
  char *b;
  size_t i;
  uint64_t x;
  b = p;
  x = 0x0101010101010101ul * (c & 255);
  switch (n) {
    case 0:
      return p;
    case 1:
      __builtin_memcpy(b, &x, 1);
      return p;
    case 2:
      __builtin_memcpy(b, &x, 2);
      return p;
    case 3:
      __builtin_memcpy(b, &x, 2);
      __builtin_memcpy(b + 1, &x, 2);
      return p;
    case 4:
      __builtin_memcpy(b, &x, 4);
      return p;
    case 5:
    case 6:
    case 7:
      __builtin_memcpy(b, &x, 4);
      __builtin_memcpy(b + n - 4, &x, 4);
      return p;
    case 8:
      __builtin_memcpy(b, &x, 8);
      return p;
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
      return p;
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
      return p;
  }
}

static void *__asan_mempcpy(void *dst, const void *src, size_t n) {
  size_t i;
  char *d, *s;
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
  __printf("your asan runtime needs\n"
           "\tSTATIC_YOINK(\"__die\");\n"
           "in order to show you backtraces\n");
  _Exit(99);
}

nodiscard static __asan_die_f *__asan_die(const char *msg) {
  __printf("%s", msg);
  if (weaken(__die)) {
    return weaken(__die);
  } else {
    return __asan_exit;
  }
}

void __asan_poison(long p, long n, signed char t) {
  signed char k, *s;
  s = (signed char *)((p >> 3) + 0x7fff8000);
  if ((k = p & 7)) {
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

void __asan_unpoison(long p, long n) {
  signed char k, *s;
  k = p & 7;
  s = (signed char *)((p >> 3) + 0x7fff8000);
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
  int i;
  struct MemoryIntervals *m;
  m = weaken(_mmi);
  i = FindMemoryInterval(m, x);
  return i < m->i && m->p[i].x <= x && x <= m->p[i].y;
}

static bool __asan_is_image(const unsigned char *p) {
  return _base <= p && p < _end;
}

static bool __asan_exists(const void *x) {
  return __asan_is_image(x) || __asan_is_mapped((intptr_t)x >> 16);
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
  signed char c, *e = s + ndiv8;
  for (; ((intptr_t)s & 7) && s < e; ++s) {
    if (*s) return __asan_fault(s - 1, kAsanHeapOverrun);
  }
  for (; s + 8 <= e; s += 8) {
    if (UNLIKELY(!((a = (intptr_t)s) & 0xffff))) {
      if (!__asan_is_mapped(a >> 16)) {
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
  uint64_t w;
  struct AsanFault f;
  signed char c, k, *s;
  if (n > 0) {
    k = (intptr_t)p & 7;
    a = ((intptr_t)p >> 3) + 0x7fff8000;
    s = (signed char *)a;
    if (OverlapsShadowSpace(p, n)) {
      return (struct AsanFault){kAsanProtected, s};
    } else if (IsLegalPointer(a) && !__asan_is_mapped(a >> 16)) {
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

static const char *__asan_dscribe_heap_poison(signed char c) {
  switch (c) {
    case kAsanHeapFree:
      return "heap double free";
    case kAsanStackFree:
      return "free stack after return";
    case kAsanHeapRelocated:
      return "free after relocate";
    default:
      return "this corruption";
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
    default:
      return "poisoned";
  }
}

nodiscard static __asan_die_f *__asan_report_invalid_pointer(void *addr) {
  __printf("\r\n%sasan error%s: this corruption at 0x%p shadow 0x%p\r\n",
           !g_isterminalinarticulate ? "\e[J\e[1;91m" : "",
           !g_isterminalinarticulate ? "\e[0m" : "", addr, SHADOW(addr));
  return __asan_die("");
}

static char *__asan_format_interval(char *p, intptr_t a, intptr_t b) {
  p = __asan_hexcpy(p, a, 48), *p++ = '-';
  p = __asan_hexcpy(p, b, 48);
  return p;
}

static char *__asan_format_section(char *p, void *p1, void *p2,
                                   const char *name, void *addr) {
  intptr_t a, b;
  if ((a = (intptr_t)p1) < (b = (intptr_t)p2)) {
    p = __asan_format_interval(p, a, b), *p++ = ' ';
    p = __stpcpy(p, name);
    if (a <= (intptr_t)addr && (intptr_t)addr <= b) {
      p = __stpcpy(p, " ←address");
    }
    *p++ = '\r', *p++ = '\n';
  }
  return p;
}

nodiscard static __asan_die_f *__asan_report(void *addr, int size,
                                             const char *message,
                                             signed char kind) {
  wint_t c;
  int i, cc;
  signed char t;
  uint64_t x, y, z;
  char *p, *q, *base;
  struct MemoryIntervals *m;
  p = __fatalbuf;
  __printf("\r\n%sasan error%s: %s %d-byte %s at 0x%p shadow 0x%p\r\n",
           !g_isterminalinarticulate ? "\e[J\e[1;91m" : "",
           !g_isterminalinarticulate ? "\e[0m" : "",
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
    *p++ = '\r', *p++ = '\n';
    for (c = i = 0; i < 80; ++i) {
      if (!(t = __asan_check(base + i, 1).kind)) {
        if (!g_isterminalinarticulate && c != 32) {
          p = __stpcpy(p, "\e[32m");
          c = 32;
        }
        *p++ = '.';
      } else {
        if (!g_isterminalinarticulate && c != 31) {
          p = __stpcpy(p, "\e[31m");
          c = 31;
        }
        p = __asan_utf8cpy(p, __asan_symbolize_access_poison(t));
      }
    }
    if (!g_isterminalinarticulate) p = __stpcpy(p, "\e[39m");
    *p++ = '\r', *p++ = '\n';
    for (i = 0; (intptr_t)(base + i) & 7; ++i) *p++ = ' ';
    for (; i + 8 <= 80; i += 8) {
      q = p + 8;
      *p++ = '|';
      z = ((intptr_t)(base + i) >> 3) + 0x7fff8000;
      if (__asan_is_mapped(z >> 16)) {
        p = __intcpy(p, *(signed char *)z);
      } else {
        *p++ = '!';
      }
      while (p < q) {
        *p++ = ' ';
      }
    }
    for (; i < 80; ++i) *p++ = ' ';
    *p++ = '\r', *p++ = '\n';
    for (i = 0; i < 80; ++i) {
      p = __asan_utf8cpy(p, __asan_exists(base + i)
                                ? kCp437[((unsigned char *)base)[i]]
                                : L'⋅');
    }
    *p++ = '\r', *p++ = '\n';
  }
  p = __asan_format_section(p, _base, _etext, ".text", addr);
  p = __asan_format_section(p, _etext, _edata, ".data", addr);
  p = __asan_format_section(p, _end, _edata, ".bss", addr);
  for (m = weaken(_mmi), i = 0; i < m->i; ++i) {
    x = m->p[i].x;
    y = m->p[i].y;
    p = __asan_format_interval(p, x << 16, (y << 16) + (FRAMESIZE - 1));
    z = (intptr_t)addr >> 16;
    if (x <= z && z <= y) p = __stpcpy(p, " ←address");
    z = (((intptr_t)addr >> 3) + 0x7fff8000) >> 16;
    if (x <= z && z <= y) p = __stpcpy(p, " ←shadow");
    *p++ = '\r', *p++ = '\n';
  }
  *p = 0;
  return __asan_die(__fatalbuf);
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

nodiscard __asan_die_f *__asan_report_memory_fault(void *addr, int size,
                                                   const char *message) {
  return __asan_report(addr, size, message,
                       __asan_fault(SHADOW(addr), -128).kind);
}

const void *__asan_morgue_add(void *p) {
  void *r;
  int i, j;
  for (;;) {
    i = __asan_morgue.i;
    j = (i + 1) & (ARRAYLEN(__asan_morgue.p) - 1);
    if (cmpxchg(&__asan_morgue.i, i, j)) {
      r = __asan_morgue.p[i];
      __asan_morgue.p[i] = p;
      return r;
    }
  }
}

static void __asan_morgue_flush(void) {
  int i;
  void *p;
  for (i = 0; i < ARRAYLEN(__asan_morgue.p); ++i) {
    p = __asan_morgue.p[i];
    if (cmpxchg(__asan_morgue.p + i, p, 0)) {
      if (weaken(dlfree)) {
        weaken(dlfree)(p);
      }
    }
  }
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

static void __asan_trace(struct AsanTrace *bt, const struct StackFrame *bp) {
  int f1, f2;
  size_t i, gi;
  intptr_t addr;
  struct Garbages *garbage;
  garbage = weaken(__garbage);
  gi = garbage ? garbage->i : 0;
  __asan_memset(bt, 0, sizeof(*bt));
  for (f1 = -1, i = 0; bp && i < ARRAYLEN(bt->p); ++i, bp = bp->next) {
    if (f1 != (f2 = ((intptr_t)bp >> 16))) {
      if (!__asan_is_mapped(f2)) break;
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
}

static void *__asan_allocate(size_t a, size_t n, int underrun, int overrun,
                             struct AsanTrace *bt) {
  char *p;
  size_t c;
  struct AsanExtra *e;
  n = __asan_user_size(n);
  if ((p = weaken(dlmemalign)(a, __asan_heap_size(n)))) {
    c = weaken(dlmalloc_usable_size)(p);
    e = (struct AsanExtra *)(p + c - sizeof(*e));
    __asan_unpoison((uintptr_t)p, n);
    __asan_poison((uintptr_t)p - 16, 16, underrun); /* see dlmalloc design */
    __asan_poison((uintptr_t)p + n, c - n, overrun);
    __asan_memset(p, 0xF9, n);
    __asan_write48(&e->size, n);
    __asan_memcpy(&e->bt, bt, sizeof(*bt));
  }
  return p;
}

static struct AsanExtra *__asan_get_extra(void *p, size_t *c) {
  int f;
  long x, n;
  struct AsanExtra *e;
  if ((0 < (intptr_t)p && (intptr_t)p < 0x800000000000) &&
      __asan_is_mapped((f = (intptr_t)p >> 16)) &&
      (LIKELY(f == (int)(((intptr_t)p - 16) >> 16)) ||
       __asan_is_mapped(((intptr_t)p - 16) >> 16)) &&
      (n = weaken(dlmalloc_usable_size)(p)) > sizeof(*e) &&
      !__builtin_add_overflow((intptr_t)p, n, &x) && x <= 0x800000000000 &&
      (LIKELY(f == (int)((x - 1) >> 16)) || __asan_is_mapped((x - 1) >> 16)) &&
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
  intptr_t x;
  size_t c, i, n;
  const char *name;
  struct AsanExtra *e;
  if (!(e = __asan_get_extra(p, &c))) {
    __printf(" bad pointer");
    return einval();
  }
  if (!__asan_read48(e->size, &n)) {
    __printf(" bad cookie");
    return -1;
  }
  __printf(" %,d used", n);
  if (!__asan_is_mapped((((intptr_t)p >> 3) + 0x7fff8000) >> 16)) {
    __printf(" (shadow not mapped?!)");
  }
  for (i = 0; i < ARRAYLEN(e->bt.p) && e->bt.p[i]; ++i) {
    __printf("\n%*x %s", 12, e->bt.p[i],
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
      __asan_poison((uintptr_t)p, c, kind);
      if (c <= FRAMESIZE) {
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
        __asan_poison((uintptr_t)p + n, m - n, kAsanHeapOverrun);
        __asan_write48(&e->size, n);
        return p;
      } else if (n <= c - sizeof(struct AsanExtra)) { /* small growth */
        __asan_unpoison((uintptr_t)p + m, n - m);
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
  uintptr_t stk, ssz;
  stk = (uintptr_t)__builtin_frame_address(0);
  ssz = GetStackSize();
  __asan_unpoison(stk, ROUNDUP(stk, ssz) - stk);
}

void __asan_register_globals(struct AsanGlobal g[], int n) {
  int i;
  __asan_poison((intptr_t)g, sizeof(*g) * n, kAsanProtected);
  for (i = 0; i < n; ++i) {
    __asan_poison(g[i].addr + g[i].size, g[i].size_with_redzone - g[i].size,
                  kAsanGlobalRedzone);
    if (g[i].location) {
      __asan_poison((intptr_t)g[i].location, sizeof(*g[i].location),
                    kAsanProtected);
    }
  }
}

void __asan_unregister_globals(struct AsanGlobal g[], int n) {
  int i;
  for (i = 0; i < n; ++i) {
    __asan_poison(g[i].addr, g[i].size, kAsanGlobalGone);
  }
}

void __asan_report_load(uint8_t *addr, int size) {
  if (cmpxchg(&__asan_noreentry, false, true)) {
    __asan_report_memory_fault(addr, size, "load")();
    __asan_unreachable();
  } else {
    __printf("WARNING: ASAN error reporting had an ASAN error\r\n");
  }
}

void __asan_report_store(uint8_t *addr, int size) {
  if (cmpxchg(&__asan_noreentry, false, true)) {
    __asan_report_memory_fault(addr, size, "store")();
    __asan_unreachable();
  } else {
    __printf("WARNING: ASAN error reporting had an ASAN error\r\n");
  }
}

void __asan_poison_stack_memory(uintptr_t addr, size_t size) {
  __asan_poison(addr, size, kAsanStackFree);
}

void __asan_unpoison_stack_memory(uintptr_t addr, size_t size) {
  __asan_unpoison(addr, size);
}

void __asan_alloca_poison(uintptr_t addr, size_t size) {
  /* TODO(jart): Make sense of this function. */
  /* __asan_poison(addr - 32, 32, kAsanAllocaUnderrun); */
  __asan_poison(ROUNDUP(addr + size, 32), 32, kAsanAllocaOverrun);
  __asan_unpoison(addr, ROUNDUP(addr + size, 32) - (addr + size) + 32 + size);
}

void __asan_allocas_unpoison(uintptr_t x, uintptr_t y) {
  if (x && x > y) __asan_unpoison(x, y - x);
}

void *__asan_addr_is_in_fake_stack(void *fakestack, void *addr, void **beg,
                                   void **end) {
  return 0;
}

void *__asan_get_current_fake_stack(void) {
  return 0;
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
  void *addr;
  size_t size;
  int prot, flag;
  int i, x, a, b;
  struct DirectMap sm;
  struct MemoryIntervals *m;
  SYSDEBUG("__asan_map_shadow(0x%p, 0x%x)", p, n);
  assert(!OverlapsShadowSpace((void *)p, n));
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
    flag = MAP_PRIVATE | MAP_FIXED | *weaken(MAP_ANONYMOUS);
    sm = weaken(sys_mmap)(addr, size, prot, flag, -1, 0);
    if (sm.addr == MAP_FAILED ||
        weaken(TrackMemoryInterval)(
            m, a, a + i - 1, sm.maphandle, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | *weaken(MAP_ANONYMOUS) | MAP_FIXED) == -1) {
      __asan_die("error: could not map asan shadow memory\n")();
      __asan_unreachable();
    }
    __repstosb((void *)(intptr_t)((int64_t)((uint64_t)a << 32) >> 16),
               kAsanUnmapped, size);
  }
  __asan_unpoison((uintptr_t)p, n);
}

static textstartup void __asan_shadow_string(char *s) {
  __asan_map_shadow((uintptr_t)s, __strlen(s) + 1);
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

static textstartup void __asan_shadow_existing_mappings(void) {
  size_t i;
  struct MemoryIntervals m;
  __asan_memcpy(&m, weaken(_mmi), sizeof(m));
  for (i = 0; i < m.i; ++i) {
    __asan_map_shadow((uintptr_t)m.p[i].x << 16,
                      (uintptr_t)(m.p[i].y - m.p[i].x + 1) << 16);
  }
  __asan_poison(GetStackAddr(0), PAGESIZE, kAsanStackOverflow);
}

static textstartup bool IsMemoryManagementRuntimeLinked(void) {
  return weaken(_mmi) && weaken(sys_mmap) && weaken(MAP_ANONYMOUS) &&
         weaken(TrackMemoryInterval);
}

textstartup void __asan_init(int argc, char **argv, char **envp,
                             intptr_t *auxv) {
  static bool once;
  if (!cmpxchg(&once, false, true)) return;
  if (IsWindows() && NtGetVersion() < kNtVersionWindows10) {
    __write_str("error: asan binaries require windows10\n");
    _Exit(0); /* So `make MODE=dbg test` passes w/ Windows7 */
  }
  REQUIRE(_mmi);
  REQUIRE(sys_mmap);
  REQUIRE(MAP_ANONYMOUS);
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
    __sysv_mprotect((void *)0x00007fff8000, 0x10000, PROT_READ);
  }
  __asan_shadow_string_list(argv);
  __asan_shadow_string_list(envp);
  __asan_shadow_auxv(auxv);
  __asan_install_malloc_hooks();
}

static textstartup void __asan_ctor(void) {
  if (weaken(__cxa_atexit)) {
    weaken(__cxa_atexit)(__asan_morgue_flush, NULL, NULL);
  }
}

const void *const g_asan_ctor[] initarray = {
    __asan_ctor,
};
