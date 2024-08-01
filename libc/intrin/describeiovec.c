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
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"

#define N 300

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribeIovec)(char buf[N], ssize_t rc, const struct iovec *iov,
                            int iovlen) {
  const char *d;
  int i, j, o = 0;

  if (!iov)
    return "NULL";
  if (rc == -1)
    return "n/a";
  if (rc == -2)
    rc = SSIZE_MAX;
  if (kisdangerous(iov)) {
    ksnprintf(buf, N, "%p", iov);
    return buf;
  }

  append("{");

  for (i = 0; rc && i < iovlen; ++i) {
    if (iov[i].iov_len < rc) {
      j = iov[i].iov_len;
      rc -= iov[i].iov_len;
    } else {
      j = rc;
      rc = 0;
    }
    if (j > 40) {
      j = 40;
      d = "...";
    } else {
      d = "";
    }
    append("%s{%#.*hhs%s, %'zu}", i ? ", " : "", j, iov[i].iov_base, d,
           iov[i].iov_len);
  }

  append("}");

  return buf;
}
