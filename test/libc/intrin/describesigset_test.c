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
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

TEST(DescribeSigset, full) {
  sigset_t ss;
  sigfillset(&ss);
  if (IsXnu()) {
    EXPECT_STREQ("~{ABRT,THR,KILL,STOP}", DescribeSigset(0, &ss));
  } else if (IsOpenbsd()) {
    EXPECT_STREQ("~{ABRT,KILL,STOP,THR}", DescribeSigset(0, &ss));
  } else {
    EXPECT_STREQ("~{ABRT,KILL,STOP,THR}", DescribeSigset(0, &ss));
  }
}

TEST(DescribeSigset, present) {
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGINT);
  sigaddset(&ss, SIGUSR1);
  EXPECT_STREQ("{INT,USR1}", DescribeSigset(0, &ss));
}

TEST(DescribeSigset, absent) {
  sigset_t ss;
  sigfillset(&ss);
  sigdelset(&ss, SIGINT);
  sigdelset(&ss, SIGUSR1);
  if (IsXnu()) {
    EXPECT_STREQ("~{INT,ABRT,THR,KILL,STOP,USR1}", DescribeSigset(0, &ss));
  } else if (IsBsd()) {
    EXPECT_STREQ("~{INT,ABRT,KILL,STOP,USR1,THR}", DescribeSigset(0, &ss));
  } else {
    EXPECT_STREQ("~{INT,ABRT,KILL,USR1,STOP,THR}", DescribeSigset(0, &ss));
  }
}
