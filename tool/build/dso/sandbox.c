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
#include "libc/calls/calls.h"
#include "libc/calls/pledge.h"
#include "libc/calls/pledge.internal.h"
#include "libc/intrin/promises.internal.h"
#include "libc/runtime/runtime.h"

// runs pledge at glibc executable load time, e.g.
// strace -vff bash -c '_PLEDGE=4194303,0 LD_PRELOAD=$HOME/sandbox.so ls'

__attribute__((__constructor__)) void init(void) {
  int c, i, j;
  const char *s;
  uint64_t arg[2] = {0};
  s = getenv("_PLEDGE");
  for (i = j = 0; i < 2; ++i) {
    while ((c = s[j] & 255)) {
      ++j;
      if ('0' <= c && c <= '9') {
        arg[i] *= 10;
        arg[i] += c - '0';
      } else {
        break;
      }
    }
  }
  sys_pledge_linux(~arg[0], arg[1]);
}
