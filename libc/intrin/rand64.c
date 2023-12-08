/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/getauxval.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static struct {
  int thepid;
  uint128_t thepool;
  pthread_spinlock_t lock;
} g_rand64;

/**
 * Returns nondeterministic random data.
 *
 * This function is similar to lemur64() except that it doesn't produce
 * the same sequences of numbers each time your program is run. This is
 * the case even across forks and threads, whose sequences will differ.
 *
 * @see rdseed(), rdrand(), rand(), random(), rngset()
 * @note this function takes 5 cycles (30 if `__threaded`)
 * @note this function is not intended for cryptography
 * @note this function passes bigcrush and practrand
 */
uint64_t _rand64(void) {
  void *p;
  uint128_t s;
  if (__threaded) pthread_spin_lock(&g_rand64.lock);
  if (__pid == g_rand64.thepid) {
    s = g_rand64.thepool;  // normal path
  } else {
    if (!g_rand64.thepid) {
      if (AT_RANDOM && (p = (void *)__getauxval(AT_RANDOM).value)) {
        // linux / freebsd kernel supplied entropy
        memcpy(&s, p, 16);
      } else {
        // otherwise initialize w/ cheap timestamp
        s = kStartTsc;
      }
    } else {
      // blend another timestamp on fork contention
      s = g_rand64.thepool ^ rdtsc();
    }
    // blend the pid on startup and fork contention
    s ^= __pid;
    g_rand64.thepid = __pid;
  }
  g_rand64.thepool = (s *= 15750249268501108917ull);  // lemur64
  pthread_spin_unlock(&g_rand64.lock);
  return s >> 64;
}
