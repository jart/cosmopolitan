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
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#include "libc/thread/wait.h"

int cthread_memory_wait32(uint32_t* addr, uint32_t val,
                          const struct timespec* timeout) {
  if (__NR_futex != 0xfff) {
    int flags = FUTEX_WAIT;
    int rc;
    register struct timespec* timeout_ asm("r10") = timeout;
    asm volatile("syscall"
                 : "=a"(rc)
                 : "0"(__NR_futex), "D"(addr), "S"(flags), "d"(val),
                   "r"(timeout_)
                 : "rcx", "r11", "cc", "memory");
    return rc;
  }
  return -1;
}

int cthread_memory_wake32(uint32_t* addr, int n) {
  if (__NR_futex != 0xfff) {
    int flags = FUTEX_WAKE;
    int rc;
    asm volatile("syscall"
                 : "=a"(rc)
                 : "0"(__NR_futex), "D"(addr), "S"(flags), "d"(n)
                 : "rcx", "r11", "cc", "memory");
    return rc;
  }
  return -1;
}
