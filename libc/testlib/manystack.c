/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/testlib/manystack.h"
#include "libc/atomic.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/dll.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"

static atomic_int manystack_gotsig;
static atomic_bool manystack_shutdown;

static void manystack_signal(int sig) {
  manystack_gotsig = sig;
}

static void *manystack_thread(void *arg) {
  sigset_t ss;
  sigfillset(&ss);
  sigdelset(&ss, SIGUSR2);
  while (!manystack_shutdown) {
    sigsuspend(&ss);
    if (!manystack_shutdown) {
      _pthread_lock();
      for (struct Dll *e = dll_first(_pthread_list); e;
           e = dll_next(_pthread_list, e)) {
        pthread_t th = (pthread_t)POSIXTHREAD_CONTAINER(e);
        if (!pthread_equal(th, pthread_self()))
          pthread_kill(th, SIGQUIT);
      }
      _pthread_unlock();
    }
  }
  return 0;
}

pthread_t manystack_start(void) {
  sigset_t ss;
  pthread_t msh;
  sigemptyset(&ss);
  sigaddset(&ss, SIGUSR2);
  sigprocmask(SIG_BLOCK, &ss, 0);
  signal(SIGUSR2, manystack_signal);
  pthread_create(&msh, 0, manystack_thread, 0);
  return msh;
}

void manystack_stop(pthread_t msh) {
  manystack_shutdown = true;
  pthread_kill(msh, SIGUSR2);
  pthread_join(msh, 0);
}
