/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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

TEST(strerror, enosys) {
  if (IsTiny()) return;
  EXPECT_STARTSWITH("ENOSYS", strerror(ENOSYS));
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
