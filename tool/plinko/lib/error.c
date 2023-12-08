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
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "tool/plinko/lib/error.h"
#include "tool/plinko/lib/plinko.h"
#include "tool/plinko/lib/printf.h"
#include "tool/plinko/lib/stack.h"

relegated wontreturn void Raise(int x) {
  Flush(1);
  Flush(2);
  longjmp(crash, ~x);
}

relegated wontreturn void Error(const char *f, ...) {
  int n;
  va_list va;
  Flush(1);
  Flush(2);
  va_start(va, f);
  n = Fprintf(2, "\e[1;31merror\e[0m: ");
  n = Vfnprintf(f, va, 2, n);
  Fprintf(2, "%n");
  va_end(va);
  Raise(kError);
}

relegated wontreturn void OutOfMemory(void) {
  Error("out of memory");
}

relegated wontreturn void StackOverflow(void) {
  Error("stack overflow");
}

relegated wontreturn void React(int e, int x, int k) {
  if (!sp || e != LO(GetCurrentFrame())) Push(e);
  Push(x);
  Raise(k);
}
