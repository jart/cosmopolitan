/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/strace.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/futex.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"

static const char *FormatFutexWakeResult(char buf[12], int ax) {
  if (ax >= 0) {
    FormatInt32(buf, ax);
    return buf;
  } else {
    return strerrno(-ax);
  }
}

privileged int _futex_wake(void *addr, int count) {
  int ax;
  bool cf;
  char buf[12];
  asm volatile(CFLAG_ASM("clc\n\t"
                         "syscall")
               : CFLAG_CONSTRAINT(cf), "=a"(ax)
               : "1"(__NR_futex), "D"(addr), "S"(FUTEX_WAKE), "d"(count)
               : "rcx", "r11", "memory");
  if (cf) ax = -ax;
  STRACE("futex(%p, FUTEX_WAKE, %d) → %s", addr, count,
         FormatFutexWakeResult(buf, ax));
  return ax;
}
