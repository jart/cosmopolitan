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
#include "libc/elf/elf.h"
#include "libc/limits.h"

void GetElfVirtualAddressRange(const Elf64_Ehdr *elf, size_t elfsize,
                               intptr_t *out_start, intptr_t *out_end) {
  unsigned i;
  Elf64_Phdr *phdr;
  intptr_t start, end, pstart, pend;
  start = INTPTR_MAX;
  end = 0;
  for (i = 0; i < elf->e_phnum; ++i) {
    phdr = GetElfSegmentHeaderAddress(elf, elfsize, i);
    if (phdr->p_type != PT_LOAD) continue;
    pstart = phdr->p_vaddr;
    pend = phdr->p_vaddr + phdr->p_memsz;
    if (pstart < start) start = pstart;
    if (pend > end) end = pend;
  }
  if (out_start) *out_start = start;
  if (out_end) *out_end = end;
}
