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
