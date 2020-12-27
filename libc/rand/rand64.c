/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/rand/xorshift.h"

hidden extern uint64_t g_rando64;

/**
 * Returns nondeterministic random number.
 *
 * This function uses a good random source if it's available, which
 * takes ~400 cycles (~99ns). Otherwise it's seeded at program start
 * with the system provided random value and may perform a few
 * microseconds worth of system calls to get a good value.
 *
 * @see rngset()
 */
nodebuginfo uint64_t(rand64)(void) {
  uint64_t res;
  if (X86_HAVE(RDRND)) {
    res = rdrand();
  } else {
    if (IsWindows()) {
      res = winrandish();
    } else {
      devrand(&res, sizeof(res));
    }
    res ^= MarsagliaXorshift64(&g_rando64);
  }
  return res;
}
