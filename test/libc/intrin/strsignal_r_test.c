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
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

TEST(strsignal, test) {
  EXPECT_STREQ("0", strsignal(0));
  EXPECT_STREQ("SIGINT", strsignal(SIGINT));
  EXPECT_STREQ("SIGQUIT", strsignal(SIGQUIT));
  EXPECT_STREQ("SIGALRM", strsignal(SIGALRM));
  EXPECT_STREQ("SIGUSR1", strsignal(SIGUSR1));
  EXPECT_STREQ("SIGSTOP", strsignal(SIGSTOP));
  EXPECT_STREQ("150", strsignal(150));
}

TEST(strsignal, realtime) {
  if (!SIGRTMIN) return;
  EXPECT_STREQ("SIGTHR", strsignal(SIGTHR));
  ASSERT_STREQ("SIGRTMIN+1", strsignal(SIGRTMIN + 1));
}
