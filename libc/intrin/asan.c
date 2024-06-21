/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "ape/sections.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/leaky.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/serialize.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#include "third_party/dlmalloc/dlmalloc.h"
#ifdef __x86_64__
#ifdef __SANITIZE_ADDRESS__

__static_yoink("_init_asan");

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

#define RBP __builtin_frame_address(0)

#define ASAN_LOG(...) (void)0  // kprintf(__VA_ARGS__)

struct AsanSourceLocation {
  char *filename;
  int line;
  int column;
};

struct AsanAccessInfo {
  char *addr;
  const uintptr_t first_bad_addr;
  size_t size;
  bool iswrite;
  unsigned long ip;
};

struct AsanGlobal {
  char *addr;
  size_t size;
  size_t size_with_redzone;
  const void *name;
  const void *module_name;
  unsigned long has_cxx_init;
  struct AsanSourceLocation *location;
  char *odr_indicator;
};

int __asan_option_detect_stack_use_after_return = 0;

void __asan_version_mismatch_check_v8(void) {
}

static bool __asan_once(void) {
  int want = false;
  static atomic_int once;
  return atomic_compare_exchange_strong_explicit(
      &once, &want, true, memory_order_relaxed, memory_order_relaxed);
}

static int __asan_bsf(uint64_t x) {
  _Static_assert(sizeof(long long) == sizeof(uint64_t), "");
  return __builtin_ctzll(x);
}

static char *__asan_utf8cpy(char *p, unsigned c) {
  uint64_t z;
  z = tpenc(c);
  do
    *p++ = z;
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

dontinstrument void __asan_memset(void *p, char c, size_t n) {
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
      i = 0;
      do {
        __builtin_memcpy(b + i, &x, 8);
        asm volatile("" ::: "memory");
        __builtin_memcpy(b + i + 8, &x, 8);
      } while ((i += 16) + 16 <= n);
      for (; i < n; ++i)
        b[i] = x;
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
      i = 0;
      do {
        __builtin_memcpy(&a, s + i, 8);
        asm volatile("" ::: "memory");
        __builtin_memcpy(d + i, &a, 8);
      } while ((i += 8) + 8 <= n);
      for (; i < n; ++i)
        d[i] = s[i];
      return d + i;
  }
}

void *__asan_memcpy(void *dst, const void *src, size_t n) {
  __asan_mempcpy(dst, src, n);
  return dst;
}

static char *__asan_hexcpy(char *p, uint64_t x, uint8_t k) {
  while (k)
    *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  return p;
}

static void __asan_exit(void) {
  kprintf("your asan runtime needs\n"
          "\t__static_yoink(\"__die\");\n"
          "in order to show you backtraces\n");
  _Exit(99);
}

__wur __asan_die_f *__asan_die(void) {
  if (_weaken(__die)) {
    return _weaken(__die);
  } else {
    return __asan_exit;
  }
}

static wontreturn void __asan_require_failed(const char *func) {
  kprintf("error: asan needs %s\n", func);
  __asan_die()();
  __asan_unreachable();
}

static bool __asan_overlaps_shadow_space(const void *p, size_t n) {
  intptr_t BegA, EndA, BegB, EndB;
  if (n) {
    BegA = (intptr_t)p;
    EndA = BegA + n;
    BegB = 0x7fff0000;
    EndB = 0x100080000000;
    return MAX(BegA, BegB) < MIN(EndA, EndB);
  } else {
    return 0;
  }
}

void __asan_unpoison(void *p, long n) {
  signed char k, *s;
  k = (intptr_t)p & 7;
  s = (signed char *)(((intptr_t)p >> 3) + 0x7fff8000);
  if (UNLIKELY(k)) {
    if (k + n < 8) {
      if (n > 0)
        *s = MAX(*s, k + n);
      return;
    }
    n -= MIN(8 - k, n);
    *s++ = 0;
  }
  __asan_memset(s, 0, n >> 3);
  if ((k = n & 7)) {
    s += n >> 3;
    if (*s < 0)
      *s = k;
    if (*s > 0)
      *s = MAX(*s, k);
  }
}

dontinstrument void __asan_poison(void *p, long n, signed char t) {
  signed char k, *s;
  s = (signed char *)(((intptr_t)p >> 3) + 0x7fff8000);
  if ((k = (intptr_t)p & 7)) {
    if ((!*s && n >= 8 - k) || *s > k)
      *s = k;
    n -= MIN(8 - k, n);
    s += 1;
  }
  __asan_memset(s, t, n >> 3);
  if ((k = n & 7)) {
    s += n >> 3;
    if (*s < 0 || (*s > 0 && *s <= k))
      *s = t;
  }
}

static void __asan_poison_safe(char *p, long n, signed char t) {
  int granularity = __granularity();
  int chunk = granularity << 3;
  while (n > 0) {
    char *block = (char *)((uintptr_t)p & -chunk);
    signed char *shadow = (signed char *)(((uintptr_t)block >> 3) + 0x7fff8000);
    if (__asan_is_mapped(shadow)) {
      char *start = MAX(p, block);
      size_t bytes = MIN(n, chunk);
      __asan_poison(start, bytes, t);
    }
    p += chunk;
    n -= chunk;
  }
}

privileged static bool __asan_is_mapped_unlocked(const char *addr) {
  struct Dll *e, *e2;
  for (e = dll_first(__maps.used); e; e = e2) {
    e2 = dll_next(__maps.used, e);
    struct Map *map = MAP_CONTAINER(e);
    if (map->addr <= addr && addr < map->addr + map->size) {
      dll_remove(&__maps.used, e);
      dll_make_first(&__maps.used, e);
      return true;
    }
  }
  return false;
}

privileged bool __asan_is_mapped(const void *addr) {
  bool32 res;
  __maps_lock();
  res = __asan_is_mapped_unlocked(addr);
  __maps_unlock();
  return res;
}

static void __asan_ensure_shadow_is_mapped(void *addr, size_t size) {
  int m = 0x7fff8000;
  int g = __granularity();
  size_t n = (size + 7) & -8;
  char *p = (char *)((((uintptr_t)addr >> 3) + m) & -g);
  char *pe = (char *)(((((uintptr_t)(addr + n) >> 3) + m) + g - 1) & -g);
  for (char *pm = p; p < pe; p = pm) {
    pm += g;
    if (!__asan_is_mapped(p)) {
      while (pm < pe && !__asan_is_mapped(pm))
        pm += g;
      if (__mmap(p, pm - p, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) != MAP_FAILED)
        __asan_memset(p, kAsanUnmapped, pm - p);
    }
  }
}

void __asan_shadow(void *addr, size_t size) {
  if (!__asan_overlaps_shadow_space(addr, size)) {
    __asan_ensure_shadow_is_mapped(addr, size);
    __asan_unpoison(addr, size);
  }
}

void __asan_unshadow(void *addr, size_t size) {
  if (!__asan_overlaps_shadow_space(addr, size)) {
    __asan_poison_safe(addr, size, kAsanUnmapped);
  }
}

static bool __asan_is_image(const unsigned char *p) {
  return __executable_start <= p && p < _end;
}

static bool __asan_exists(const void *x) {
  return !kisdangerous(x);
}

static struct AsanFault __asan_fault(const signed char *s, signed char dflt) {
  struct AsanFault r;
  if (s[0] < 0) {
    r.kind = s[0];
  } else if (((uintptr_t)(s + 1) & 4095) && s[1] < 0) {
    r.kind = s[1];
  } else {
    r.kind = dflt;
  }
  r.shadow = s;
  return r;
}

static struct AsanFault __asan_checka(const signed char *s, long ndiv8) {
  uint64_t w;
  int g = __granularity();
  const signed char *o = s;
  const signed char *e = s + ndiv8;
  for (; ((intptr_t)s & 7) && s < e; ++s) {
    if (*s)
      return __asan_fault(s - 1, kAsanHeapOverrun);
  }
  for (; s + 8 <= e; s += 8) {
    if (UNLIKELY(!((intptr_t)s & (g - 1))) && kisdangerous(s)) {
      return (struct AsanFault){kAsanUnmapped, s};
    }
    if ((w = READ64LE(s))) {
      s += __asan_bsf(w) >> 3;
      return __asan_fault(s, kAsanHeapOverrun);
    }
  }
  for (; s < e; ++s) {
    if (*s)
      return __asan_fault(s > o ? s - 1 : s, kAsanHeapOverrun);
  }
  return (struct AsanFault){0};
}

/**
 * Checks validity of memory range.
 *
 * This is normally abstracted by the compiler. In some cases, it may be
 * desirable to perform an ASAN memory safety check explicitly, e.g. for
 * system call wrappers that need to vet memory passed to the kernel, or
 * string library routines that use the `dontasan` keyword due to compiler
 * generated ASAN being too costly. This function is fast especially for
 * large memory ranges since this takes a few picoseconds for each byte.
 *
 * @param p is starting virtual address
 * @param n is number of bytes to check
 * @return kind is 0 on success or <0 if invalid
 * @return shadow points to first poisoned shadow byte
 */
struct AsanFault __asan_check(const void *p, long n) {
  struct AsanFault f;
  signed char c, k, *s;
  if (n > 0) {
    k = (intptr_t)p & 7;
    s = SHADOW(p);
    if (__asan_overlaps_shadow_space(p, n)) {
      return (struct AsanFault){kAsanProtected, s};
    } else if (kisdangerous(s)) {
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

/**
 * Checks validity of nul-terminated string.
 *
 * This is similar to `__asan_check(p, 1)` except it checks the validity
 * of the entire string including its trailing nul byte, and goes faster
 * than calling strlen() beforehand.
 *
 * @param p is starting virtual address
 * @param n is number of bytes to check
 * @return kind is 0 on success or <0 if invalid
 * @return shadow points to first poisoned shadow byte
 */
struct AsanFault __asan_check_str(const char *p) {
  uint64_t w;
  signed char c, k, *s;
  int g = __granularity();
  s = SHADOW(p);
  if (__asan_overlaps_shadow_space(p, 1)) {
    return (struct AsanFault){kAsanProtected, s};
  }
  if (kisdangerous(s)) {
    return (struct AsanFault){kAsanUnmapped, s};
  }
  if ((k = (intptr_t)p & 7)) {
    do {
      if ((c = *s) && c < k + 1) {
        return __asan_fault(s, kAsanHeapOverrun);
      }
      if (!*p++) {
        return (struct AsanFault){0};
      }
    } while ((k = (intptr_t)p & 7));
    ++s;
  }
  for (;; ++s, p += 8) {
    if (UNLIKELY(!((intptr_t)s & (g - 1))) && kisdangerous(s)) {
      return (struct AsanFault){kAsanUnmapped, s};
    }
    if ((c = *s) < 0) {
      return (struct AsanFault){c, s};
    }
    w = *(const uint64_t *)p;
    if (!(w = ~w & (w - 0x0101010101010101) & 0x8080808080808080)) {
      if (c > 0) {
        return __asan_fault(s, kAsanHeapOverrun);
      }
    } else {
      k = (unsigned)__builtin_ctzll(w) >> 3;
      if (!c || c > k) {
        return (struct AsanFault){0};
      } else {
        return __asan_fault(s, kAsanHeapOverrun);
      }
    }
  }
}

/**
 * Checks memory validity of memory region.
 */
bool __asan_is_valid(const void *p, long n) {
  struct AsanFault f;
  f = __asan_check(p, n);
  return !f.kind;
}

/**
 * Checks memory validity of nul-terminated string.
 */
bool __asan_is_valid_str(const char *p) {
  struct AsanFault f;
  f = __asan_check_str(p);
  return !f.kind;
}

/**
 * Checks memory validity of null-terminated nul-terminated string list.
 */
bool __asan_is_valid_strlist(char *const *p) {
  for (;; ++p) {
    if (!__asan_is_valid(p, sizeof(char *)))
      return false;
    if (!*p)
      return true;
    if (!__asan_is_valid_str(*p))
      return false;
  }
}

/**
 * Checks memory validity of `iov` array and each buffer it describes.
 */
bool __asan_is_valid_iov(const struct iovec *iov, int iovlen) {
  int i;
  size_t size;
  if (iovlen >= 0 && !ckd_mul(&size, iovlen, sizeof(struct iovec)) &&
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

static wint_t __asan_symbolize_access_poison(signed char kind) {
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
    case kAsanMmapSizeOverrun:
      return L'Z';
    default:
      return L'?';
  }
}

static const char *__asan_describe_access_poison(signed char kind) {
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

void __asan_report_memory_origin_image(intptr_t a, int z) {
  unsigned l, m, r, n, k;
  struct SymbolTable *st;
  kprintf("\nthe memory belongs to image symbols\n");
  if (_weaken(GetSymbolTable)) {
    if ((st = _weaken(GetSymbolTable)())) {
      l = 0;
      r = n = st->count;
      k = a - st->addr_base;
      while (l < r) {
        m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
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
      kprintf("\tunknown please supply .dbg symbols or set COMDBG\n");
    }
  } else {
    kprintf("\tunknown please __static_yoink(\"GetSymbolTable\");\n");
  }
}

void __asan_report_memory_origin(const unsigned char *addr, int size,
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
  if (__executable_start <= addr && addr < _end) {
    __asan_report_memory_origin_image((intptr_t)addr, size);
    /* } else if (IsAutoFrame((intptr_t)addr >> 16)) { */
    /*   if (_weaken(__asan_report_memory_origin_heap)) */
    /*     _weaken(__asan_report_memory_origin_heap)(addr, size); */
  }
}

static __wur __asan_die_f *__asan_report(const void *addr, int size,
                                         const char *message,
                                         signed char kind) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  char buf[8192];
  CheckLargeStackAllocation(buf, sizeof(buf));
#pragma GCC pop_options
  int i;
  wint_t c;
  signed char t;
  uint64_t z;
  char *base, *q, *p = buf;
  ftrace_enabled(-1);
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
    for (i = 0; (intptr_t)(base + i) & 7; ++i)
      *p++ = ' ';
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
    for (; i < 80; ++i)
      *p++ = ' ';
    *p++ = '\n';
    for (i = 0; i < 80; ++i) {
      p = __asan_utf8cpy(p, __asan_exists(base + i)
                                ? kCp437[((unsigned char *)base)[i]]
                                : L'⋅');
    }
    *p++ = '\n';
  }
  p = __asan_format_section(p, __executable_start, _etext, ".text", addr);
  p = __asan_format_section(p, _etext, _edata, ".data", addr);
  p = __asan_format_section(p, _end, _edata, ".bss", addr);
  __maps_lock();
  struct Dll *e, *e2;
  for (e = dll_first(__maps.used); e; e = e2) {
    e2 = dll_next(__maps.used, e);
    struct Map *map = MAP_CONTAINER(e);
    p = __asan_format_interval(p, (uintptr_t)map->addr,
                               (uintptr_t)map->addr + map->size);
    if (!__asan_overlaps_shadow_space(map->addr, map->size))
      p = __asan_stpcpy(p, " ←address");
    else
      p = __asan_stpcpy(p, " ←shadow");
    *p++ = '\n';
  }
  __maps_unlock();
  *p = 0;
  kprintf("%s", buf);
  __asan_report_memory_origin(addr, size, kind);
  kprintf("\nthe crash was caused by %s\n", program_invocation_name);
  ftrace_enabled(+1);
  return __asan_die();
}

static wontreturn void __asan_verify_failed(const void *p, size_t n,
                                            struct AsanFault f) {
  const char *q;
  q = UNSHADOW(f.shadow);
  if ((uintptr_t)q != ((uintptr_t)p & -8) && (uintptr_t)q - (uintptr_t)p < n) {
    n -= (uintptr_t)q - (uintptr_t)p;
    p = q;
  }
  __asan_report(p, n, "verify", f.kind)();
  __asan_unreachable();
}

void __asan_verify(const void *p, size_t n) {
  struct AsanFault f;
  if (!(f = __asan_check(p, n)).kind)
    return;
  __asan_verify_failed(p, n, f);
}

void __asan_verify_str(const char *p) {
  struct AsanFault f;
  if (!(f = __asan_check_str(p)).kind)
    return;
  __asan_verify_failed(UNSHADOW(f.shadow), 8, f);
}

static __wur __asan_die_f *__asan_report_memory_fault(void *addr, int size,
                                                      const char *message) {
  return __asan_report(addr, size, message,
                       __asan_fault(SHADOW(addr), -128).kind);
}

void __asan_rawtrace(struct AsanTrace *bt, const struct StackFrame *bp) {
  size_t i;
  for (i = 0; bp && i < ARRAYLEN(bt->p); ++i, bp = bp->next) {
    if (kisdangerous(bp))
      break;
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
  garbage = __tls_enabled ? __get_tls()->tib_garbages : 0;
  gi = garbage ? garbage->i : 0;
  for (f1 = -1, i = 0; bp && i < ARRAYLEN(bt->p); ++i, bp = bp->next) {
    if (f1 != (f2 = ((intptr_t)bp >> 16))) {
      if (kisdangerous(bp))
        break;
      f1 = f2;
    }
    if (!__asan_checka(SHADOW(bp), sizeof(*bp) >> 3).kind) {
      addr = bp->addr;
#ifdef __x86_64__
      if (addr == (uintptr_t)_weaken(__gc) && (uintptr_t)_weaken(__gc)) {
        do
          --gi;
        while ((addr = garbage->p[gi].ret) == (uintptr_t)_weaken(__gc));
      }
#endif
      bt->p[i] = addr;
    } else {
      break;
    }
  }
  for (; i < ARRAYLEN(bt->p); ++i) {
    bt->p[i] = 0;
  }
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
  __asan_rawtrace(&tr, RBP);
  kprintf("WARNING: ASAN bad %d byte %s at %lx bt %x %x %x\n", size, s, addr,
          tr.p[0], tr.p[1], tr.p[2], tr.p[3]);
}

void __asan_report_load(uint8_t *addr, int size) {
  __asan_evil(addr, size, "load");
  if (!__vforked && __asan_once()) {
    __asan_report_memory_fault(addr, size, "load")();
  }
  __asan_unreachable();
}

void __asan_report_store(uint8_t *addr, int size) {
  __asan_evil(addr, size, "store");
  if (!__vforked && __asan_once()) {
    __asan_report_memory_fault(addr, size, "store")();
  }
  __asan_unreachable();
}

void __asan_handle_no_return(void) {
  // this check is stupid and has far-reaching toilsome ramifications
}

void __asan_poison_stack_memory(char *addr, size_t size) {
  ASAN_LOG("__asan_poison_stack_memory(%p, %zu)\n", addr, size);
  __asan_poison(addr, size, kAsanStackFree);
}

void __asan_unpoison_stack_memory(char *addr, size_t size) {
  ASAN_LOG("__asan_unpoison_stack_memory(%p, %zu)\n", addr, size);
  __asan_unpoison(addr, size);
}

void __asan_alloca_poison(char *addr, uintptr_t size) {
  ASAN_LOG("__asan_alloca_poison(%p, %zu)\n", addr, size);
  size_t rounded_up_size = ROUNDUP(size, 8);
  size_t padding_size = ROUNDUP(size, 32) - rounded_up_size;
  size_t rounded_down_size = ROUNDDOWN(size, 8);
  char *left_redzone = addr - 32;
  void *right_redzone = addr + rounded_up_size;
  __asan_unpoison(addr + rounded_down_size, size - rounded_down_size);
  __asan_poison(left_redzone, 32, kAsanAllocaUnderrun);
  __asan_poison(right_redzone, padding_size + 32, kAsanAllocaOverrun);
}

void __asan_allocas_unpoison(uintptr_t x, uintptr_t y) {
  ASAN_LOG("__asan_allocas_unpoison(%p, %p)\n", x, y);
  if (!x || x > y)
    return;
  __asan_unpoison((char *)x, y - x);
}

void *__asan_addr_is_in_fake_stack(void *fakestack, void *addr, void **beg,
                                   void **end) {
  ASAN_LOG("__asan_addr_is_in_fake_stack(%p)\n", addr);
  return 0;
}

void *__asan_get_current_fake_stack(void) {
  ASAN_LOG("__asan_get_current_fake_stack()\n");
  return 0;
}

void __sanitizer_annotate_contiguous_container(char *beg, char *end,
                                               char *old_mid, char *new_mid) {
  ASAN_LOG("__sanitizer_annotate_contiguous_container()\n");
  // the c++ stl uses this
  // TODO(jart): make me faster
  __asan_unpoison(beg, new_mid - beg);
  __asan_poison(new_mid, end - new_mid, kAsanHeapOverrun);
}

void __asan_before_dynamic_init(const char *module_name) {
  ASAN_LOG("__asan_before_dynamic_init()\n");
}

void __asan_after_dynamic_init(void) {
  ASAN_LOG("__asan_after_dynamic_init()\n");
}

void __asan_init(void) {
  static bool once;
  if (once)
    return;
  once = true;

  __asan_shadow(0, 4096);
  __asan_shadow(__maps.stack.addr, __maps.stack.size);
  __asan_shadow(__executable_start, _end - __executable_start);
  __asan_poison((void *)__veil("r", 0L), getauxval(AT_PAGESZ), kAsanNullPage);
  STRACE("    _    ____    _    _   _ ");
  STRACE("   / \\  / ___|  / \\  | \\ | |");
  STRACE("  / _ \\ \\___ \\ / _ \\ |  \\| |");
  STRACE(" / ___ \\ ___) / ___ \\| |\\  |");
  STRACE("/_/   \\_\\____/_/   \\_\\_| \\_|");
  STRACE("cosmopolitan memory safety module initialized");
}

__weak_reference(__asan_poison, __asan_poison_memory_region);
__weak_reference(__asan_unpoison, __asan_unpoison_memory_region);

#endif /* __SANITIZE_ADDRESS__ */
#endif /* __x86_64__ */
