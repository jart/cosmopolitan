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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/pollfd.internal.h"

#define N 300

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribePollFds)(char buf[N], ssize_t rc, struct pollfd *fds,
                              size_t nfds) {
  char b64[64];
  int i, o = 0;

  if (!fds) return "NULL";
  if ((!IsAsan() && kisdangerous(fds)) ||
      (IsAsan() && !__asan_is_valid(fds, sizeof(*fds) * nfds))) {
    ksnprintf(buf, N, "%p", fds);
    return buf;
  }

  append("{");

  for (i = 0; i < nfds; ++i) {
    if (i) append(", ");
    append("{%d, %s", fds[i].fd, (DescribePollFlags)(b64, fds[i].events));
    if (rc >= 0) {
      append(", [%s]", (DescribePollFlags)(b64, fds[i].revents));
    }
    append("}");
  }

  append("}");

  return buf;
}
