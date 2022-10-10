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
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

TEST(read, eof) {
  char b[8] = "hello";
  ASSERT_SYS(0, 3, creat("foo", 0644));
  ASSERT_SYS(0, 4, open("foo", O_RDONLY));
  ASSERT_SYS(0, 0, read(4, b, 8));
  ASSERT_SYS(0, 8, write(3, b, 8));
  ASSERT_SYS(0, 8, read(4, b, 8));
  ASSERT_SYS(0, 0, read(4, b, 8));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

////////////////////////////////////////////////////////////////////////////////

static long Read(long fd, void *buf, unsigned long size) {
  long ax, di, si, dx;
  asm volatile("syscall"
               : "=a"(ax), "=D"(di), "=S"(si), "=d"(dx)
               : "0"(__NR_read), "1"(fd), "2"(buf), "3"(size)
               : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  return ax;
}

BENCH(read, bench) {
  char buf[16];
  ASSERT_SYS(0, 3, open("/dev/zero", O_RDONLY));
  EZBENCH2("read", donothing, read(3, buf, 5));
  EZBENCH2("pread", donothing, pread(3, buf, 5, 0));
  EZBENCH2("readv₁", donothing, readv(3, &(struct iovec){buf, 5}, 1));
  EZBENCH2("readv₂", donothing,
           readv(3, (struct iovec[]){{buf, 1}, {buf + 1, 4}}, 2));
  EZBENCH2("preadv₁", donothing, preadv(3, &(struct iovec){buf, 5}, 1, 0));
  EZBENCH2("preadv₂", donothing,
           preadv(3, (struct iovec[]){{buf, 1}, {buf + 1, 4}}, 2, 0));
  EZBENCH2("sys_read", donothing, sys_read(3, buf, 5));
  EZBENCH2("sys_readv", donothing, sys_readv(3, &(struct iovec){buf, 5}, 1));
  EZBENCH2("Read", donothing, Read(3, buf, 5));
  EZBENCH2("Read", donothing, Read(3, buf, 5));
  ASSERT_SYS(0, 0, close(3));
}
