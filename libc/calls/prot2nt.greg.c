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
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#define HAS(X, BITS) (((X) & (BITS)) == (BITS))

/**
 * Converts System Five memory protection flags to Windows NT, Part 1.
 * @see libc/sysv/consts.sh
 */
privileged uint32_t prot2nt(int prot, int flags) {
  return (HAS(prot, PROT_READ | PROT_WRITE | PROT_EXEC)
              ? (HAS(flags, MAP_SHARED) || HAS(flags, MAP_ANONYMOUS))
                    ? kNtPageExecuteReadwrite
                    : kNtPageExecuteWritecopy
              : HAS(prot, PROT_READ | PROT_WRITE)
                    ? (HAS(flags, MAP_SHARED) || HAS(flags, MAP_ANONYMOUS))
                          ? kNtPageReadwrite
                          : kNtPageWritecopy
                    : HAS(prot, PROT_READ | PROT_EXEC)
                          ? kNtPageExecuteRead
                          : HAS(prot, PROT_EXEC)
                                ? kNtPageExecute
                                : HAS(prot, PROT_READ) ? kNtPageReadonly
                                                       : kNtPageNoaccess) |
         ((prot | flags) &
          (kNtSecReserve | kNtSecCommit | kNtSecImage | kNtSecImageNoExecute |
           kNtSecLargePages | kNtSecNocache | kNtSecWritecombine));
}
