/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/dce.h"
#include "libc/rand/rand.h"
#include "libc/rand/xorshift.h"
#include "libc/sysv/consts/auxv.h"

hidden uint64_t g_rando64;

textstartup static void g_rando64_init() {
  intptr_t *auxvp;
  if (!IsXnu() && !IsWindows()) {
    asm("mov\t%%r15,%0" : "=r"(auxvp)); /* libc/crt/crt.S */
    for (; auxvp[0]; auxvp += 2) {
      if (auxvp[0] == AT_RANDOM) {
        uint8_t(*sysrandseed)[16] = (uint8_t(*)[16])auxvp[1];
        if (sysrandseed) g_rando64 ^= read64le(&(*sysrandseed)[0]);
        return;
      }
    }
  }
  g_rando64 ^= kMarsagliaXorshift64Seed;
  if (IsWindows()) {
    g_rando64 ^= winrandish();
  } else {
    devrand(&g_rando64, sizeof(g_rando64));
  }
}

const void *const g_rando64_ctor[] initarray = {g_rando64_init};
