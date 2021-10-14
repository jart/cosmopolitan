/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/testlib/testlib.h"

jmp_buf jb;
bool gotsegv;
struct sigaction old[2];

void OnSigSegv(int sig) {
  gotsegv = true;
  longjmp(jb, 1);
}

void SetUp(void) {
  sigaction(SIGBUS, NULL, &old[0]);
  sigaction(SIGSEGV, NULL, &old[1]);
}

void TearDown(void) {
  sigaction(SIGBUS, &old[0], NULL);
  sigaction(SIGSEGV, &old[1], NULL);
}

TEST(mprotect, test) {
  char *p = gc(memalign(PAGESIZE, PAGESIZE));
  p[0] = 0;
  ASSERT_NE(-1, mprotect(p, PAGESIZE, PROT_READ | PROT_WRITE));
  p[0] = 1;
  EXPECT_EQ(1, p[0]);
}

TEST(mprotect, testSegfault) {
  char *p;
  struct sigaction ss = {.sa_handler = OnSigSegv, .sa_flags = SA_NODEFER};
  if (IsWindows()) return; /* TODO */
  p = gc(memalign(PAGESIZE, PAGESIZE));
  EXPECT_NE(-1, sigaction(SIGBUS, &ss, NULL));
  EXPECT_NE(-1, sigaction(SIGSEGV, &ss, NULL));
  if (!setjmp(jb)) p[0] = 1;
  EXPECT_FALSE(gotsegv);
  EXPECT_NE(-1, mprotect(p, sizeof(p), PROT_READ));
  if (!setjmp(jb)) p[0] = 2;
  EXPECT_TRUE(gotsegv);
  EXPECT_EQ(1, p[0]);
  EXPECT_NE(-1, mprotect(p, sizeof(p), PROT_READ | PROT_WRITE));
}
