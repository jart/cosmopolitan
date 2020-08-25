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
#include "tool/build/lib/machine.h"

static void InitRegisterBytePointers(struct Machine *m) {
  unsigned i, j, k;
  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 2; ++j) {
      for (k = 0; k < 8; ++k) {
        if (i) {
          m->beg[i << 4 | j << 3 | k] = m->reg[j << 3 | k];
        } else {
          m->beg[i << 4 | j << 3 | k] = &m->reg[k & 0b11][(k & 0b100) >> 2];
        }
      }
    }
  }
}

static void InitRegisterXmmPointers(struct Machine *m) {
  unsigned i, j;
  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 8; ++j) {
      m->veg[i << 3 | j] = m->xmm[i][j];
    }
  }
}

void InitMachine(struct Machine *m) {
  InitRegisterBytePointers(m);
  InitRegisterXmmPointers(m);
}
