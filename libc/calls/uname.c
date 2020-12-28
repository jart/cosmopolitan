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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/utsname.h"
#include "libc/str/str.h"

/**
 * Asks kernel to give us the `uname -a` data.
 * @return 0 on success, or -1 w/ errno
 */
int uname(struct utsname *lool) {
  char *out;
  size_t i, j, len;
  char tmp[sizeof(struct utsname)];
  memset(tmp, 0, sizeof(tmp));
  if (uname$sysv(tmp) != -1) {
    out = (char *)lool;
    i = 0;
    j = 0;
    for (;;) {
      len = strlen(&tmp[j]);
      if (len >= sizeof(struct utsname) - i) break;
      memcpy(&out[i], &tmp[j], len + 1);
      i += SYS_NMLN;
      j += len;
      while (j < sizeof(tmp) && tmp[j] == '\0') ++j;
      if (j == sizeof(tmp)) break;
    }
    return 0;
  } else {
    memset(lool, 0, sizeof(struct utsname));
    return -1;
  }
}
