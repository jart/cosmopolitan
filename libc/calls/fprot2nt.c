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
#include "libc/calls/internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/memory.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define HAS(X, BITS) (((X) & (BITS)) == (BITS))

/**
 * Converts System Five memory protection flags to Windows NT, Part 2.
 * @see libc/sysv/consts.sh
 */
uint32_t fprot2nt(int prot, int flags) {
  return (HAS(prot, PROT_READ) ? kNtFileMapRead : 0) |
         (HAS(prot, PROT_WRITE) ? kNtFileMapWrite : 0) |
         (HAS(prot, PROT_EXEC) ? kNtFileMapExecute : 0) |
         (HAS(flags, kNtSecLargePages) ? kNtFileMapLargePages : 0) |
         (HAS(flags, kNtSecReserve) ? kNtFileMapReserve : 0) |
         ((HAS(flags, MAP_PRIVATE) && HAS(prot, PROT_READ) &&
           HAS(prot, PROT_WRITE))
              ? kNtFileMapCopy
              : 0);
}
