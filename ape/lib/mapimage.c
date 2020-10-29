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
#include "ape/relocations.h"
#include "libc/runtime/runtime.h"

textreal static void __map_segment(uint64_t k, uint64_t a, uint64_t b) {
  uint64_t *e;
  for (; a < b; a += 0x1000) {
    e = getpagetableentry(IMAGE_BASE_VIRTUAL + a, 3, &g_pml4t, &g_ptsp_xlm);
    *e = (IMAGE_BASE_PHYSICAL + a) | k;
  }
}

textreal void __map_image(void) {
  pageunmap(0);
  __map_segment(PAGE_V | PAGE_U, 0, (uintptr_t)_etext - IMAGE_BASE_VIRTUAL);
  __map_segment(PAGE_V | PAGE_U | PAGE_RW | PAGE_XD,
                (uintptr_t)_etext - IMAGE_BASE_VIRTUAL,
                (uintptr_t)_end - IMAGE_BASE_VIRTUAL);
}
