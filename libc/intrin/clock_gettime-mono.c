/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timeval.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/nexgen32e/rdtsc.h"

/**
 * @fileoverview Monotonic clock polyfill.
 *
 * This isn't quite `CLOCK_MONOTONIC` and isn't quite `CLOCK_BOOTTIME`
 * either; however it is fast and almost always goes in one direction.
 *
 * Intel architecture guarantees that a mapping exists between rdtsc &
 * nanoseconds only if the cpu advertises invariant timestamps support
 * however this shouldn't matter for a monotonic clock since we really
 * don't want to have it tick while suspended. Sadly that shall happen
 * since nearly all x86 microprocessors support invariant tsc which is
 * why we try to avoid this fallback when possible.
 */

int sys_sysctl(int *, unsigned, void *, size_t *, void *, size_t) libcesque;

static struct {
  atomic_uint once;
  unsigned long base;
  struct timespec boot;
} g_mono;

static struct timespec get_boot_time_xnu(void) {
  struct timeval t;
  size_t n = sizeof(t);
  int mib[] = {1 /* CTL_KERN */, 21 /* KERN_BOOTTIME */};
  if (sys_sysctl(mib, 2, &t, &n, 0, 0) == -1)
    __builtin_trap();
  return timeval_totimespec(t);
}

static void sys_clock_gettime_mono_init(void) {
  g_mono.base = rdtsc();
  if (IsXnu()) {
    g_mono.boot = get_boot_time_xnu();
  } else {
    __builtin_trap();
  }
}

int sys_clock_gettime_mono(struct timespec *time) {
  uint64_t nanos;
  uint64_t cycles;
  cosmo_once(&g_mono.once, sys_clock_gettime_mono_init);
  // ensure we get the full 64 bits of counting, which avoids wraparound
  cycles = rdtsc() - g_mono.base;
  // this is a crude approximation, that's worked reasonably well so far
  // only the kernel knows the actual mapping between rdtsc and nanosecs
  // which we could attempt to measure ourselves using clock_gettime but
  // we'd need to impose 100 ms of startup latency for a guess this good
  nanos = cycles / 3;
  *time = timespec_add(g_mono.boot, timespec_fromnanos(nanos));
  return 0;
}
