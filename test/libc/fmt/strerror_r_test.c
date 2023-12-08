/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

/*
 * If these tests break, it's probably because
 * libc/sysv/consts.sh changed and
 * libc/sysv/kErrnoNames.S needs updating.
 */

TEST(strerror, e2big) {
  EXPECT_STARTSWITH("Arg list too long", strerror(E2BIG));
}

TEST(strerror, enosys) {
  EXPECT_STARTSWITH("Function not implemented", strerror(ENOSYS));
}

TEST(strerror, einval) {
  EXPECT_STARTSWITH("Invalid argument", strerror(EINVAL));
}

TEST(strerror, symbolizingTheseNumbersAsErrorsIsHeresyInUnixStyle) {
  EXPECT_STARTSWITH("No error information", strerror(0));
}

TEST(strerror, enotconn_orLinkerIsntUsingLocaleC_orCodeIsOutOfSync) {
  EXPECT_STARTSWITH("Transport endpoint is not connected", strerror(ENOTCONN));
}

TEST(strerror, exfull_orLinkerIsntUsingLocaleC_orCodeIsOutOfSync) {
  EXPECT_STARTSWITH("Text file busy", strerror(ETXTBSY));
}
