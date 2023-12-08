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
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/futex.h"

const char *(DescribeFutexOp)(char buf[64], int x) {

  bool priv = false;
  if (x & FUTEX_PRIVATE_FLAG) {
    priv = true;
    x &= ~FUTEX_PRIVATE_FLAG;
  }

  bool real = false;
  if (x & FUTEX_CLOCK_REALTIME) {
    real = true;
    x &= ~FUTEX_CLOCK_REALTIME;
  }

  char *p = buf;

  if (x == FUTEX_WAIT) {
    p = stpcpy(p, "FUTEX_WAIT");
  } else if (x == FUTEX_WAKE) {
    p = stpcpy(p, "FUTEX_WAKE");
  } else if (x == FUTEX_REQUEUE) {
    p = stpcpy(p, "FUTEX_REQUEUE");
  } else if (x == FUTEX_WAIT_BITSET) {
    p = stpcpy(p, "FUTEX_WAIT_BITSET");
  } else {
    p = stpcpy(p, "FUTEX_");
    p = FormatUint32(p, x);
  }

  if (priv) {
    p = stpcpy(p, "_PRIVATE");
  }

  if (real) {
    p = stpcpy(p, "|FUTEX_CLOCK_REALTIME");
  }

  return buf;
}
