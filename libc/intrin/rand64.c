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
#include "libc/nexgen32e/rdtsc.h"
#include "libc/stdio/rand.h"
#include "libc/sysv/pib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

alignas(64) static struct {
  pthread_mutex_t mtx;
  bool once;
  union {
    uint128_t pool;
    uint64_t lo;
  };
} _rand64_state = {
    PTHREAD_MUTEX_INITIALIZER,
};

static uint64_t _rand64_hash(uint64_t k) {
  /* MurmurHash3 was written by Austin Appleby, and is placed in the
     public domain. See fmix64() in MurmurHash3.cpp */
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccd;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53;
  k ^= k >> 33;
  return k;
}

static void _rand64_blend(uint64_t x) {
  _rand64_state.lo ^= _rand64_hash(x);
}

static void _rand64_reseed(void) {
  _rand64_blend(rdtsc());
  _rand64_blend(__get_pib()->pid);
}

void __rand64_lock(void) {
  if (__isthreaded >= 2)
    pthread_mutex_lock(&_rand64_state.mtx);
}

void __rand64_unlock(void) {
  if (__isthreaded >= 2)
    pthread_mutex_unlock(&_rand64_state.mtx);
}

void __rand64_wipe(void) {
  pthread_mutex_wipe_np(&_rand64_state.mtx);
  _rand64_reseed();
}

dontinline static void _rand64_init(void) {
  _rand64_reseed();
  _rand64_state.once = true;
}

/**
 * Returns nondeterministic random data.
 *
 * This function is similar to lemur64() except that it doesn't produce
 * the same sequences of numbers each time your program is run. This is
 * the case even across forks and threads, whose sequences will differ.
 *
 * This function fully passes bigcrush and practrand. However it wasn't
 * written by a crypto expert. So if you want a nondeterministic random
 * number generator that's cryptographically secure, and nearly as fast
 * as this function, consider using arc4random().
 *
 * What makes a random number generator nondeterministic is that it can
 * be counted upon to generate sequences that are different each time a
 * program executes. Secondly, it needs to be thread safe, in the sense
 * that two threads generating generating a random number at once won't
 * have much chance of receiving the same one. Another important aspect
 * of nondeterministic random is fork() safety, i.e. it should not make
 * the same numbers in both the parent and child process. To meet those
 * requirements you need either _rand64(), arc4random(), getrandom() or
 * getentropy(). The general rule of thumb is _rand64() goes 10x faster
 * than arc4random() which goes 10x faster than getentropy().
 *
 * See test/libc/intrin/rand64_test.c for demonstration of this safety.
 *
 * The downside to this random number generator is it uses one lock, so
 * it isn't scalable across many threads. Consider arc4random() because
 * it uses cosmo_shard() to create a separate chacha state per cpu core
 * although getentropy() and getrandom() should be thread scalable too.
 *
 * If you are not sure, consider arc4random() and arc4random_uniform().
 */
uint64_t _rand64(void) {
  uint128_t s;
  __rand64_lock();
  if (!_rand64_state.once)
    _rand64_init();
  s = _rand64_state.pool;
  _rand64_state.pool = (s *= 15750249268501108917ull);  // lemur64
  __rand64_unlock();
  return s >> 64;
}
