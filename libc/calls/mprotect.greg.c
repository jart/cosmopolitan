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
#define ShouldUseMsabiAttribute() 1
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/memory.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sysv/consts/nr.h"

/**
 * Modifies restrictions on virtual memory address range.
 *
 * @param prot can have PROT_{NONE,READ,WRITE,EXEC,GROWSDOWN}
 * @return 0 on success, or -1 w/ errno
 * @see mmap()
 */
int mprotect(void *addr, uint64_t len, int prot) {
  extern __msabi typeof(VirtualProtect) *const __imp_VirtualProtect;
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
      return winerr();
    }
  }
}
