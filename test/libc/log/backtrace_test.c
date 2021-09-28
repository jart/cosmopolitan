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
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/escape.h"

typedef char xmm_t __attribute__((__vector_size__(16)));

noinline void ThisIsAnFpeCrash(void) {
  xmm_t v = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
             0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
  volatile int x = 0;
  asm volatile("fldpi");
  asm volatile("mov\t%0,%%r15" : /* no outputs */ : "g"(0x3133731337));
  asm volatile("movaps\t%0,%%xmm15" : /* no outputs */ : "x"(v));
  fputc(7 / x, stdout);
}

void SetUp(void) {
  ShowCrashReports();
  if (__argc == 2 && !strcmp(__argv[1], "1")) {
    ThisIsAnFpeCrash();
  }
}

// error: Uncaught SIGFPE (FPE_INTDIV) on nightmare pid 11724
//   /home/jart/cosmo/o/dbg/test/libc/log/backtrace_test.com.tmp.11721
//   ENOTTY[25]
//   Linux nightmare SMP Thu, 12 Aug 2021 06:16:45 UTC
//
// 0x0000000000414659: ThisIsAnFpeCrash at test/libc/log/backtrace_test.c:35
// 0x000000000045003b: testlib_runtestcases at libc/testlib/testrunner.c:98
// 0x000000000044b770: testlib_runalltests at libc/testlib/runner.c:37
// 0x000000000040278e: main at libc/testlib/testmain.c:86
// 0x0000000000403210: cosmo at libc/runtime/cosmo.S:65
// 0x0000000000402247: _start at libc/crt/crt.S:67
//
// RAX 0000000000000007 RBX 00006fffffffff10 RDI 00007ffe0745fde1 ST(0) 0.0
// RCX 0000000000000000 RDX 0000000000000000 RSI 0000000000489900 ST(1) 0.0
// RBP 00006fffffffff70 RSP 00006fffffffff10 RIP 000000000041465a ST(2) 0.0
//  R8 0000000000000001  R9 00006ffffffffcc0 R10 00006ffffffffe60 ST(3) 0.0
// R11 000000000000000d R12 00000dffffffffe2 R13 00006fffffffff10 ST(4) 0.0
// R14 0000000000000003 R15 000000000049b700 VF PF ZF IF
//
// XMM0  00000000000000000000000000000000 XMM8  00000000000000000000000000000000
// XMM1  000000008000000400000000004160ea XMM9  00000000000000000000000000000000
// XMM2  00000000000000000000000000000000 XMM10 00000000000000000000000000000000
// XMM3  00000000000000000000000000000000 XMM11 00000000000000000000000000000000
// XMM4  00000000000000000000000000000000 XMM12 00000000000000000000000000000000
// XMM5  00000000000000000000000000000000 XMM13 00000000000000000000000000000000
// XMM6  00000000000000000000000000000000 XMM14 00000000000000000000000000000000
// XMM7  00000000000000000000000000000000 XMM15 00000000000000000000000000000000
//
// mm->i == 4;
// mm->p[  0]=={0x00008007,0x00008008,-1,3,50}; /* 2 */
// /* 234,881,012 */
// mm->p[  1]=={0x0e007ffd,0x0e007fff,-1,3,50}; /* 3 */
// /* 33,538,280 */
// mm->p[  2]=={0x100040e8,0x100040e8,-1,3,50}; /* 1 */
// /* 1,610,596,103 */
// mm->p[  3]=={0x6ffffff0,0x6fffffff,12884901888,306,0}; /* 16 */
// /* 22 frames mapped w/ 1,879,015,395 frames gapped */
//
// 00400000-0045b000 r-xp 00000000 08:03 4587526
// /home/jart/cosmo/o/dbg/test/libc/log/backtrace_test.com.tmp.11721
// 0045b000-00461000 rw-p 0005b000 08:03 4587526
// /home/jart/cosmo/o/dbg/test/libc/log/backtrace_test.com.tmp.11721
// 00461000-004a0000 rw-p 00000000 00:00 0
// 80070000-80090000 rw-p 00000000 00:00 0
// e007ffd0000-e0080000000 rw-p 00000000 00:00 0
// 100040e80000-100040e90000 rw-p 00000000 00:00 0
// 6ffffff00000-700000000000 rw-p 00000000 00:00 0
// 7ffe0743f000-7ffe07460000 rw-p 00000000 00:00 0 [stack]
// 7ffe075a8000-7ffe075ab000 r--p 00000000 00:00 0 [vvar]
// 7ffe075ab000-7ffe075ac000 r-xp 00000000 00:00 0 [vdso]
//
// /home/jart/cosmo/o/dbg/test/libc/log/backtrace_test.com.tmp.11721 1

TEST(ShowCrashReports, testDivideByZero) {
  size_t got;
  ssize_t rc;
  int ws, pid, fds[2];
  char *output, buf[512];
  ASSERT_NE(-1, pipe2(fds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = vfork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv(program_executable_name,
          (char *const[]){program_executable_name, "1", 0});
    _exit(127);
  }
  close(fds[1]);
  output = 0;
  appends(&output, "");
  for (;;) {
    rc = read(fds[0], buf, sizeof(buf));
    if (rc == -1) {
      ASSERT_EQ(EINTR, errno);
      continue;
    }
    if ((got = rc)) {
      appendd(&output, buf, got);
    } else {
      break;
    }
  }
  ASSERT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(128 + SIGFPE, WEXITSTATUS(ws));

  /* NULL is stopgap until we can copy symbol tablces into binary */
  if (!strstr(output, "ThisIsAnFpeCrash") && !strstr(output, "NULL")) {
    fprintf(stderr, "ERROR: crash report didn't have backtrace\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    exit(1);
  }

  if (!strstr(output, gc(xasprintf("%d", pid)))) {
    fprintf(stderr, "ERROR: crash report didn't have pid\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    exit(1);
  }

  if (!strstr(output, "SIGFPE")) {
    fprintf(stderr, "ERROR: crash report didn't have signal name\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    exit(1);
  }

  if (!strstr(output, "3.141")) {
    fprintf(stderr, "ERROR: crash report didn't have fpu register\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    exit(1);
  }

  if (!strstr(output, "0f0e0d0c0b0a09080706050403020100")) {
    fprintf(stderr, "ERROR: crash report didn't have sse register\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    exit(1);
  }

  if (!strstr(output, "3133731337")) {
    fprintf(stderr, "ERROR: crash report didn't have general register\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    exit(1);
  }

  free(output);
}
