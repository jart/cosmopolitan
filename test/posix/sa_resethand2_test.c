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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"

volatile int handler_invoked;

void signal_handler(int signum) {
  handler_invoked = 1;
}

int main() {
  sigset_t mask, oldmask;
  struct sigaction sa, current_sa;

  if (IsWindows()) {
    // TODO(jart): support non-fatal signals between processes
    return 0;
  }

  sa.sa_handler = signal_handler;
  sa.sa_flags = SA_RESETHAND;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, 0) == -1) {
    return 1;
  }

  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
    return 2;
  }

  int pid = fork();
  if (pid == -1) {
    return 3;
  } else if (pid == 0) {
    kill(getppid(), SIGINT);
    return 0;
  } else {
    sigsuspend(&oldmask);
    if (!handler_invoked) {
      return 4;
    }
    if (sigaction(SIGINT, 0, &current_sa) == -1) {
      return 5;
    }
    if (current_sa.sa_handler != SIG_DFL) {
      return 6;
    }
    return 0;
  }
}
