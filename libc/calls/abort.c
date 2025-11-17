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
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

alignas(64) static struct {
  pthread_mutex_t lock;
  int phase;
} __abort = {
    PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,
};

/**
 * Terminates program abnormally.
 *
 * This function first tries to trigger your SIGABRT handler. If the
 * signal handler returns, then `signal(SIGABRT, SIG_DFL)` is called
 * before SIGABRT is raised again.
 *
 * @asyncsignalsafe
 * @noreturn
 */
wontreturn void abort(void) {
  sigset_t mask;
  if (!__spawned)
    pthread_mutex_lock(&__abort.lock);
  switch (__abort.phase) {
    case 0:
      mask = 1ull << (SIGABRT - 1);
      sigprocmask(SIG_UNBLOCK, &mask, 0);
      __abort.phase = 1;
      // fallthrough
    case 1:
      __abort.phase = 0;
      if (!__spawned)
        pthread_mutex_unlock(&__abort.lock);
      raise(SIGABRT);
      if (!__spawned)
        pthread_mutex_lock(&__abort.lock);
      __abort.phase = 2;
      // fallthrough
    case 2:
      __abort.phase = 3;
      signal(SIGABRT, SIG_DFL);
      // fallthrough
    case 3:
      __abort.phase = 4;
      raise(SIGABRT);
      // fallthrough
    case 4:
      __abort.phase = 5;
      _Exit(128 + SIGABRT);
  }
  for (;;)
    notpossible;
}
