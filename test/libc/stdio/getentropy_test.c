/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#ifndef __aarch64__
// TODO(jart): Make this test less resource intensive.
// TODO(jart): Why can EINTR happen on Windows?

atomic_int done;
atomic_int ready;
pthread_t parent;
atomic_int gotsome;

void OnSig(int sig) {
  ++gotsome;
}

void *TortureWorker(void *arg) {
  sigset_t ss;
  sigfillset(&ss);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &ss, 0));
  ready = true;
  while (!done) {
    if (!IsWindows()) pthread_kill(parent, SIGUSR1);
    usleep(1);
    if (!IsWindows()) pthread_kill(parent, SIGUSR2);
    usleep(1);
  }
  return 0;
}

TEST(getentropy, test) {
  pthread_t child;
  double e, w = 7.7;
  struct sigaction sa;
  int i, k, m, n = 999;
  char *buf = gc(calloc(1, n));
  sa.sa_flags = 0;
  sa.sa_handler = OnSig;
  sigemptyset(&sa.sa_mask);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, 0));
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &sa, 0));
  parent = pthread_self();
  ASSERT_EQ(0, pthread_create(&child, 0, TortureWorker, 0));
  while (!ready) pthread_yield();
  for (k = 0; k < 10; ++k) {
    ASSERT_SYS(0, 0, getentropy(0, 0));
    for (i = 0; i < n; i += m) {
      m = MIN(n - i, 256);
      ASSERT_SYS(0, 0, getentropy(buf + i, m));
      ASSERT_SYS(EFAULT, -1, getentropy(0, m));
    }
    if ((e = MeasureEntropy(buf, n)) < w) {
      fprintf(stderr, "error: entropy suspect! got %g but want >=%g\n", e, w);
      for (i = 0; i < n;) {
        if (!(i % 16)) fprintf(stderr, "%6x ", i);
        fprintf(stderr, "%lc", kCp437[buf[i] & 255]);
        if (!(++i % 16)) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
      done = true;
      pthread_join(child, 0);
      exit(1);
    }
  }
  done = true;
  ASSERT_EQ(0, pthread_join(child, 0));
  if (!IsWindows()) ASSERT_GT(gotsome, 0);
}

#endif /* __aarch64__ */
