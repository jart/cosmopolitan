/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define THREADS 5
#define FDS     10

void *Torturer(void *arg) {
  int i;
  int fd[FDS];
  for (i = 0; i < FDS; ++i) {
    ASSERT_NE(-1, (fd[i] = open("/dev/null", O_WRONLY)));
  }
  for (i = 0; i < FDS; ++i) {
    ASSERT_EQ(2, write(fd[i], "hi", 2));
    if (!vfork()) _Exit(0);
    wait(0);
  }
  for (i = 0; i < FDS; ++i) {
    ASSERT_NE(-1, close(fd[i]));
  }
  return 0;
}

TEST(fds_torture, test) {
  int i;
  pthread_t t[THREADS];
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Torturer, 0));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, pthread_join(t[i], 0));
  }
}
