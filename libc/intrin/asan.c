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
#include "libc/bits/bits.h"
#include "libc/bits/likely.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/hook/hook.internal.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

#define COOKIE 21578

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

#define REQUIRE(FUNC)                              \
  do {                                             \
    if (!weaken(FUNC)) {                           \
      __asan_die("error: asan needs " #FUNC "\n"); \
    }                                              \
  } while (0)

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

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

static struct AsanMorgue __asan_morgue;

static inline int __asan_bsrl(uint64_t x) {
  return __builtin_clzll(x) ^ 63;
}

static uint64_t __asan_roundup2pow(uint64_t x) {
  return x > 1 ? 1ull << (__asan_bsrl(x - 1) + 1) : x ? 1 : 0;
}

static uint64_t __asan_rounddown2pow(uint64_t x) {
  return x ? 1ull << __asan_bsrl(x) : 0;
}

static uint64_t __asan_encodeutf8(unsigned c) {
  static const unsigned short kTpEnc[32 - 7] = {
      1 | 0300 << 8, 1 | 0300 << 8, 1 | 0300 << 8, 1 | 0300 << 8, 2 | 0340 << 8,
      2 | 0340 << 8, 2 | 0340 << 8, 2 | 0340 << 8, 2 | 0340 << 8, 3 | 0360 << 8,
      3 | 0360 << 8, 3 | 0360 << 8, 3 | 0360 << 8, 3 | 0360 << 8, 4 | 0370 << 8,
      4 | 0370 << 8, 4 | 0370 << 8, 4 | 0370 << 8, 4 | 0370 << 8, 5 | 0374 << 8,
      5 | 0374 << 8, 5 | 0374 << 8, 5 | 0374 << 8, 5 | 0374 << 8, 5 | 0374 << 8,
  };
  int e, n;
  unsigned long long w;
  if (c < 0200) return c;
  e = kTpEnc[__asan_bsrl(c) - 7];
  n = e & 0xff;
  w = 0;
  do {
    w |= 0200 | (c & 077);
    w <<= 8;
    c >>= 6;
  } while (--n);
  return c | w | e >> 8;
}

static size_t __asan_strlen(const char *s) {
  size_t n = 0;
  while (*s++) ++n;
  return n;
}

static int __asan_strcmp(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 255) - (r[i] & 255);
}

static char *__asan_stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

static void *__asan_repstosb(void *di, int al, size_t cx) {
  asm("rep stosb"
      : "=D"(di), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(cx), "a"(al));
  return di;
}

static void *__asan_repmovsb(void *di, void *si, size_t cx) {
  asm("rep movsb"
      : "=D"(di), "=S"(si), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(si), "2"(cx), "m"(*(char(*)[cx])si));
  return di;
}

static void *__asan_memset(void *p, int c, size_t n) {
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
        __asan_repstosb(p, c, n);
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
        return __asan_repmovsb(d, s, n);
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

static char *__asan_uintcpy(char p[hasatleast 21], uint64_t x) {
  char t;
  size_t i, a, b;
  i = 0;
  do {
    p[i++] = x % 10 + '0';
    x = x / 10;
  } while (x > 0);
  p[i] = '\0';
  if (i) {
    for (a = 0, b = i - 1; a < b; ++a, --b) {
      t = p[a];
      p[a] = p[b];
      p[b] = t;
    }
  }
  return p + i;
}

static char *__asan_intcpy(char p[hasatleast 21], int64_t x) {
  if (x < 0) *p++ = '-', x = -(uint64_t)x;
  return __asan_uintcpy(p, x);
}

privileged noinline wontreturn void __asan_exit(int rc) {
  if (!IsWindows()) {
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_exit_group), "D"(rc)
                 : "memory");
    unreachable;
  } else {
    ExitProcess(rc);
  }
}

privileged noinline ssize_t __asan_write(const void *data, size_t size) {
  ssize_t rc;
  uint32_t wrote;
  if (!IsWindows()) {
    asm volatile("syscall"
                 : "=a"(rc)
                 : "0"(__NR_write), "D"(2), "S"(data), "d"(size)
                 : "rcx", "r11", "memory");
    return rc;
  } else {
    if (WriteFile(GetStdHandle(kNtStdErrorHandle), data, size, &wrote, 0)) {
      return wrote;
    } else {
      return -1;
    }
  }
}

static ssize_t __asan_write_string(const char *s) {
  return __asan_write(s, __asan_strlen(s));
}

wontreturn void __asan_die(const char *msg) {
  __asan_write_string(msg);
  if (weaken(__die)) {
    weaken(__die)();
  } else {
    __printf("this binary needs\n"
             "\tSTATIC_YOINK(\"__die\");\n"
             "if you want to see backtraces\n");
  }
  __asan_exit(134);
}

void __asan_poison(uintptr_t p, size_t n, int t) {
  signed char k, *s;
  k = p & 7;
  s = (signed char *)((p >> 3) + 0x7fff8000);
  if (UNLIKELY(k)) {
    if (k < *s && *s <= k + n) *s = k;
    n -= MIN(8 - k, n);
    s += 1;
  }
  __asan_memset(s, t, n >> 3);
  if ((k = n & 7)) {
    s += n >> 3;
    if (*s >= 0) {
      *s = kAsanHeapOverrun;
    }
  }
}

void __asan_unpoison(uintptr_t p, size_t n) {
  signed char k, *s;
  k = p & 7;
  s = (signed char *)((p >> 3) + 0x7fff8000);
  if (UNLIKELY(k)) {
    if (!n) return;
    if (k + n < 8) {
      *s = MAX(*s, k + n);
      return;
    } else {
      *s = 0;
    }
    n -= MIN(8 - k, n);
    s += 1;
  }
  __asan_memset(s, 0, n >> 3);
  if ((k = n & 7)) {
    s += n >> 3;
    if (*s < 0) {
      *s = k;
    } else if (*s > 0) {
      *s = MAX(*s, k);
    }
  }
}

static inline bool __asan_is_mapped(int x) {
  int i;
  struct MemoryIntervals *m;
  m = weaken(_mmi);
  i = FindMemoryInterval(m, x);
  return i < m->i && m->p[i].x <= x && x <= m->p[i].y;
}

static inline bool __asan_is_image(const unsigned char *p) {
  return _base <= p && p < _end;
}

static inline bool __asan_exists(const void *x) {
  return __asan_is_image(x) || __asan_is_mapped((intptr_t)x >> 16);
}

static struct AsanFault __asan_fault(signed char *s, char dflt) {
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

struct AsanFault __asan_check(const void *p, size_t n) {
  intptr_t a;
  uint64_t w;
  unsigned u, r;
  signed char k, *s, *e, *f;
  if (!n) return (struct AsanFault){0};
  k = (intptr_t)p & 7;
  a = ((intptr_t)p >> 3) + 0x7fff8000;
  s = (signed char *)a;
  if (!__asan_is_mapped(a >> 16)) {
    return (struct AsanFault){kAsanUnmapped, s};
  }
  if (UNLIKELY(k)) {
    if (!*s) {
      n -= MIN(8 - k, n);
      s += 1;
    } else if (*s > 0 && k + n < 8 && *s >= k + n) {
      return (struct AsanFault){0};
    } else {
      return __asan_fault(s, kAsanHeapOverrun);
    }
  }
  e = s;
  k = n & 7;
  e += n >> 3;
  while (s < e && ((intptr_t)s & 7)) {
    if (*s++) {
      return __asan_fault(s - 1, kAsanHeapOverrun);
    }
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
      s += (unsigned)__builtin_ctzll(w) >> 3;
      return __asan_fault(s, kAsanHeapOverrun);
    }
  }
  while (s < e) {
    if (*s++) {
      return __asan_fault(s - 1, kAsanHeapOverrun);
    }
  }
  if (!k || !*s || k <= *s) {
    return (struct AsanFault){0};
  } else {
    return __asan_fault(s, kAsanHeapOverrun);
  }
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
  __asan_report(p, n, "verify", f.kind);
}

bool __asan_is_valid(const void *p, size_t n) {
  return !__asan_check(p, n).kind;
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

static const char *__asan_dscribe_heap_poison(long c) {
  switch (c) {
    case kAsanHeapFree:
      return "heap double free";
    case kAsanStackFree:
      return "stack double free";
    case kAsanRelocated:
      return "free after relocate";
    default:
      return "this corruption";
  }
}

static const char *__asan_describe_access_poison(char kind) {
  switch (kind) {
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
    case kAsanAllocaUnderrun:
      return "alloca underflow";
    case kAsanAllocaOverrun:
      return "alloca overflow";
    case kAsanUnscoped:
      return "unscoped";
    case kAsanUnmapped:
      return "unmapped";
    case kAsanProtected:
      return "protected";
    case kAsanStackGuard:
      return "stack overflow";
    case kAsanNullPage:
      return "null page access";
    default:
      return "poisoned";
  }
}

static bool __asan_ansi(void) {
  const char *term;
  term = weaken(getenv) ? weaken(getenv)("TERM") : NULL;
  return !term || __asan_strcmp(term, "dumb") != 0;
}

static char *__asan_report_start(char *p, bool ansi) {
  if (ansi) p = __asan_stpcpy(p, "\r\e[J\e[1;91m");
  p = __asan_stpcpy(p, "asan error");
  if (ansi) p = __asan_stpcpy(p, "\e[0m");
  return __asan_stpcpy(p, ": ");
}

static wontreturn void __asan_report_invalid_pointer(void *addr) {
  char *p;
  p = __asan_report_start(__fatalbuf, __asan_ansi());
  p = __asan_stpcpy(p, "invalid pointer 0x");
  p = __asan_hexcpy(p, (intptr_t)addr, 48);
  p = __asan_stpcpy(p, " shadow 0x");
  p = __asan_hexcpy(p, (intptr_t)SHADOW(addr), 48);
  p = __asan_stpcpy(p, "\r\n");
  __asan_die(__fatalbuf);
}

static char *__asan_format_interval(char *p, intptr_t a, intptr_t b) {
  p = __asan_hexcpy(p, a, 48);
  *p++ = '-';
  p = __asan_hexcpy(p, b, 48);
  return p;
}

static char *__asan_format_section(char *p, void *p1, void *p2,
                                   const char *name, void *addr) {
  intptr_t a, b;
  if ((a = (intptr_t)p1) < (b = (intptr_t)p2)) {
    p = __asan_format_interval(p, a, b);
    *p++ = ' ';
    p = __asan_stpcpy(p, name);
    if (a <= (intptr_t)addr && (intptr_t)addr <= b) {
      p = __asan_stpcpy(p, " ←address");
    }
    *p++ = '\r';
    *p++ = '\n';
  }
  return p;
}

wontreturn void __asan_report(void *addr, int size, const char *message,
                              char kind) {
  bool a;
  wint_t c;
  int i, cc;
  signed char t;
  uint64_t x, y, z;
  char *p, *q, *base;
  struct MemoryIntervals *m;
  a = __asan_ansi();
  p = __asan_report_start(__fatalbuf, a);
  p = __asan_stpcpy(p, __asan_describe_access_poison(kind));
  p = __asan_stpcpy(p, " ");
  p = __asan_intcpy(p, size);
  p = __asan_stpcpy(p, "-byte ");
  p = __asan_stpcpy(p, message);
  p = __asan_stpcpy(p, " at 0x");
  p = __asan_hexcpy(p, (uintptr_t)addr, 48);
  p = __asan_stpcpy(p, " shadow 0x");
  p = __asan_hexcpy(p, (uintptr_t)SHADOW(addr), 48);
  *p++ = '\r', *p++ = '\n';
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
        if (a && c != 32) {
          p = __asan_stpcpy(p, "\e[32m");
          c = 32;
        }
        *p++ = '.';
      } else {
        if (a && c != 31) {
          p = __asan_stpcpy(p, "\e[31m");
          c = 31;
        }
        *p++ = "FFRRUOOGUOUOSMP~"[-t & 15];
      }
    }
    if (a) p = __asan_stpcpy(p, "\e[39m");
    *p++ = '\r', *p++ = '\n';
    for (i = 0; (intptr_t)(base + i) & 7; ++i) *p++ = ' ';
    for (; i + 8 <= 80; i += 8) {
      q = p + 8;
      *p++ = '|';
      z = ((intptr_t)(base + i) >> 3) + 0x7fff8000;
      if (__asan_is_mapped(z >> 16)) {
        p = __asan_intcpy(p, *(signed char *)z);
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
      if (__asan_exists(base + i)) {
        c = kCp437[((unsigned char *)base)[i]];
      } else {
        c = u'⋅';
      }
      z = __asan_encodeutf8(c);
      do {
        *p++ = z;
      } while ((z >>= 8));
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
    if (x <= z && z <= y) p = __asan_stpcpy(p, " ←address");
    z = (((intptr_t)addr >> 3) + 0x7fff8000) >> 16;
    if (x <= z && z <= y) p = __asan_stpcpy(p, " ←shadow");
    *p++ = '\r', *p++ = '\n';
  }
  *p = 0;
  __asan_die(__fatalbuf);
}

wontreturn void __asan_report_memory_fault(void *addr, int size,
                                           const char *message) {
  __asan_report(addr, size, message, __asan_fault(SHADOW(addr), -128).kind);
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
    if (cmpxchg(__asan_morgue.p + i, p, NULL)) {
      if (weaken(dlfree)) {
        weaken(dlfree)(p);
      }
    }
  }
}

static size_t __asan_heap_size(size_t n) {
  if (n <= 0x7fffffffffff) {
    return __asan_roundup2pow(ROUNDUP(n, 8) + 8);
  } else {
    return -1;
  }
}

static size_t __asan_user_size(size_t n) {
  if (n) {
    return n;
  } else {
    return 1;
  }
}

static size_t __asan_stack_size(void) {
  extern char ape_stack_memsz[] __attribute__((__weak__));
  if (ape_stack_memsz) {
    return (uintptr_t)ape_stack_memsz;
  } else {
    return STACKSIZE;
  }
}

forceinline void __asan_write48(char *p, uint64_t x) {
  uint64_t value, cookie;
  cookie = COOKIE;
  cookie ^= x & 0xffff;
  value = (x & 0xffffffffffff) | cookie << 48;
  WRITE64BE(p, value);
}

forceinline bool __asan_read48(const char *p, uint64_t *x) {
  uint64_t value, cookie;
  value = READ64BE(p);
  cookie = value >> 48;
  cookie ^= value & 0xffff;
  *x = (int64_t)(value << 16) >> 16;
  return cookie == COOKIE;
}

static void *__asan_allocate(size_t a, size_t n, int underrun, int overrun) {
  char *p;
  size_t c;
  if ((p = weaken(dlmemalign)(a, __asan_heap_size(n)))) {
    n = __asan_user_size(n);
    c = weaken(dlmalloc_usable_size)(p);
    __asan_unpoison((uintptr_t)p, n);
    __asan_poison((uintptr_t)p - 16, 16, underrun); /* see dlmalloc design */
    __asan_poison((uintptr_t)p + n, c - n, overrun);
    __asan_memset(p, 0xF9, n);
    __asan_write48(p + c - 8, n);
  }
  return p;
}

static size_t __asan_malloc_usable_size(const void *p) {
  size_t c, n;
  struct AsanFault f;
  if (!(f = __asan_check(p, 1)).kind) {
    if ((c = weaken(dlmalloc_usable_size)(p)) >= 8) {
      if (__asan_read48((char *)p + c - 8, &n) && n <= c) {
        return n;
      }
    }
    __asan_report_invalid_pointer(p);
  } else {
    __asan_report(p, 1, "heaping", f.kind);
  }
}

static void __asan_deallocate(char *p, long kind) {
  size_t c, n;
  if ((c = weaken(dlmalloc_usable_size)(p)) >= 8) {
    if (__asan_read48(p + c - 8, &n) && n <= c) {
      __asan_poison((uintptr_t)p, c, kind);
      if (c <= FRAMESIZE) {
        p = __asan_morgue_add(p);
      }
      weaken(dlfree)(p);
    } else {
      __asan_report_invalid_pointer(p);
    }
  } else {
    __asan_report_invalid_pointer(p);
  }
}

void __asan_free(void *p) {
  if (!p) return;
  __asan_deallocate(p, kAsanHeapFree);
}

void *__asan_memalign(size_t align, size_t size) {
  return __asan_allocate(align, size, kAsanHeapUnderrun, kAsanHeapOverrun);
}

void *__asan_malloc(size_t size) {
  return __asan_memalign(__BIGGEST_ALIGNMENT__, size);
}

void *__asan_calloc(size_t n, size_t m) {
  char *p;
  if (__builtin_mul_overflow(n, m, &n)) n = -1;
  if ((p = __asan_malloc(n))) __asan_memset(p, 0, n);
  return p;
}

static void *__asan_realloc_nogrow(void *p, size_t n, size_t m) {
  return 0;
}

static void *__asan_realloc_grow(void *p, size_t n, size_t m) {
  char *q;
  if ((q = __asan_malloc(n))) {
    __asan_memcpy(q, p, m);
    __asan_deallocate(p, kAsanRelocated);
  }
  return q;
}

static void *__asan_realloc_impl(void *p, size_t n,
                                 void *grow(void *, size_t, size_t)) {
  char *f;
  size_t c, m;
  if ((c = weaken(dlmalloc_usable_size)(p)) >= 8) {
    f = (char *)p + c - 8;
    if (__asan_read48(f, &m) && m <= c) {
      if (n <= m) { /* shrink */
        __asan_poison((uintptr_t)p + n, m - n, kAsanHeapOverrun);
        __asan_write48(f, n);
        return p;
      } else if (n <= c - 8) { /* small growth */
        __asan_unpoison((uintptr_t)p + m, n - m);
        __asan_write48(f, n);
        return p;
      } else { /* exponential growth */
        return grow(p, n, m);
      }
    } else {
      __asan_report_invalid_pointer(p);
    }
  } else {
    __asan_report_invalid_pointer(p);
  }
}

void *__asan_realloc(void *p, size_t n) {
  if (p) {
    if (n) {
      return __asan_realloc_impl(p, n, __asan_realloc_grow);
    } else {
      __asan_free(p);
      return 0;
    }
  } else {
    return __asan_malloc(n);
  }
}

void *__asan_realloc_in_place(void *p, size_t n) {
  if (p) {
    return __asan_realloc_impl(p, n, __asan_realloc_nogrow);
  } else {
    return 0;
  }
}

void *__asan_valloc(size_t n) {
  return __asan_memalign(PAGESIZE, n);
}

void *__asan_pvalloc(size_t n) {
  return __asan_valloc(ROUNDUP(n, PAGESIZE));
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
  return __asan_allocate(32, size, kAsanStackUnderrun, kAsanStackOverrun);
}

void __asan_stack_free(char *p, size_t size, int classid) {
  __asan_deallocate(p, kAsanStackFree);
}

void __asan_handle_no_return(void) {
  uintptr_t stk, ssz;
  stk = (uintptr_t)__builtin_frame_address(0);
  ssz = __asan_stack_size();
  __asan_unpoison(stk, ROUNDUP(stk, ssz) - stk);
}

void __asan_register_globals(struct AsanGlobal g[], int n) {
  int i;
  for (i = 0; i < n; ++i) {
    __asan_unpoison(g[i].addr, g[i].size);
    __asan_poison(g[i].addr + g[i].size, g[i].size_with_redzone - g[i].size,
                  kAsanGlobalOverrun);
  }
}

void __asan_unregister_globals(struct AsanGlobal g[], int n) {
  int i;
  for (i = 0; i < n; ++i) {
    __asan_poison(g[i].addr, g[i].size_with_redzone, kAsanGlobalUnregistered);
  }
}

void __asan_report_load(uint8_t *addr, int size) {
  __asan_report_memory_fault(addr, size, "load");
}

void __asan_report_store(uint8_t *addr, int size) {
  __asan_report_memory_fault(addr, size, "store");
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
  return NULL;
}

void *__asan_get_current_fake_stack(void) {
  return NULL;
}

void __asan_install_malloc_hooks(void) {
  HOOK(hook_free, __asan_free);
  HOOK(hook_malloc, __asan_malloc);
  HOOK(hook_calloc, __asan_calloc);
  HOOK(hook_valloc, __asan_valloc);
  HOOK(hook_pvalloc, __asan_pvalloc);
  HOOK(hook_realloc, __asan_realloc);
  HOOK(hook_memalign, __asan_memalign);
  HOOK(hook_malloc_trim, __asan_malloc_trim);
  HOOK(hook_realloc_in_place, __asan_realloc_in_place);
  HOOK(hook_malloc_usable_size, __asan_malloc_usable_size);
}

void __asan_map_shadow(uintptr_t p, size_t n) {
  int i, x, a, b;
  struct DirectMap sm;
  struct MemoryIntervals *m;
  if ((0x7fff8000 <= p && p < 0x100080000000) ||
      (0x7fff8000 <= p + n && p + n < 0x100080000000) ||
      (p < 0x7fff8000 && 0x100080000000 <= p + n)) {
    __asan_die("asan error: mmap can't shadow a shadow\r\n");
  }
  m = weaken(_mmi);
  a = (uintptr_t)SHADOW(p) >> 16;
  b = ROUNDUP((uintptr_t)SHADOW(ROUNDUP((uintptr_t)p + n, 8)), 1 << 16) >> 16;
  for (; a < b; ++a) {
    if (!__asan_is_mapped(a)) {
      sm = weaken(sys_mmap)(
          (void *)((uintptr_t)a << 16), 1 << 16, PROT_READ | PROT_WRITE,
          MAP_PRIVATE | *weaken(MAP_ANONYMOUS) | MAP_FIXED, -1, 0);
      if (sm.addr == MAP_FAILED ||
          weaken(TrackMemoryInterval)(
              m, a, a, sm.maphandle, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | *weaken(MAP_ANONYMOUS) | MAP_FIXED) == -1) {
        __asan_die("error: could not map asan shadow memory\n");
      }
      __asan_repstosb((void *)((uintptr_t)a << 16), kAsanUnmapped, 1 << 16);
    }
  }
  __asan_unpoison((uintptr_t)p, n);
}

static textstartup void __asan_shadow_string(char *s) {
  __asan_map_shadow((uintptr_t)s, __asan_strlen(s) + 1);
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
  uintptr_t rsp, stk, ssz;
  struct MemoryIntervals m;
  __asan_memcpy(&m, weaken(_mmi), sizeof(m));
  for (i = 0; i < m.i; ++i) {
    __asan_map_shadow((uintptr_t)m.p[i].x << 16,
                      (uintptr_t)(m.p[i].y - m.p[i].x + 1) << 16);
  }
  rsp = (uintptr_t)__builtin_frame_address(0);
  ssz = __asan_stack_size();
  stk = ROUNDDOWN(rsp, ssz);
  __asan_poison(stk, PAGESIZE, kAsanStackGuard);
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
    __asan_write_string("error: asan binaries require windows10\n");
    __asan_exit(0); /* So `make MODE=dbg test` passes w/ Windows7 */
  }
  REQUIRE(_mmi);
  REQUIRE(sys_mmap);
  REQUIRE(MAP_ANONYMOUS);
  REQUIRE(TrackMemoryInterval);
  if (weaken(hook_malloc) || weaken(hook_calloc) || weaken(hook_realloc) ||
      weaken(hook_realloc_in_place) || weaken(hook_pvalloc) ||
      weaken(hook_valloc) || weaken(hook_free) ||
      weaken(hook_malloc_usable_size)) {
    REQUIRE(dlmemalign);
    REQUIRE(dlmalloc_usable_size);
  }
  __asan_shadow_existing_mappings();
  __asan_map_shadow((uintptr_t)_base, _end - _base);
  __asan_map_shadow(0, 4096);
  __asan_poison(0, PAGESIZE, kAsanNullPage);
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

const void *const g_asan_ctor[] initarray = {__asan_ctor};
