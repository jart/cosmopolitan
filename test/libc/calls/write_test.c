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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

static long Write(long fd, const void *data, unsigned long size) {
  long ax, di, si, dx;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"(__NR_write), "1"(fd), "2"(data), "3"(size)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  return ax;
}

BENCH(write, bench) {
  ASSERT_SYS(0, 3, open("/dev/null", O_WRONLY));
  EZBENCH2("write", donothing, write(3, "hello", 5));
  EZBENCH2("sys_write", donothing, sys_write(3, "hello", 5));
  EZBENCH2("sys_writev", donothing,
           sys_writev(3, &(struct iovec){"hello", 5}, 1));
  EZBENCH2("Write", donothing, Write(3, "hello", 5));
  EZBENCH2("Write", donothing, Write(3, "hello", 5));
  ASSERT_SYS(0, 0, close(3));
}
