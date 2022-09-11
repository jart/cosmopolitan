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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

/**
 * @fileoverview test non-executable stack is default
 */

jmp_buf jb;

void EscapeSegfault(int sig, struct siginfo *si, void *vctx) {
  longjmp(jb, 666);
}

TEST(xstack, test) {
  if (IsXnu()) return;  // TODO(jart): what's up with xnu in MODE=tiny?
  struct sigaction old[2];
  struct sigaction sa = {
      .sa_sigaction = EscapeSegfault,
      .sa_flags = SA_SIGINFO,
  };
  sigaction(SIGSEGV, &sa, old + 0);
  sigaction(SIGBUS, &sa, old + 1);
  char code[16] = {
      0x55,                          // push %rbp
      0xb8, 0007, 0x00, 0x00, 0x00,  // mov  $7,%eax
      0x5d,                          // push %rbp
      0xc3,                          // ret
  };
  int (*func)(void) = (void *)code;
  int rc;
  if (!(rc = setjmp(jb))) {
    func();
    abort();
  }
  ASSERT_EQ(666, rc);
  sigaction(SIGBUS, old + 1, 0);
  sigaction(SIGSEGV, old + 0, 0);
}
