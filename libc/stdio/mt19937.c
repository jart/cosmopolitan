/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│                                                                              │
│  Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,                  │
│  All rights reserved.                                                        │
│                                                                              │
│  Redistribution and use in source and binary forms, with or without          │
│  modification, are permitted provided that the following conditions          │
│  are met:                                                                    │
│                                                                              │
│    1. Redistributions of source code must retain the above copyright         │
│       notice, this list of conditions and the following disclaimer.          │
│                                                                              │
│    2. Redistributions in binary form must reproduce the above                │
│       copyright notice, this list of conditions and the following            │
│       disclaimer in the documentation and/or other materials                 │
│       provided with the distribution.                                        │
│                                                                              │
│    3. The names of its contributors may not be used to endorse or            │
│       promote products derived from this software without specific           │
│       prior written permission.                                              │
│                                                                              │
│  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS         │
│  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT           │
│  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR       │
│  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT        │
│  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       │
│  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT            │
│  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,       │
│  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY       │
│  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT         │
│  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE       │
│  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.        │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/likely.h"
#include "libc/macros.internal.h"
#include "libc/stdio/rand.h"

asm(".ident\t\"\\n\\n\
mt19937 (BSD-3)\\n\
Copyright 1997-2004 Makoto Matsumoto and Takuji Nishimura\"");
asm(".include \"libc/disclaimer.inc\"");

/*
 * A C-program for MT19937-64 (2004/9/29 version).
 * Coded by Takuji Nishimura and Makoto Matsumoto.
 *
 * This is a 64-bit version of Mersenne Twister pseudorandom number
 * generator.
 *
 * Before using, initialize the state by using init_genrand64(seed)
 * or init_by_array64(init_key, key_length).
 *
 * References:
 * T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
 *   ACM Transactions on Modeling and
 *   Computer Simulation 10. (2000) 348--357.
 * M. Matsumoto and T. Nishimura,
 *   ``Mersenne Twister: a 623-dimensionally equidistributed
 *     uniform pseudorandom number generator''
 *   ACM Transactions on Modeling and
 *   Computer Simulation 8. (Jan. 1998) 3--30.
 *
 * Any feedback is very welcome.
 * http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 * email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
 */

#define NN 312
#define MM 156
#define LM 0x7fffffff
#define UM 0xffffffff80000000

static int mti = NN + 1;
static uint64_t mt[NN];
static const uint64_t mag01[2] = {0, 0xb5026f5aa96619e9};

/**
 * Initializes mt[NN] with small seed value.
 *
 * @see mt19937(), Smt19937()
 */
void _smt19937(uint64_t seed) {
  mt[0] = seed;
  for (mti = 1; mti < NN; mti++) {
    mt[mti] = 0x5851f42d4c957f2d * (mt[mti - 1] ^ (mt[mti - 1] >> 62)) + mti;
  }
}

/**
 * Initializes mt[NN] with array.
 *
 * @param K is the array for initializing keys
 * @param n is its length
 * @see mt19937(), smt19937()
 */
void _Smt19937(uint64_t K[], size_t n) {
  size_t i, j, k;
  _smt19937(19650218);
  for (i = 1, j = 0, k = MAX(NN, n); k; k--) {
    mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 62)) * 0x369dea0f31a53f85)) +
            K[j] + j;
    if (++i >= NN) mt[0] = mt[NN - 1], i = 1;
    if (++j >= n) j = 0;
  }
  for (k = NN - 1; k; k--) {
    mt[i] =
        (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 62)) * 0x27bb2ee687b0b0fd)) - i;
    if (++i >= NN) mt[0] = mt[NN - 1], i = 1;
  }
  mt[0] = 0x8000000000000000; /* assures non-zero initial array */
}

/**
 * Generates random integer on [0, 2^64)-interval.
 *
 * This uses the Mersenne Twister pseudorandom number generator.
 *
 * @see smt19937(), Smt19937()
 */
uint64_t _mt19937(void) {
  int i;
  uint64_t x;
  if (mti >= NN) {
    if (mti == NN + 1) _smt19937(5489);
    for (i = 0; i < NN - MM; i++) {
      x = (mt[i] & UM) | (mt[i + 1] & LM);
      mt[i] = mt[i + MM] ^ (x >> 1) ^ mag01[x & 1];
    }
    for (; i < NN - 1; i++) {
      x = (mt[i] & UM) | (mt[i + 1] & LM);
      mt[i] = mt[i + (MM - NN)] ^ (x >> 1) ^ mag01[x & 1];
    }
    x = (mt[NN - 1] & UM) | (mt[0] & LM);
    mt[NN - 1] = mt[MM - 1] ^ (x >> 1) ^ mag01[x & 1];
    mti = 0;
  }
  x = mt[mti++];
  x ^= (x >> 29) & 0x5555555555555555;
  x ^= (x << 17) & 0x71d67fffeda60000;
  x ^= (x << 37) & 0xfff7eee000000000;
  x ^= (x >> 43);
  return x;
}
