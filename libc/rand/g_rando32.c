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

hidden uint32_t g_rando32;

textstartup static void g_rando32_init() {
  intptr_t *auxvp;
  if (!IsXnu() && !IsWindows()) {
    asm("mov\t%%r15,%0" : "=r"(auxvp)); /* libc/crt/crt.S */
    for (; *auxvp; auxvp += 2) {
      if (*auxvp == AT_RANDOM) {
        uint8_t(*sysrandseed)[16] = (uint8_t(*)[16])auxvp[1];
        if (sysrandseed) g_rando32 ^= read32le(&(*sysrandseed)[8]);
        return;
      }
    }
  }
  g_rando32 ^= kMarsagliaXorshift32Seed;
  if (IsWindows()) {
    g_rando32 ^= winrandish();
  } else {
    devrand(&g_rando32, sizeof(g_rando32));
  }
}

const void *const g_rando32_ctor[] initarray = {g_rando32_init};
