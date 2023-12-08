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
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"

/**
 * Converts errno value to string.
 *
 * @param err is error number or zero if unknown
 * @return 0 on success, or errno on error
 * @raise ERANGE if insufficient buffer was available, in which case a
 *     nul-terminated string is still copied to `buf`
 */
errno_t strerror_r(int err, char *buf, size_t size) {

  int c;
  char tmp[32];
  char *p = buf;
  char *pe = p + size;

  // copy unix error information
  const char *msg;
  if (!err) {
    msg = "No error information";
  } else if (!(msg = _strerdoc(err))) {
    FormatInt32(stpcpy(tmp, "Error "), err);
    msg = tmp;
  }
  while ((c = *msg++) && p + 1 < pe) {
    *p++ = c;
  }

  // copy windows error information
  if (IsWindows()) {
    uint32_t winerr;
    if ((winerr = GetLastError()) != err && winerr) {
      stpcpy(FormatUint32(stpcpy(tmp, " (win32 error "), winerr), ")");
      msg = tmp;
      while ((c = *msg++) && p + 1 < pe) {
        *p++ = c;
      }
    }
  }

  // force nul terminator
  if (p < pe) {
    *p++ = 0;
  }

  // return result code
  if (p < pe) {
    return 0;
  } else {
    return ERANGE;
  }
}
