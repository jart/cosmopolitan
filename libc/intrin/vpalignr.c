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
#include "libc/intrin/palignr.h"

/**
 * Shifts and concatenates xmm registers.
 *
 * @param i may be a non-literal
 * @see palignr()
 * @mayalias
 */
void pvalignr(void *p, const void *prev, const void *next, size_t i) {
  switch (i) {
    case 0:
      palignr(p, prev, next, 0);
      break;
    case 1:
      palignr(p, prev, next, 1);
      break;
    case 2:
      palignr(p, prev, next, 2);
      break;
    case 3:
      palignr(p, prev, next, 3);
      break;
    case 4:
      palignr(p, prev, next, 4);
      break;
    case 5:
      palignr(p, prev, next, 5);
      break;
    case 6:
      palignr(p, prev, next, 6);
      break;
    case 7:
      palignr(p, prev, next, 7);
      break;
    case 8:
      palignr(p, prev, next, 8);
      break;
    case 9:
      palignr(p, prev, next, 9);
      break;
    case 10:
      palignr(p, prev, next, 10);
      break;
    case 11:
      palignr(p, prev, next, 11);
      break;
    case 12:
      palignr(p, prev, next, 12);
      break;
    case 13:
      palignr(p, prev, next, 13);
      break;
    case 14:
      palignr(p, prev, next, 14);
      break;
    case 15:
      palignr(p, prev, next, 15);
      break;
    case 16:
      palignr(p, prev, next, 16);
      break;
    case 17:
      palignr(p, prev, next, 17);
      break;
    case 18:
      palignr(p, prev, next, 18);
      break;
    case 19:
      palignr(p, prev, next, 19);
      break;
    case 20:
      palignr(p, prev, next, 20);
      break;
    case 21:
      palignr(p, prev, next, 21);
      break;
    case 22:
      palignr(p, prev, next, 22);
      break;
    case 23:
      palignr(p, prev, next, 23);
      break;
    case 24:
      palignr(p, prev, next, 24);
      break;
    case 25:
      palignr(p, prev, next, 25);
      break;
    case 26:
      palignr(p, prev, next, 26);
      break;
    case 27:
      palignr(p, prev, next, 27);
      break;
    case 28:
      palignr(p, prev, next, 28);
      break;
    case 29:
      palignr(p, prev, next, 29);
      break;
    case 30:
      palignr(p, prev, next, 30);
      break;
    case 31:
      palignr(p, prev, next, 31);
      break;
    default:
      palignr(p, prev, next, 32);
      break;
  }
}
