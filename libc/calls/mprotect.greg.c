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
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/memory.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/nr.h"

/**
 * Modifies restrictions on virtual memory address range.
 *
 * @param prot can have PROT_{NONE,READ,WRITE,EXEC,GROWSDOWN}
 * @return 0 on success, or -1 w/ errno
 * @see mmap()
 */
noasan noubsan privileged int mprotect(void *addr, size_t len, int prot) {
  bool cf;
  int64_t rc;
  uint32_t oldprot;
  char protbuf[4];
  if (!IsWindows()) {
    asm volatile(CFLAG_ASM("clc\n\tsyscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(rc)
                 : "1"(__NR_mprotect), "D"(addr), "S"(len), "d"(prot)
                 : "rcx", "r11", "memory", "cc");
    if (cf) {
      errno = rc;
      rc = -1;
    } else if (rc > -4096ul) {
      errno = -rc;
      rc = -1;
    }
  } else {
    if (VirtualProtect(addr, len, __prot2nt(prot, 0), &oldprot)) {
      rc = 0;
    } else {
      rc = __winerr();
    }
  }
  STRACE("mprotect(%p, %'zu, %s[%#x]) → %d% m", addr, len,
         DescribeProt(prot, protbuf), prot, rc);
  return rc;
}
