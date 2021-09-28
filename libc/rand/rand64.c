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
#include "libc/bits/xadd.h"
#include "libc/calls/calls.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"

static uint64_t thepool;

/**
 * Returns nondeterministic random data.
 *
 * This function automatically seeds itself on startup and reseeds
 * itself after fork() and vfork(). It takes about nanosecond to run.
 * That makes it much slower than vigna() and rand() but much faster
 * than rdrand() and rdseed().
 *
 * @see rdseed(), rdrand(), rand(), random(), rngset()
 * @note based on vigna's algorithm
 * @asyncsignalsafe
 * @vforksafe
 */
uint64_t rand64(void) {
  bool cf;
  register uint64_t t;
  if (X86_HAVE(RDSEED)) {
    asm volatile(CFLAG_ASM("rdseed\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(t)
                 : /* no inputs */
                 : "cc");
    if (cf) {
      thepool ^= t;
      return t;
    }
  }
  t = _xadd(&thepool, 0x9e3779b97f4a7c15);
  t ^= (getpid() * 0x1001111111110001ull + 0xdeaadead) >> 31;
  t = (t ^ (t >> 30)) * 0xbf58476d1ce4e5b9;
  t = (t ^ (t >> 27)) * 0x94d049bb133111eb;
  return t ^ (t >> 31);
}

static textstartup void rand64_init(int argc, char **argv, char **envp,
                                    intptr_t *auxv) {
  for (; auxv[0]; auxv += 2) {
    if (auxv[0] == AT_RANDOM) {
      thepool = READ64LE((const char *)auxv[1]);
      return;
    }
  }
  thepool = kStartTsc;
}

const void *const g_rand64_init[] initarray = {rand64_init};
