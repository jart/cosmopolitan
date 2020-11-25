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
#include "libc/bits/pushpop.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/nr.h"

#define STACK_SMASH_MESSAGE "stack smashed\n"

/**
 * Aborts program under enemy fire to avoid being taken alive.
 */
void __stack_chk_fail(void) {
  size_t len;
  const char *msg;
  int64_t ax, cx, si;
  if (!IsWindows()) {
    msg = STACK_SMASH_MESSAGE;
    len = pushpop(sizeof(STACK_SMASH_MESSAGE) - 1);
    if (!IsMetal()) {
      asm volatile("syscall"
                   : "=a"(ax)
                   : "0"(__NR_write), "D"(pushpop(STDERR_FILENO)), "S"(msg),
                     "d"(len)
                   : "rcx", "r11", "cc", "memory");
      asm volatile("syscall"
                   : "=a"(ax)
                   : "0"(__NR_exit), "D"(pushpop(23))
                   : "rcx", "r11", "cc", "memory");
    }
    asm volatile("rep outsb"
                 : "=S"(si), "=c"(cx)
                 : "0"(msg), "1"(len), "d"(0x3F8 /* COM1 */)
                 : "memory");
    triplf();
  }
  if (NtGetVersion() < kNtVersionFuture) {
    do {
      asm volatile(
          "syscall"
          : "=a"(ax), "=c"(cx)
          : "0"(NtGetVersion() < kNtVersionWindows8
                    ? 0x0029
                    : NtGetVersion() < kNtVersionWindows81
                          ? 0x002a
                          : NtGetVersion() < kNtVersionWindows10 ? 0x002b
                                                                 : 0x002c),
            "1"(pushpop(-1L)), "d"(42)
          : "r11", "cc", "memory");
    } while (!ax);
  }
  for (;;) {
    TerminateProcess(GetCurrentProcess(), 42);
  }
}
