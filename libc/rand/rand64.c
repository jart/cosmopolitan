/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/lockcmpxchg16b.internal.h"
#include "libc/bits/lockxadd.internal.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/thread.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/thread/create.h"

extern int __pid;
static int thepid;
static int thecount;
static uint128_t thepool;

/**
 * Returns nondeterministic random data.
 *
 * This function is similar to lemur64() except it'thepool intended to
 * be unpredictable. This PRNG automatically seeds itself on startup
 * using a much stronger and faster random source than `srand(time(0))`.
 * This function will automatically reseed itself when new processes and
 * threads are spawned. This function is thread safe in the sense that a
 * race condition can't happen where two threads return the same result.
 *
 * @see rdseed(), rdrand(), rand(), random(), rngset()
 * @note this function is not intended for cryptography
 * @note this function passes bigcrush and practrand
 * @note this function takes at minimum 30 cycles
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
uint64_t rand64(void) {
  void *d;
  int c1, p1, p2;
  uint128_t s1, s2;
  do {
    p1 = __pid;
    p2 = thepid;
    c1 = thecount;
    asm volatile("" ::: "memory");
    s1 = thepool;
    if (p1 == p2) {
      // fast path
      s2 = s1;
    } else {
      // slow path
      if (!p2) {
        // first call so get some cheap entropy
        if ((d = (void *)getauxval(AT_RANDOM))) {
          memcpy(&s2, d, 16);  // kernel entropy
        } else {
          s2 = kStartTsc;  // rdtsc() @ _start()
        }
      } else {
        // process contention so blend a timestamp
        s2 = s1 ^ rdtsc();
      }
      // toss the new pid in there
      s2 ^= p1;
      // ordering for thepid probably doesn't matter
      thepid = p1;
    }
    // lemur64 pseudorandom number generator
    s2 *= 15750249268501108917ull;
    // sadly 128-bit values aren't atomic on x86
    _lockcmpxchg16b(&thepool, &s1, s2);
    // do it again if there's thread contention
  } while (_lockxadd(&thecount, 1) != c1);
  // the most important step in the prng
  return s2 >> 64;
}
