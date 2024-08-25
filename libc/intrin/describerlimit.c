/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/rlimit.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/rlim.h"

const char *_DescribeRlimit(char buf[64], int rc, const struct rlimit *rlim) {
  if (rc == -1)
    return "n/a";
  if (!rlim)
    return "NULL";
  if (kisdangerous(rlim)) {
    ksnprintf(buf, 64, "%p", rlim);
  } else {
    char str[2][21];
    if (rlim->rlim_cur == RLIM_INFINITY) {
      strcpy(str[0], "RLIM_INFINITY");
    } else {
      FormatInt64(str[0], rlim->rlim_cur);
    }
    if (rlim->rlim_max == RLIM_INFINITY) {
      strcpy(str[1], "RLIM_INFINITY");
    } else {
      FormatInt64(str[1], rlim->rlim_max);
    }
    ksnprintf(buf, 64, "{%s, %s}", str[0], str[1]);
  }
  return buf;
}
