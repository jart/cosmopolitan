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
#include "libc/atomic.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/getauxval.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/limits.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"

#define VDSO_TK_VER_CURR        1
#define VDSO_TH_ALGO_X86_TSC    1
#define VDSO_TH_ALGO_ARM_GENTIM 1

#define common_powers_of_two(a, b) ((~(a) & ((a) - 1) & ~(b) & ((b) - 1)) + 1)

struct bintime {
  uint64_t sec;
  uint64_t frac;
};

struct vdso_timehands {
  uint32_t th_algo;
  atomic_uint th_gen;
  uint64_t th_scale;
  uint32_t th_offset_count;
  uint32_t th_counter_mask;
  struct bintime th_offset;
  struct bintime th_boottime;
#if defined(__x86_64__)
  uint32_t th_x86_shift;
  uint32_t th_x86_hpet_idx;
  uint64_t th_x86_pvc_last_systime;
  uint8_t th_x86_pvc_stable_mask;
  uint8_t th_res[15];
#elif defined(__aarch64__)
  uint32_t th_physical;
  uint32_t th_res[7];
#else
#error "unsupported architecture"
#endif
};

struct vdso_timekeep {
  uint32_t tk_ver;
  uint32_t tk_enabled;
  atomic_uint tk_current;
  struct vdso_timehands tk_th[];
};

static inline int fls(int mask) {  // find last set bit
  return !mask ? 0 : CHAR_BIT * sizeof(mask) - __builtin_clz(mask);
}

static inline int flsl(long mask) {
  return !mask ? 0 : CHAR_BIT * sizeof(mask) - __builtin_clzl(mask);
}

static inline uint64_t utime64_scale32_floor(uint64_t x, uint32_t factor,
                                             uint32_t divisor) {
  const uint64_t rem = x % divisor;
  return x / divisor * factor + (rem * factor) / divisor;
}

static inline uint64_t utime64_scale64_floor(uint64_t x, uint64_t factor,
                                             uint64_t divisor) {
  const uint64_t gcd = common_powers_of_two(factor, divisor);
  return utime64_scale32_floor(x, factor / gcd, divisor / gcd);
}

static inline void bintime2timespec(const struct bintime *bt,
                                    struct timespec *ts) {
  ts->tv_sec = bt->sec;
  ts->tv_nsec = utime64_scale64_floor(bt->frac, 1000000000, 1ULL << 32) >> 32;
}

static inline void bintime_addx(struct bintime *bt, uint64_t x) {
  uint64_t u;
  u = bt->frac;
  bt->frac += x;
  if (u > bt->frac)
    bt->sec++;
}

static inline void bintime_add(struct bintime *bt, const struct bintime *bt2) {
  uint64_t u;
  u = bt->frac;
  bt->frac += bt2->frac;
  if (u > bt->frac)
    bt->sec++;
  bt->sec += bt2->sec;
}

static bool gettc(const struct vdso_timehands *th, unsigned *tc) {
#if defined(__x86_64__)
  if (th->th_algo != VDSO_TH_ALGO_X86_TSC)
    return false;
  if (!X86_HAVE(RDTSCP))
    return false;
  if (th->th_x86_shift > 0) {
    asm volatile("rdtscp\n\t"
                 "movl\t%1,%%ecx\n\t"
                 "shrd\t%%cl,%%edx,%0"
                 : "=&a"(*tc)
                 : "m"(th->th_x86_shift)
                 : "ecx", "edx");
  } else {
    asm volatile("rdtscp" : "=a"(*tc) : /* no inputs */ : "ecx", "edx");
  }
  return true;
#elif defined(__aarch64__)
  if (th->th_algo != VDSO_TH_ALGO_ARM_GENTIM)
    return false;
  asm volatile("isb" ::: "memory");
  if (!th->th_physical) {
    asm volatile("mrs\t%0,cntvct_el0" : "=r"(*tc));
  } else {
    asm volatile("mrs\t%0,cntpct_el0" : "=r"(*tc));
  }
  return true;
#else
#error "unsupported architecture"
#endif
}

static bool tc_delta(const struct vdso_timehands *th, unsigned *delta) {
  unsigned tc;
  if (gettc(th, &tc)) {
    *delta = (tc - th->th_offset_count) & th->th_counter_mask;
    return true;
  } else {
    return false;
  }
}

static bool binuptime(struct bintime *bt, struct vdso_timekeep *tk, bool abs) {
  uint64_t scale, x;
  uint32_t curr, gen;
  struct vdso_timehands *th;
  unsigned delta, scale_bits;
  do {
    if (!tk->tk_enabled)
      return false;
    curr = atomic_load_explicit(&tk->tk_current, memory_order_acquire);
    th = &tk->tk_th[curr];
    gen = atomic_load_explicit(&th->th_gen, memory_order_acquire);
    *bt = th->th_offset;
    if (!tc_delta(th, &delta))
      return false;
    scale = th->th_scale;
    scale_bits = flsl(scale);
    if (UNLIKELY(scale_bits + fls(delta) > 63)) {
      x = (scale >> 32) * delta;
      scale &= 0xffffffff;
      bt->sec += x >> 32;
      bintime_addx(bt, x << 32);
    }
    bintime_addx(bt, scale * delta);
    if (abs)
      bintime_add(bt, &th->th_boottime);
    // ensure load of th_offset is completed before the load of th_gen
    atomic_thread_fence(memory_order_acquire);
  } while (
      curr != atomic_load_explicit(&tk->tk_current, memory_order_relaxed) ||
      !gen || gen != atomic_load_explicit(&th->th_gen, memory_order_relaxed));
  return true;
}

static bool getnanouptime(struct bintime *bt, struct vdso_timekeep *tk) {
  struct vdso_timehands *th;
  uint32_t curr, gen;
  do {
    if (!tk->tk_enabled)
      return false;
    curr = atomic_load_explicit(&tk->tk_current, memory_order_acquire);
    th = &tk->tk_th[curr];
    gen = atomic_load_explicit(&th->th_gen, memory_order_acquire);
    *bt = th->th_offset;
    atomic_thread_fence(memory_order_acquire);
  } while (
      curr != atomic_load_explicit(&tk->tk_current, memory_order_relaxed) ||
      !gen || gen != atomic_load_explicit(&th->th_gen, memory_order_relaxed));
  return true;
}

static bool sys_clock_gettime_freebsd_vdso(int clock, struct timespec *ts) {
  struct bintime bt;
  static struct vdso_timekeep *tk;
  if (!tk) {
    struct AuxiliaryValue x;
    x = __getauxval(AT_TIMEKEEP);
    if (!x.isfound)
      return false;
    tk = (struct vdso_timekeep *)x.value;
  }
  if (tk->tk_ver != VDSO_TK_VER_CURR)
    return false;
  if (clock == CLOCK_MONOTONIC_COARSE) {
    if (!getnanouptime(&bt, tk))
      return false;
  } else if (clock == CLOCK_REALTIME ||  //
             clock == CLOCK_REALTIME_COARSE) {
    if (!binuptime(&bt, tk, true))
      return false;
  } else if (clock == CLOCK_BOOTTIME ||   //
             clock == CLOCK_MONOTONIC ||  //
             clock == CLOCK_MONOTONIC_RAW) {
    if (!binuptime(&bt, tk, false))
      return false;
  } else {
    return false;
  }
  bintime2timespec(&bt, ts);
  return true;
}

int sys_clock_gettime_freebsd(int clock, struct timespec *ts) {
  if (sys_clock_gettime_freebsd_vdso(clock, ts))
    return 0;
  return sys_clock_gettime(clock, ts);
}
