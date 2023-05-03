/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

int main(int, char **, char **);

int __argc;
char **__argv;
char **__envp;
char **environ;

static inline long sys_set_tid_address(int *t) {
  register int64_t __r0 asm("x0") = (int64_t)t;
  register int64_t __res_x0 asm("x0");
  int64_t __res;
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0x0\n"
               : "=r"(__res_x0)
               : "i"(96), "r"(__r0)
               : "x8", "memory");
  return __res_x0;
}

void _start_c(long *sp) {
  int argc;
  char **argv, **envp;
  unsigned long *auxv;
  argc = *sp;
  argv = (char **)(sp + 1);
  envp = (char **)(sp + 1 + argc + 1);
  auxv = (unsigned long *)(sp + 1 + argc + 1);
  for (;;) {
    if (!*auxv++) {
      break;
    }
  }
  __auxv = auxv;
  environ = envp;
  exit(main(argc, argv, envp));
}
