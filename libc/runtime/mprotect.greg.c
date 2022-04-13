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
#include "libc/bits/asmflag.h"
#include "libc/bits/likely.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/memory.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

extern typeof(VirtualProtect) *const __imp_VirtualProtect __msabi;

#define ADDR(x) ((char *)((int64_t)((uint64_t)(x) << 32) >> 16))

/**
 * Modifies restrictions on virtual memory address range.
 *
 * @param addr needs to be 4kb aligned
 * @param prot can have PROT_{NONE,READ,WRITE,EXEC,GROWSDOWN,GROWSUP}
 * @return 0 on success, or -1 w/ errno
 * @see mmap()
 */
noasan noubsan privileged int mprotect(void *addr, size_t size, int prot) {
  bool cf;
  int64_t rc;
  unsigned i;
  uint32_t op;
  char *a, *b, *x, *y, *p;
  if (SupportsWindows() && (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC |
                                     PROT_GROWSDOWN | PROT_GROWSUP))) {
    rc = einval();  // unix checks prot before checking size
  } else if (!size) {
    return 0;  // make new technology consistent with unix
  } else if (UNLIKELY((intptr_t)addr & 4095)) {
    rc = einval();
  } else if (!IsWindows()) {
    asm volatile(CFLAG_ASM("clc\n\tsyscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(rc)
                 : "1"(__NR_mprotect), "D"(addr), "S"(size), "d"(prot)
                 : "rcx", "r11", "memory", "cc");
    if (cf) {
      errno = rc;
      rc = -1;
    } else if (rc > -4096ul) {
      errno = -rc;
      rc = -1;
    }
  } else {
    rc = 0;
    p = addr;
    i = FindMemoryInterval(&_mmi, (intptr_t)p >> 16);
    if (i == _mmi.i || (!i && p + size <= ADDR(_mmi.p[0].x))) {
      // memory isn't in memtrack
      // let's just trust the user then
      // it's probably part of the executable
      if (!VirtualProtect(addr, size, __prot2nt(prot, false), &op)) {
        rc = -1;
      }
    } else {
      // memory is in memtrack, so use memtrack, to do dimensioning
      // we unfortunately must do something similar to this for cow
      for (; i < _mmi.i; ++i) {
        x = ADDR(_mmi.p[i].x);
        y = x + _mmi.p[i].size;
        if ((x <= p && p < y) || (x < p + size && p + size <= y) ||
            (p < x && y < p + size)) {
          a = MIN(MAX(p, x), y);
          b = MAX(MIN(p + size, y), x);
          if (!VirtualProtect(a, b - a, __prot2nt(prot, _mmi.p[i].iscow),
                              &op)) {
            rc = -1;
            break;
          }
        } else {
          break;
        }
      }
    }
  }
  STRACE("mprotect(%p, %'zu, %s) → %d% m", addr, size, DescribeProtFlags(prot),
         rc);
  return rc;
}
