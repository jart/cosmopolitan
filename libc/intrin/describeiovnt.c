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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/nt/winsock.h"

void DescribeIovNt(const struct NtIovec *iov, uint32_t iovlen, ssize_t rem) {
  int i;
  if ((!IsAsan() && kisdangerous(iov)) ||
      (IsAsan() && !__asan_is_valid(iov, iovlen * sizeof(struct NtIovec)))) {
    kprintf("%p", iov);
    return;
  }
  kprintf("{");
  for (i = 0; rem && i < MIN(5, iovlen); ++i) {
    kprintf("%s{%#.*hhs%s, %'zu}", i ? ", " : "",
            MAX(0, MIN(40, MIN(rem, iov[i].len))), iov[i].buf,
            MAX(0, MIN(40, MIN(rem, iov[i].len))) < iov[i].len ? "..." : "",
            iov[i].len);
    rem -= iov[i].len;
  }
  kprintf("%s}", iovlen > 5 ? "..." : "");
}
