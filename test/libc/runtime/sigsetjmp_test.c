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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

sigjmp_buf jb;
volatile int sigs;
volatile int jumps;

void OnSignal(int sig, siginfo_t *si, void *ctx) {
  ++sigs;
}

TEST(sigsetjmp, test) {
  sigset_t ss;
  int i, n = 1000;
  struct sigaction sa = {.sa_sigaction = OnSignal};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, 0));
  for (i = 0; i < n; ++i) {
    if (!sigsetjmp(jb, 1)) {
      sigemptyset(&ss);
      sigaddset(&ss, SIGUSR1);
      ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &ss, 0));
      ASSERT_SYS(0, 0, raise(SIGUSR1));
      siglongjmp(jb, 1);
    } else {
      ++jumps;
    }
  }
  ASSERT_EQ(1000, jumps);
  ASSERT_EQ(1000, sigs);
}
