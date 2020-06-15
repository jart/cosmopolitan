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
#include "libc/elf/def.h"
#include "tool/build/lib/elfwriter.h"

void elfwriter_cargoculting(struct ElfWriter *elf) {
  elfwriter_startsection(elf, "", SHT_NULL, 0);
  elfwriter_startsection(elf, ".text", SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR);
  elfwriter_finishsection(elf);
  elfwriter_startsection(elf, ".data", SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
  elfwriter_finishsection(elf);
  elfwriter_startsection(elf, ".bss", SHT_NOBITS, SHF_ALLOC | SHF_WRITE);
  elfwriter_finishsection(elf);
}
