/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

#define IP(X)      (intptr_t)(X)
#define VIP(X)     (void *)IP(X)

void *__zipos_mmap(void *addr, size_t size, int prot, int flags, struct ZiposHandle *h, int64_t off) {
  if (VERY_UNLIKELY(!!(flags & MAP_ANONYMOUS))) {
    STRACE("fd anonymous mismatch");
    return VIP(einval());
  }

  if (VERY_UNLIKELY(!(!!(prot & PROT_WRITE) ^ !!(flags & MAP_SHARED)))) {
    STRACE("PROT_WRITE and MAP_SHARED on zipos");
    return VIP(eacces());
  }

  const int tempProt = !IsXnu() ? prot | PROT_WRITE : PROT_WRITE;
  void *outAddr = mmap(addr, size, tempProt, flags | MAP_ANONYMOUS, -1, 0);
  if (outAddr == MAP_FAILED) {
    return MAP_FAILED;
  }
  const int64_t beforeOffset = __zipos_lseek(h, 0, SEEK_CUR);
  if ((beforeOffset == -1) || (__zipos_read(h, &(struct iovec){outAddr, size}, 1, off) == -1)) {
    munmap(outAddr, size);
    return MAP_FAILED;
  }
  __zipos_lseek(h, beforeOffset, SEEK_SET);
  if(prot != tempProt) {
    mprotect(outAddr, size, prot);
  }
  return outAddr;
}
