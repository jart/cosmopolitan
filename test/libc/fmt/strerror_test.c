/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

/*
 * If these tests break, it's probably because
 * libc/sysv/consts.sh changed and
 * libc/sysv/kErrnoNames.S needs updating.
 */

TEST(strerror, e2big) {
  if (IsTiny()) return;
  EXPECT_STARTSWITH("E2BIG", strerror(E2BIG));
}

TEST(strerror, enosys) {
  if (IsTiny()) return;
  EXPECT_STARTSWITH("ENOSYS", strerror(ENOSYS));
}

TEST(strerror, einval) {
  if (IsTiny()) return;
  EXPECT_STARTSWITH("EINVAL", strerror(EINVAL));
}

TEST(strerror, symbolizingTheseNumbersAsErrorsIsHeresyInUnixStyle) {
  EXPECT_STARTSWITH("E?/err=0/errno:0/GetLastError:0", strerror(0));
  EXPECT_STARTSWITH("E?", strerror(-1));
}

TEST(strerror, enotconn_orLinkerIsntUsingLocaleC_orCodeIsOutOfSync) {
  if (IsTiny()) return;
  EXPECT_STARTSWITH("ENOTCONN", strerror(ENOTCONN));
}

TEST(strerror, exfull_orLinkerIsntUsingLocaleC_orCodeIsOutOfSync) {
  if (IsLinux() && !IsTiny()) {
    EXPECT_STARTSWITH("EXFULL", strerror(EXFULL));
  } else {
    EXPECT_STARTSWITH("E?", strerror(EXFULL));
  }
}
