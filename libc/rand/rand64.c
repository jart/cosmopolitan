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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/intrin/spinlock.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/thread.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/thread/create.h"

extern int __pid;
static int thepid;
static uint128_t thepool;
static cthread_spinlock_t rand64_lock;

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
 * @note this function takes at minimum 15 cycles
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
uint64_t rand64(void) {
  void *p;
  uint128_t s;
  cthread_spinlock(&rand64_lock);
  if (__pid == thepid) {
    s = thepool;  // normal path
  } else {
    if (!thepid) {
      if (AT_RANDOM && (p = (void *)getauxval(AT_RANDOM))) {
        // linux / freebsd kernel supplied entropy
        memcpy(&s, p, 16);
      } else {
        // otherwise initialize w/ cheap timestamp
        s = kStartTsc;
      }
    } else {
      // blend another timestamp on fork contention
      s = thepool ^ rdtsc();
    }
    // blend the pid on startup and fork contention
    s ^= __pid;
    thepid = __pid;
  }
  thepool = (s *= 15750249268501108917ull);  // lemur64
  cthread_spunlock(&rand64_lock);
  return s >> 64;
}
