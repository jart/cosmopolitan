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
#include "ape/config.h"
#include "ape/lib/pc.h"
#include "libc/bits/bits.h"
#include "libc/macros.h"

/**
 * Virtualizes physical memory.
 *
 * This function removes memory holes (discovered by e820() earlier) and
 * creates the illusion of flat contiguous memory for as much RAM as the
 * BIOS reports usable. Memory is safe to use and remap afterwards.
 *
 * @see ape/ape.S
 */
textreal void flattenhighmemory(struct SmapEntry *e820, struct PageTable *pml4t,
                                uint64_t *ptsp) {
  uint64_t *entry, paddr, vaddr;
  struct SmapEntry *smap, *hole;
  for (smap = hole = e820, vaddr = IMAGE_BASE_VIRTUAL; smap->size; ++smap) {
    while (smap->size && smap->type != kMemoryUsable) smap++;
    paddr = ROUNDUP(MAX(IMAGE_BASE_PHYSICAL, smap->addr), PAGESIZE);
    while (paddr < ROUNDDOWN(smap->addr + smap->size, PAGESIZE)) {
      while (hole->size &&
             (hole->type == kMemoryUsable || hole->addr + hole->size < paddr)) {
        hole++;
      }
      if (paddr >= hole->addr && paddr < hole->addr + hole->size) {
        paddr = ROUNDUP(hole->addr + hole->size, PAGESIZE);
      } else {
        entry = __getpagetableentry(vaddr, 3, pml4t, ptsp);
        *entry = paddr | PAGE_V | PAGE_RW;
        vaddr += 0x1000;
        paddr += 0x1000;
      }
    }
  }
}
