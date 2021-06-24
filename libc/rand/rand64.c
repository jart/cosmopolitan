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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

static uint64_t thepool;

/**
 * Returns nondeterministic random data.
 *
 * This random number seed generator blends information from:
 *
 * - rdtsc() hardware clock
 * - getpid() process identifier
 * - getauxval(AT_RANDOM) on Linux
 *
 * It's 100% guaranteed to not hard block the system.
 *
 * @see rngset(), getrandom()
 * @asyncsignalsafe
 * @vforksafe
 */
uint64_t rand64(void) {
  register uint64_t t;
  t = thepool;
  t ^= getpid() * 11400714819643198487ull + 123456789123456789;
  t ^= t << 13;
  t ^= t >> 7;
  t ^= t << 17;
  t ^= rdtsc() * 11400714819643198487ull + 123456789123456789;
  t ^= t << 13;
  t ^= t >> 7;
  t ^= t << 17;
  thepool ^= t;
  return t;
}

static textstartup void rand64_init(int argc, char **argv, char **envp,
                                    intptr_t *auxv) {
  uint64_t t;
  t = kStartTsc * 88172645463325252 + 123456789123456789;
  if (AT_RANDOM) {
    for (; auxv[0]; auxv += 2) {
      if (auxv[0] == AT_RANDOM) {
        t ^= READ64LE((const char *)auxv[1]);
        break;
      }
    }
  }
  thepool = t;
}

const void *const g_rand64_init[] initarray = {rand64_init};
