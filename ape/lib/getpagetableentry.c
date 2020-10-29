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
#include "libc/assert.h"

textreal static uint64_t pushpagetable(uint64_t *ptsp) {
  return (*ptsp -= PAGESIZE) | PAGE_V | PAGE_RW;
}

textreal uint64_t *getpagetableentry(int64_t vaddr, unsigned depth,
                                     struct PageTable *pml4t, uint64_t *ptsp) {
  uint64_t *entry;
  unsigned char shift;
  assert(depth <= 3);
  assert(!(*ptsp & 0xfff));
  assert(!((uintptr_t)pml4t & 0xfff));
  shift = 39;
  for (;;) {
    entry = &pml4t->p[(vaddr >> shift) & 511];
    if (!depth--) return entry;
    shift -= 9;
    if (!*entry) *entry = pushpagetable(ptsp);
    pml4t = (void *)(*entry & PAGE_TA);
  }
}
