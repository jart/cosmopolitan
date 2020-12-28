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
#define ShouldUseMsabiAttribute() 1
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/memory.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sysv/consts/nr.h"

extern __msabi typeof(VirtualProtect) *const __imp_VirtualProtect;

/**
 * Modifies restrictions on virtual memory address range.
 *
 * @param prot can have PROT_{NONE,READ,WRITE,EXEC,GROWSDOWN}
 * @return 0 on success, or -1 w/ errno
 * @see mmap()
 */
int mprotect(void *addr, uint64_t len, int prot) {
  bool cf;
  int64_t rc;
  uint32_t oldprot;
  if (!IsWindows()) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(rc)
                 : "1"(__NR_mprotect), "D"(addr), "S"(len), "d"(prot)
                 : "rcx", "r11", "memory", "cc");
    if (cf) {
      rc = -rc;
      rc = -1;
    } else if (rc > -4096ul) {
      errno = -rc;
      rc = -1;
    }
    return rc;
  } else {
    if (__imp_VirtualProtect(addr, len, prot2nt(prot, 0), &oldprot)) {
      return 0;
    } else {
      return __winerr();
    }
  }
}
