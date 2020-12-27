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
#include "libc/bits/safemacros.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"

const char *geterrname(int code) {
  extern const char kErrnoNames[];
  const long *e;
  size_t i, n;
  e = &E2BIG;
  n = &EXFULL + 1 - e;
  for (i = 0; i < n; ++i) {
    if (code == e[i]) {
      return IndexDoubleNulString(kErrnoNames, i);
    }
  }
  return NULL;
}

/**
 * Converts errno value to string.
 * @return 0 on success, or error code
 */
int strerror_r(int err, char *buf, size_t size) {
  const char *s;
  if (err == -1 || IsTiny()) {
    s = "?";
  } else {
    s = firstnonnull(geterrname(err), "?");
  }
  if (!SupportsWindows()) {
    DebugBreak();
    snprintf(buf, size, "E%s[%d]", s, err);
  } else {
    char16_t buf16[100];
    int winstate, sysvstate;
    winstate = GetLastError();
    sysvstate = errno;
    if (FormatMessage(
            kNtFormatMessageFromSystem | kNtFormatMessageIgnoreInserts, NULL,
            err, 0, buf16, ARRAYLEN(buf16) - 1, 0) > 0) {
      chomp16(buf16);
    } else {
      buf16[0] = u'\0';
    }
    snprintf(buf, size, "E%s/err=%d/errno:%d/GetLastError:%d%s%hs", s, err,
             sysvstate, winstate, buf16[0] ? " " : "", buf16);
  }
  return 0;
}
