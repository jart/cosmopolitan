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
#include "libc/calls/calls.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/nr.h"
#include "libc/testlib/testlib.h"

#define Z 0x5555555555555555

#define FLAGS_cf 0
#define FLAGS_pf 2
#define FLAGS_sf 7
#define FLAGS_of 11

intptr_t diagnose_syscall(intptr_t nr,         //
                          intptr_t arg1,       //
                          intptr_t arg2,       //
                          intptr_t arg3,       //
                          intptr_t arg4,       //
                          intptr_t arg5,       //
                          intptr_t arg6,       //
                          intptr_t arg7,       //
                          ucontext_t *before,  //
                          ucontext_t *after);  //

#define GREG(FIELD)                                     \
  do {                                                  \
    uint64_t f1 = x->uc_mcontext.FIELD;                 \
    uint64_t f2 = y->uc_mcontext.FIELD;                 \
    if (f1 != f2) {                                     \
      if (b) appendw(&b, ' ');                          \
      appends(&b, #FIELD);                              \
      kprintf("%3s %016lx → %016lx\n", #FIELD, f1, f2); \
    }                                                   \
  } while (0)

#define FLAG(FLAG)                                       \
  if ((x->uc_mcontext.eflags & (1ul << FLAGS_##FLAG)) ^  \
      (y->uc_mcontext.eflags & (1ul << FLAGS_##FLAG))) { \
    if (b) appendw(&b, ' ');                             \
    appends(&b, #FLAG);                                  \
  }

char *DiffContexts(ucontext_t *x, ucontext_t *y) {
  char *b = 0;
  GREG(rax);
  GREG(rdx);
  GREG(rdi);
  GREG(rsi);
  GREG(rcx);
  GREG(r8);
  GREG(r9);
  GREG(r10);
  GREG(r11);
  GREG(r12);
  GREG(r13);
  GREG(r14);
  GREG(r15);
  GREG(rbx);
  GREG(rbp);
  FLAG(cf);
  FLAG(sf);
  FLAG(of);
  FLAG(pf);
  return b;
}

void SetUp(void) {
  if (IsWindows()) {
    exit(0);
  }
}

TEST(diagnose_syscall, getpid) {
  ucontext_t x, y;
  diagnose_syscall(__NR_getpid, Z, Z, Z, Z, Z, Z, Z, &x, &y);
  if (IsFreebsd()) {
    ASSERT_STREQ("rax rcx r8 r9 r10 r11", _gc(DiffContexts(&x, &y)));
  } else if (IsNetbsd() || IsXnu()) {
    // netbsd puts parent pid in edx
    // xnu seems to just clobber it!
    ASSERT_STREQ("rax rdx rcx r11", _gc(DiffContexts(&x, &y)));
  } else if (IsWsl1()) {
    // XXX: WSL1 must be emulating SYSCALL instructions.
    ASSERT_STREQ("rax rcx", _gc(DiffContexts(&x, &y)));
  } else {
    ASSERT_STREQ("rax rcx r11", _gc(DiffContexts(&x, &y)));
  }
}

TEST(diagnose_syscall, testWriteSuccess) {
  ucontext_t x, y;
  diagnose_syscall(__NR_write, 2, Z, 0, Z, Z, Z, Z, &x, &y);
  if (IsFreebsd()) {
    ASSERT_STREQ("rax rcx r8 r9 r10 r11", _gc(DiffContexts(&x, &y)));
  } else if (IsWsl1()) {
    // XXX: WSL1 must be emulating SYSCALL instructions.
    ASSERT_STREQ("rax rcx", _gc(DiffContexts(&x, &y)));
  } else {
    ASSERT_STREQ("rax rcx r11", _gc(DiffContexts(&x, &y)));
  }
}

TEST(diagnose_syscall, testWriteFailed) {
  ucontext_t x, y;
  diagnose_syscall(__NR_write, -1, Z, Z, Z, Z, Z, Z, &x, &y);
  if (IsFreebsd()) {
    ASSERT_STREQ("rax rcx r8 r9 r10 r11 cf", _gc(DiffContexts(&x, &y)));
  } else if (IsBsd()) {
    ASSERT_STREQ("rax rcx r11 cf", _gc(DiffContexts(&x, &y)));
  } else if (IsWsl1()) {
    // XXX: WSL1 must be emulating SYSCALL instructions.
    ASSERT_STREQ("rax rcx", _gc(DiffContexts(&x, &y)));
  } else {
    ASSERT_STREQ("rax rcx r11", _gc(DiffContexts(&x, &y)));
  }
}
