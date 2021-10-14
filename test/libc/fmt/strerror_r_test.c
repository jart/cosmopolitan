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
  if (IsTiny()) {
    EXPECT_STARTSWITH("E2BIG", strerror(E2BIG));
  } else {
    EXPECT_STARTSWITH("E2BIG[Arg list too long]", strerror(E2BIG));
  }
}

TEST(strerror, enosys) {
  if (IsTiny()) {
    EXPECT_STARTSWITH("ENOSYS", strerror(ENOSYS));
  } else {
    EXPECT_STARTSWITH("ENOSYS[Function not implemented]", strerror(ENOSYS));
  }
}

TEST(strerror, einval) {
  if (IsTiny()) {
    EXPECT_STARTSWITH("EINVAL", strerror(EINVAL));
  } else {
    EXPECT_STARTSWITH("EINVAL[Invalid argument]", strerror(EINVAL));
  }
}

TEST(strerror, symbolizingTheseNumbersAsErrorsIsHeresyInUnixStyle) {
  if (IsTiny()) {
    EXPECT_STARTSWITH("EUNKNOWN", strerror(0));
  } else {
    EXPECT_STARTSWITH("EUNKNOWN[No error information]", strerror(0));
  }
  if (IsTiny()) {
    EXPECT_STARTSWITH("EUNKNOWN", strerror(-1));
  } else {
    EXPECT_STARTSWITH("EUNKNOWN[No error information]", strerror(-1));
  }
}

TEST(strerror, enotconn_orLinkerIsntUsingLocaleC_orCodeIsOutOfSync) {
  if (IsTiny()) {
    EXPECT_STARTSWITH("ENOTCONN", strerror(ENOTCONN));
  } else {
    EXPECT_STARTSWITH("ENOTCONN[Transport endpoint is not connected]",
                      strerror(ENOTCONN));
  }
}

TEST(strerror, exfull_orLinkerIsntUsingLocaleC_orCodeIsOutOfSync) {
  if (IsTiny()) {
    EXPECT_STARTSWITH("ETXTBSY", strerror(ETXTBSY));
  } else {
    EXPECT_STARTSWITH("ETXTBSY[Text file busy]", strerror(ETXTBSY));
  }
}
