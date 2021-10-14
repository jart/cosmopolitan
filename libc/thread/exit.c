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
#include "libc/sysv/consts/nr.h"
#include "libc/thread/descriptor.h"
#include "libc/thread/exit.h"
#include "libc/thread/self.h"

wontreturn void cthread_exit(int rc) {
  cthread_t td = cthread_self();
  td->rc = rc;
  size_t size = (intptr_t)(td->alloc.top) - (intptr_t)(td->alloc.bottom);

  int state;
  asm volatile("lock xadd\t%1, %0\n\t"      // mark thread as finished
               "test\t%2, %b1\n\t"          // test if thread was detached
               "jz .L.cthread_exit.%=\n\t"  // skip unmap if not detached
               "syscall\n"                  // unmap thread
               ".L.cthread_exit.%=:\n\t"
               "mov\t%%rbx, %%rdi\n\t"  // rc
               "mov\t$60, %%rax\n\t"
               "syscall"  // thread exit
               : "+m"(td->state), "=&r"(state)
               : "I"(cthread_detached), "1"(cthread_finished), "a"(__NR_munmap),
                 "b"(rc), "D"(td->alloc.bottom), "S"(size)
               : "rcx", "r11", "cc", "memory");
  unreachable;
}
