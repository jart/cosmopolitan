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
#include "libc/calls/struct/sigset.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

sigset_t actual;
sigset_t golden;
sigset_t original;

static void *MyThread(void *arg) {
  ASSERT_EQ(0, sigprocmask(SIG_BLOCK, 0, &actual));
  ASSERT_EQ(0, memcmp(&golden, &actual, sizeof(sigset_t)));
  return 0;
}

TEST(pthread_attr_setsigmask_np, getsAppliedToThread) {
  pthread_t id;
  pthread_attr_t attr;
  sigemptyset(&golden);
  sigaddset(&golden, SIGSYS);
  sigaddset(&golden, SIGUSR1);
  ASSERT_EQ(0, sigprocmask(SIG_BLOCK, 0, &original));
  ASSERT_NE(0, memcmp(&golden, &original, sizeof(sigset_t)));
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setsigmask_np(&attr, &golden));
  ASSERT_EQ(0, pthread_create(&id, &attr, MyThread, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
  ASSERT_EQ(0, sigprocmask(SIG_BLOCK, 0, &actual));
  ASSERT_EQ(0, memcmp(&actual, &original, sizeof(sigset_t)));
}
