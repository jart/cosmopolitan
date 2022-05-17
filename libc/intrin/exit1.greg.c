/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/intrin/setjmp.internal.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"

/**
 * Terminates thread with raw system call.
 *
 * @param rc only works on Linux and Windows
 * @see cthread_exit()
 * @threadsafe
 * @noreturn
 */
privileged wontreturn void _Exit1(int rc) {
  struct WinThread *wt;
  STRACE("_Exit1(%d)", rc);
  if (!IsWindows() && !IsMetal()) {
    register long r10 asm("r10") = 0;
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_exit), "D"(IsLinux() ? rc : 0), "S"(0), "d"(0),
                   "r"(r10)
                 : "rcx", "r11", "memory");
  } else if (IsWindows()) {
    ExitThread(rc);
  }
  for (;;) {
    asm("ud2");
  }
}
