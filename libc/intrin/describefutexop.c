/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/describeflags.internal.h"
#include "libc/sysv/consts/futex.h"

const char *DescribeFutexOp(int x) {
  if (x == FUTEX_WAIT) return "FUTEX_WAIT";
  if (x == FUTEX_WAKE) return "FUTEX_WAKE";
  if (x == FUTEX_REQUEUE) return "FUTEX_REQUEUE";
  // order matters (the private bit might be zero)
  if (x == FUTEX_WAIT_PRIVATE) return "FUTEX_WAIT_PRIVATE";
  if (x == FUTEX_WAKE_PRIVATE) return "FUTEX_WAKE_PRIVATE";
  if (x == FUTEX_REQUEUE_PRIVATE) return "FUTEX_REQUEUE_PRIVATE";
  return "FUTEX_???";
}
