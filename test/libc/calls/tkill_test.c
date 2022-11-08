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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

_Thread_local intptr_t gotsig;

void OnSig(int sig) {
  gotsig = sig;
}

void *Worker(void *arg) {
  sigset_t ss;
  sigemptyset(&ss);
  ASSERT_SYS(EINTR, -1, sigsuspend(&ss));
  return (void *)gotsig;
}

TEST(tkill, test) {
  if (IsWindows()) return;  // TODO(jart): fix me
  void *res;
  int i, tid;
  pthread_t t;
  sigset_t ss, oldss;
  sighandler_t oldsig;
  sigemptyset(&ss);
  sigaddset(&ss, SIGUSR1);
  oldsig = signal(SIGUSR1, OnSig);
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ASSERT_EQ(0, pthread_create(&t, 0, Worker, 0));
  ASSERT_EQ(0, pthread_getunique_np(t, &tid));
  ASSERT_SYS(0, 0, tkill(tid, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, &res));
  ASSERT_EQ(SIGUSR1, (intptr_t)res);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldss, 0));
  signal(SIGUSR1, oldsig);
}
