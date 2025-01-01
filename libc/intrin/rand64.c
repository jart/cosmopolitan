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
#include "libc/intrin/getauxval.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static int _rand64_pid;
static unsigned __int128 _rand64_pool;
static pthread_mutex_t __rand64_lock_obj = PTHREAD_MUTEX_INITIALIZER;

void __rand64_lock(void) {
  _pthread_mutex_lock(&__rand64_lock_obj);
}

void __rand64_unlock(void) {
  _pthread_mutex_unlock(&__rand64_lock_obj);
}

void __rand64_wipe(void) {
  _pthread_mutex_wipe_np(&__rand64_lock_obj);
}

/**
 * Returns nondeterministic random data.
 *
 * This function is similar to lemur64() except that it doesn't produce
 * the same sequences of numbers each time your program is run. This is
 * the case even across forks and threads, whose sequences will differ.
 *
 * @see rdseed(), rdrand(), rand(), random(), rngset()
 * @note this function passes bigcrush and practrand
 */
uint64_t _rand64(void) {
  void *p;
  uint128_t s;
  __rand64_lock();
  if (__pid == _rand64_pid) {
    s = _rand64_pool;  // normal path
  } else {
    if (!_rand64_pid) {
      if (AT_RANDOM && (p = (void *)__getauxval(AT_RANDOM).value)) {
        // linux / freebsd kernel supplied entropy
        memcpy(&s, p, 16);
      } else {
        // otherwise initialize w/ cheap timestamp
        s = kStartTsc;
      }
    } else {
      // blend another timestamp on fork contention
      s = _rand64_pool ^ rdtsc();
    }
    // blend the pid on startup and fork contention
    s ^= __pid;
    _rand64_pid = __pid;
  }
  _rand64_pool = (s *= 15750249268501108917ull);  // lemur64
  __rand64_unlock();
  return s >> 64;
}
