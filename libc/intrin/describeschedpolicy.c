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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sched.h"

/**
 * Describes clock_gettime() clock argument.
 */
const char *(DescribeSchedPolicy)(char buf[48], int x) {
  char *p = buf;
  if (x == -1) {
    goto DoNumber;
  }
  if (x & SCHED_RESET_ON_FORK) {
    x &= ~SCHED_RESET_ON_FORK;
    p = stpcpy(p, "SCHED_RESET_ON_FORK");
  }
  if (x == SCHED_OTHER) {
    stpcpy(p, "SCHED_OTHER");
  } else if (x == SCHED_FIFO) {
    stpcpy(p, "SCHED_FIFO");
  } else if (x == SCHED_RR) {
    stpcpy(p, "SCHED_RR");
  } else if (x == SCHED_BATCH) {
    stpcpy(p, "SCHED_BATCH");
  } else if (x == SCHED_IDLE) {
    stpcpy(p, "SCHED_IDLE");
  } else if (x == SCHED_DEADLINE) {
    stpcpy(p, "SCHED_DEADLINE");
  } else {
  DoNumber:
    FormatInt32(p, x);
  }
  return buf;
}
