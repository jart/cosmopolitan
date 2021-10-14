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
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/nr.h"
#include "libc/thread/descriptor.h"
#include "libc/thread/join.h"

int cthread_join(cthread_t td, int* rc) {
  int tid = td->tid;  // tid must be loaded before lock xadd
  // otherwise, tid could be set to 0 even though `state` is not finished

  // mark thread as joining
  int state;
  asm volatile("lock xadd\t%1, %0"
               : "+m"(td->state), "=r"(state)
               : "1"(cthread_joining)
               : "cc");

  if (!(state & cthread_finished)) {
    int flags = FUTEX_WAIT;  // PRIVATE makes it hang
    register struct timespec* timeout asm("r10") = NULL;
    asm volatile("syscall"
                 : /* no outputs */
                 : "a"(__NR_futex), "D"(&td->tid), "S"(flags), "d"(tid),
                   "r"(timeout)
                 : "rcx", "r11", "cc", "memory");
  }

  *rc = td->rc;

  size_t size = (intptr_t)(td->alloc.top) - (intptr_t)(td->alloc.bottom);
  munmap(td->alloc.bottom, size);
  return 0;
}
