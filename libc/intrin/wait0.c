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
#include "libc/assert.h"
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/futex.internal.h"
#include "libc/intrin/wait0.internal.h"
#include "libc/linux/futex.h"

/**
 * Blocks until memory location becomes zero.
 *
 * This is intended to be used on the child thread id, which is updated
 * by the clone() system call when a thread terminates. The purpose of
 * this operation is to know when it's safe to munmap() a thread stack.
 */
void _wait0(const int *ptid) {
  int x;
  for (;;) {
    if (!(x = atomic_load_explicit(ptid, memory_order_acquire))) {
      break;
    } else if (IsLinux() /* || IsOpenbsd() */) {
      _futex_wait(ptid, x, &(struct timespec){2});
    } else {
      sched_yield();
    }
  }
}
