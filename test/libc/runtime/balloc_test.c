/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/ucontext.h"
#include "libc/runtime/buffer.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "third_party/xed/x86.h"

char *p;
bool segfaulted_;
struct GuardedBuffer b_;
struct sigaction oldsegv_;
struct XedDecodedInst xedd_;

void RestrictPage(void *addr, unsigned flags) {
  addr = (void *)rounddown((intptr_t)addr, PAGESIZE);
  EXPECT_NE(-1, mprotect(addr, PAGESIZE, flags));
}

void OnSegLol(int sig, struct siginfo *si, struct ucontext *uc) {
  size_t i;
  uint8_t *rip;
  segfaulted_ = true;
  rip = (uint8_t *)uc->uc_mcontext.rip;
  RestrictPage(rip, PROT_READ | PROT_WRITE | PROT_EXEC);
  ASSERT_EQ(XED_ERROR_NONE,
            xed_instruction_length_decode(xed_decoded_inst_zero_set_mode(
                                              &xedd_, XED_MACHINE_MODE_LONG_64),
                                          rip, XED_MAX_INSTRUCTION_BYTES));
  for (i = 0; i < xedd_.decoded_length; ++i) rip[i] = 0x90; /* NOP */
  RestrictPage(rip, PROT_READ | PROT_EXEC);
}

void SetUp(void) {
  segfaulted_ = false;
  memset(&b_, 0, sizeof(b_));
  ASSERT_NE(-1, xsigaction(SIGSEGV, OnSegLol, SA_RESETHAND | SA_RESTART, 0,
                           &oldsegv_));
}

void TearDown(void) {
  EXPECT_NE(-1, sigaction(SIGSEGV, &oldsegv_, NULL));
  bfree(&b_);
  EXPECT_EQ(NULL, b_.p);
}

TEST(balloc, createsGuardPage) {
  ASSERT_NE(NULL, (p = balloc(&b_, 1, 1)));
  EXPECT_EQ(p, b_.p);
  p[0] = '.';
  ASSERT_FALSE(segfaulted_);
  /* TODO(jart): fix me!!! */
  /* p[1 + __BIGGEST_ALIGNMENT__] = '!'; */
  /* EXPECT_TRUE(segfaulted_); */
}

TEST(balloc, aligned_roundsUp) {
  ASSERT_NE(NULL, (p = balloc(&b_, 128, 1)));
  EXPECT_EQ(0, (intptr_t)b_.p & 127);
  p[127] = '.';
  ASSERT_FALSE(segfaulted_);
  /* TODO(jart): fix me!!! */
  /* p[128 + __BIGGEST_ALIGNMENT__] = '!'; */
  /* EXPECT_TRUE(segfaulted_); */
}

TEST(balloc, multipleCalls_avoidsNeedlessSyscalls) {
  size_t c;
  c = g_syscount;
  ASSERT_NE(NULL, (p = balloc(&b_, 1, 31337)));
  EXPECT_GT(g_syscount, c);
  c = g_syscount;
  ASSERT_NE(NULL, (p = balloc(&b_, 1, 31337 / 2)));
  EXPECT_EQ(g_syscount, c);
  c = g_syscount;
  ASSERT_NE(NULL, (p = balloc(&b_, 1, 31337 * 2)));
  EXPECT_GT(g_syscount, c);
}
