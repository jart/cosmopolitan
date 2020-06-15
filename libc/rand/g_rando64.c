/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/dce.h"
#include "libc/rand/rand.h"
#include "libc/rand/xorshift.h"
#include "libc/sysv/consts/auxv.h"

hidden uint64_t g_rando64;

textstartup static void g_rando64_init() {
  register intptr_t *auxv asm("r15"); /* @see libc/crt/crt.S */
  asm volatile("" : "=r"(auxv));
  if (!IsXnu() && !IsWindows()) {
    for (intptr_t *auxvp = auxv; auxvp[0]; auxvp += 2) {
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
