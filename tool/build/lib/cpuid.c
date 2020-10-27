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
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"

void OpCpuid(struct Machine *m, uint32_t rde) {
  uint32_t ax, bx, cx, dx;
  ax = 0;
  bx = 0;
  cx = 0;
  dx = 0;
  switch (Read32(m->ax)) {
    case 0:
    case 0x80000000:
      ax = 7;
      bx = 'G' | 'e' << 8 | 'n' << 16 | 'u' << 24;
      dx = 'i' | 'n' << 8 | 'e' << 16 | 'C' << 24;
      cx = 'o' | 's' << 8 | 'm' << 16 | 'o' << 24;
      break;
    case 1:
      cx |= 1 << 0;   // sse3
      cx |= 0 << 1;   // pclmulqdq
      cx |= 1 << 9;   // ssse3
      cx |= 1 << 23;  // popcnt
      cx |= 0 << 30;  // rdrnd
      cx |= 0 << 25;  // aes
      dx |= 1 << 0;   // fpu
      dx |= 1 << 4;   // tsc
      dx |= 1 << 6;   // pae
      dx |= 1 << 8;   // cmpxchg8b
      dx |= 1 << 15;  // cmov
      dx |= 1 << 19;  // clflush
      dx |= 1 << 23;  // mmx
      dx |= 1 << 25;  // sse
      dx |= 1 << 26;  // sse2
      break;
    case 7:
      bx |= 1 << 9;  // erms
      break;
    case 0x80000001:
      cx |= 1 << 0;   // lahf
      dx |= 1 << 11;  // syscall
      dx |= 1 << 29;  // long
      break;
    case 0x80000007:
      dx |= 1 << 8;  // invtsc
      break;
    default:
      break;
  }
  Write64(m->ax, ax);
  Write64(m->bx, bx);
  Write64(m->cx, cx);
  Write64(m->dx, dx);
}
