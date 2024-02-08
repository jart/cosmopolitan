/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "third_party/xed/x86.h"

volatile bool gotsegv;
volatile bool gotbusted;
struct sigaction old[2];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

#ifdef __x86_64__
static const char kRet31337[] = {
    0xb8, 0x69, 0x7a, 0x00, 0x00,  // mov $31337,%eax
    0xc3,                          // ret
};
#elif defined(__aarch64__)
static const uint32_t kRet31337[] = {
    0x528f4d20,  // mov w0,#31337
    0xd65f03c0,  // ret
};
#else
#error "unsupported architecture"
#endif

void SkipOverFaultingInstruction(struct ucontext *ctx) {
#ifdef __x86_64__
  struct XedDecodedInst xedd;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
#elif defined(__aarch64__)
  ctx->uc_mcontext.pc += 4;
#else
#error "unsupported architecture"
#endif
}

void OnSigSegv(int sig, struct siginfo *si, void *vctx) {
  struct ucontext *ctx = vctx;
  gotsegv = true;
  SkipOverFaultingInstruction(ctx);
}

void OnSigBus(int sig, struct siginfo *si, void *vctx) {
  struct ucontext *ctx = vctx;
  gotbusted = true;
  SkipOverFaultingInstruction(ctx);
#if 0
  kprintf("SIGBUS%n");
  kprintf("si->si_signo = %G%n", si->si_signo);
  kprintf("si->si_errno = %s (%d)%n", _strerrno(si->si_errno),
          si->si_errno);
  kprintf("si->si_code = %s (%d)%n", DescribeSiCode(sig, si->si_code),
          si->si_code);
  kprintf("┌si->si_addr = %p%n", si->si_addr);
  kprintf("┼─────────────────%n");
  kprintf("│si->si_pid = %d (note: getpid() is %d)%n", si->si_pid, getpid());
  kprintf("│si->si_uid = %d%n", si->si_uid);
  kprintf("┼─────────────────%n");
  kprintf("│si->si_timerid = %d%n", si->si_timerid);
  kprintf("└si->si_overrun = %d%n", si->si_overrun);
  kprintf("si->si_value.sival_int = %d%n", si->si_value.sival_int);
  kprintf("si->si_value.sival_ptr = %p%n", si->si_value.sival_ptr);
  system(xasprintf("cat /proc/%d/map", getpid()));
#endif
}

void SetUp(void) {
  struct sigaction sabus = {.sa_sigaction = OnSigBus,
                            .sa_flags = SA_SIGINFO | SA_RESETHAND};
  struct sigaction sasegv = {.sa_sigaction = OnSigSegv,
                             .sa_flags = SA_SIGINFO | SA_RESETHAND};
  sigaction(SIGBUS, &sabus, old + 0);
  sigaction(SIGSEGV, &sasegv, old + 1);
  gotbusted = false;
  gotsegv = false;
}

void TearDown(void) {
  sigaction(SIGBUS, old + 0, 0);
  sigaction(SIGSEGV, old + 1, 0);
}

TEST(mprotect, testOkMemory) {
  char *p = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  p[0] = 0;
  ASSERT_NE(-1, mprotect(p, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE));
  p[0] = 1;
  EXPECT_EQ(1, p[0]);
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
}

TEST(mprotect, testSegfault_writeToReadOnlyAnonymous) {
  volatile char *p;
  p = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  EXPECT_FALSE(gotsegv);
  p[0] = 1;
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
  EXPECT_NE(-1, mprotect((void *)p, getauxval(AT_PAGESZ), PROT_READ));
  __expropriate(p[0]);
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
  p[0] = 2;
  EXPECT_TRUE(gotsegv | gotbusted);
  EXPECT_EQ(1, p[0]);
  EXPECT_NE(-1,
            mprotect((void *)p, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE));
}

TEST(mprotect, testExecOnly_canExecute) {
  char *p = _mapanon(FRAMESIZE);
  void (*f)(void) = (void *)p;
  memcpy(p, kRet31337, sizeof(kRet31337));
  ASSERT_SYS(0, 0, mprotect(p, FRAMESIZE, PROT_EXEC | PROT_READ));
  f();
  // On all supported platforms, PROT_EXEC implies PROT_READ. There is
  // one exception to this rule: Chromebook's fork of the Linux kernel
  // which has been reported, to have the ability to prevent a program
  // from reading its own code.
  ASSERT_SYS(0, 0, mprotect(p, FRAMESIZE, PROT_EXEC));
  f();
  munmap(p, FRAMESIZE);
}

TEST(mprotect, testProtNone_cantEvenRead) {
  volatile char *p;
  p = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  EXPECT_NE(-1, mprotect((void *)p, getauxval(AT_PAGESZ), PROT_NONE));
  __expropriate(p[0]);
  EXPECT_TRUE(gotsegv | gotbusted);
  EXPECT_NE(-1,
            mprotect((void *)p, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE));
}

TEST(mprotect, testExecJit_actuallyWorks) {
  int (*p)(void) = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  memcpy(p, kRet31337, sizeof(kRet31337));
  EXPECT_NE(-1, mprotect(p, getauxval(AT_PAGESZ), PROT_EXEC));
  EXPECT_EQ(31337, p());
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
  EXPECT_NE(-1, mprotect(p, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE));
}

TEST(mprotect, testRwxMap_vonNeumannRules) {
  if (IsOpenbsd()) return;     // boo
  if (IsXnuSilicon()) return;  // boo
  int (*p)(void) = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  memcpy(p, kRet31337, sizeof(kRet31337));
  EXPECT_NE(-1, mprotect(p, getauxval(AT_PAGESZ),
                         PROT_READ | PROT_WRITE | PROT_EXEC));
  EXPECT_EQ(31337, p());
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
  EXPECT_NE(-1, mprotect(p, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE));
}

TEST(mprotect, testExecuteFlatFileMapOpenedAsReadonly) {
  if (IsXnuSilicon()) return;  // TODO(jart): Use APE Loader SIP workaround?
  int (*p)(void);
  size_t n = sizeof(kRet31337);
  ASSERT_SYS(0, 3, creat("return31337", 0755));
  ASSERT_SYS(0, n, write(3, kRet31337, n));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("return31337", O_RDONLY));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, n, PROT_READ | PROT_EXEC, MAP_PRIVATE, 3, 0)));
  EXPECT_EQ(31337, p());
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, munmap(p, n));
}

TEST(mprotect, testFileMap_canChangeToExecWhileOpenInRdwrMode) {
  int (*p)(void);
  size_t n = sizeof(kRet31337);
  ASSERT_SYS(0, 3, open("return31337", O_CREAT | O_TRUNC | O_RDWR, 0755));
  ASSERT_SYS(0, n, write(3, kRet31337, n));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_PRIVATE, 3, 0)));
  EXPECT_NE(-1, mprotect(p, n, PROT_READ | PROT_EXEC));
  EXPECT_EQ(31337, p());
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 0, munmap(p, n));
}

TEST(mprotect, testBadProt_failsEinval) {
  volatile char *p = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  EXPECT_EQ(-1, mprotect((void *)p, 9999, -1));
  EXPECT_EQ(EINVAL, errno);
}

TEST(mprotect, testZeroSize_doesNothing) {
  volatile char *p = gc(memalign(getauxval(AT_PAGESZ), getauxval(AT_PAGESZ)));
  EXPECT_NE(-1, mprotect((void *)p, 0, PROT_READ));
  p[0] = 1;
  EXPECT_FALSE(gotsegv);
  EXPECT_FALSE(gotbusted);
}
