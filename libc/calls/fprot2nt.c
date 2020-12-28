/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
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
