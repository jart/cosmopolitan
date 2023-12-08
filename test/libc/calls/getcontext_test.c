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
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

int x;
bool ok1;
bool ok2;
ucontext_t context;

void func(void) {
  x++;
  setcontext(&context);
  abort();
}

void test(void) {
  getcontext(&context);
  if (!x) {
    ok1 = true;
    func();
  } else {
    ok2 = true;
  }
}

TEST(getcontext, test) {
  test();
  ASSERT_TRUE(ok1);
  ASSERT_TRUE(ok2);
}

TEST(getcontext, canReadAndWriteSignalMask) {
  sigset_t ss, old;
  volatile int n = 0;
  sigemptyset(&ss);
  sigaddset(&ss, SIGUSR1);
  sigprocmask(SIG_SETMASK, &ss, &old);
  ASSERT_EQ(0, getcontext(&context));
  if (!n) {
    n = 1;
    ASSERT_TRUE(sigismember(&context.uc_sigmask, SIGUSR1));
    sigaddset(&context.uc_sigmask, SIGUSR2);
    setcontext(&context);
    abort();
  }
  sigprocmask(SIG_SETMASK, 0, &ss);
  ASSERT_TRUE(sigismember(&ss, SIGUSR2));
  sigprocmask(SIG_SETMASK, &old, 0);
}

void SetGetContext(void) {
  int a = 0;
  getcontext(&context);
  if (!a) {
    a = 1;
    setcontext(&context);
  }
}

BENCH(getcontext, bench) {
  EZBENCH2("getsetcontext", donothing, SetGetContext());
}

BENCH(swapcontext, bench) {
  ucontext_t main, loop;
  volatile bool ready = false;
  getcontext(&main);
  if (ready) {
    for (;;) {
      swapcontext(&main, &loop);
      // kprintf("boom\n");
    }
  } else {
    ready = true;
    EZBENCH2("swapcontextx2", donothing, swapcontext(&loop, &main));
    // kprintf("dollar\n");
  }
}
