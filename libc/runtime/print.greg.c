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
#include "libc/dce.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/missioncritical.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/nr.h"

static privileged void __print$nt(const void *data, size_t len) {
  int64_t hand;
  uint32_t wrote;
  char xmm[256] aligned(16);
  savexmm(&xmm[128]);
  hand = __imp_GetStdHandle(kNtStdErrorHandle);
  __imp_WriteFile(hand, data, len, &wrote, NULL);
  __imp_FlushFileBuffers(hand);
  loadxmm(&xmm[128]);
}

/**
 * Prints string, by any means necessary.
 *
 * This function offers a subset of write(STDERR_FILENO) functionality.
 * It's designed to work even when the runtime hasn't initialized, e.g.
 * before _init() gets called.
 *
 * @param len can be computed w/ tinystrlen()
 * @clob nothing except flags
 * @see PRINT()
 */
privileged interruptfn void __print(const void *data, size_t len) {
  int64_t ax, ordinal;
  LOAD_DEFAULT_RBX();
  if (NT_HAVE_IMPORT(__imp_WriteFile)) {
    __print$nt(data, len);
  } else {
    ordinal = __NR_write > 0 ? __NR_write : IsXnu() ? 0x2000004 : 4;
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(ordinal), "D"(STDERR_FILENO), "S"(data), "d"(len)
                 : "rcx", "r11", "memory", "cc");
    if (ax == -1 && !hostos && !__NR_write) {
      asm volatile("syscall"
                   : "=a"(ax)
                   : "0"(ordinal), "D"(STDERR_FILENO), "S"(data), "d"(len)
                   : "rcx", "r11", "memory", "cc");
    }
  }
  RESTORE_RBX();
}
