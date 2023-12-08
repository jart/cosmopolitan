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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

void OnUsr1(int sig, struct siginfo *si, void *vctx) {
}

void SetUp(void) {
  struct sigaction sig = {.sa_sigaction = OnUsr1, .sa_flags = SA_SIGINFO};
  sigaction(SIGUSR1, &sig, 0);
}

void TriggerSignal(void) {
  sched_yield();
  raise(SIGUSR1);
  sched_yield();
}

static void *Increment(void *arg) {
  ASSERT_EQ(gettid(), pthread_getthreadid_np());
  TriggerSignal();
  return (void *)((uintptr_t)arg + 1);
}

TEST(pthread_detach, testCreateReturn) {
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, Increment, 0));
  ASSERT_EQ(0, pthread_detach(id));
  while (!pthread_orphan_np()) {
    _pthread_decimate();
  }
}

TEST(pthread_detach, testDetachUponCreation) {
  pthread_t th;
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
  ASSERT_EQ(0, pthread_create(&th, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  while (!pthread_orphan_np()) {
    _pthread_decimate();
  }
}
