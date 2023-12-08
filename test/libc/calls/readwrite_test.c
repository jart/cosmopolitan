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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

jmp_buf jb;
int pfds[2];
atomic_bool isdone;
volatile bool canjmp;

void OnSignal(int sig) {
  if (canjmp) {
    canjmp = false;
    longjmp(jb, 1);
  }
}

void *ReadWorker(void *arg) {
  int got;
  char buf[8];
  while (!isdone) {
    if (!(got = setjmp(jb))) {
      canjmp = true;
      read(pfds[0], buf, sizeof(buf));
      abort();
    }
  }
  return 0;
}

TEST(eintr, longjmp) {
  pthread_t th;
  struct sigaction sa = {
      .sa_handler = OnSignal,
      .sa_flags = SA_SIGINFO | SA_NODEFER,
  };
  sigaction(SIGUSR1, &sa, 0);
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, ReadWorker, 0));
  for (int i = 0; i < 10; ++i) {
    pthread_kill(th, SIGUSR1);
    usleep(1. / CLK_TCK * 1e6);
  }
  isdone = true;
  pthread_kill(th, SIGUSR1);
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}
