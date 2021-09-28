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
#include "libc/thread/exit.h"
#include "libc/thread/descriptor.h"
#include "libc/linux/munmap.h"

static wontreturn void _self_exit(void* p, size_t s, int r) {
  // asm is necessary as the stack does not exist between the unmap and the actual exit
  asm volatile(
    "mov $11, %%rax\n\t"
    "syscall\n\t"
    "mov %%rbx, %%rdi\n\t"
    "mov $60, %%rax\n\t"
    "syscall"
    :
    : "D"(p), "S"(s), "b"(r)
    : "memory"
  );
  unreachable;
}

wontreturn void cthread_exit(int rc) {
  // TODO: wait joiners
  
  cthread_t td;
  asm("mov %%fs:0, %0" : "=r"(td));
  size_t size = (intptr_t)(td->alloc.top) - (intptr_t)(td->alloc.bottom);
  
  _self_exit(td->alloc.top, size, rc);
}
