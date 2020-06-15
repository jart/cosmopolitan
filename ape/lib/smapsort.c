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
#include "ape/lib/pc.h"

textreal static unsigned smapcount(const struct SmapEntry *se) {
  unsigned i = 0;
  while (se[i].size) ++i;
  return i;
}

textreal static void smapsorter(size_t n, struct SmapEntry a[n]) {
  struct SmapEntry t;
  unsigned i, j;
  for (i = 1; i < n; ++i) {
    j = i;
    t = a[i];
    while (j > 0 && (intptr_t)t.addr - (intptr_t)a[j - 1].addr) {
      a[j] = a[j - 1];
      --j;
    }
    a[j] = t;
  }
}

/**
 * Sorts BIOS e820 memory map.
 */
textreal void smapsort(struct SmapEntry *smap) {
  smapsorter(smapcount(smap), smap);
}
