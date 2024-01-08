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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/asan.internal.h"
#include "libc/limits.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"
#include "net/http/escape.h"
#ifdef __x86_64__

#if 0
__static_yoink("backtrace.com");
__static_yoink("backtrace.com.dbg");

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown_once();
  ASSERT_NE(-1, mkdir("bin", 0755));
  testlib_extract("/zip/backtrace.com", "bin/backtrace.com", 0755);
  testlib_extract("/zip/backtrace.com.dbg", "bin/backtrace.com.dbg", 0755);
}

static bool OutputHasSymbol(const char *output, const char *s) {
  return strstr(output, s) || (!FindDebugBinary() && strstr(output, "NULL"));
}

// UNFREED MEMORY
// o/dbg/test/libc/log/backtrace_test.com
// max allocated space            655,360
// total allocated space               80
// total free space               327,600
// releasable space                     0
// mmaped space                    65,360
// non-mmapped space              327,680
//
// 100080040020 64 bytes 5 used
//       421871 strdup
//       416529 MemoryLeakCrash
//       41666d SetUp
//       45428c testlib_runtestcases
//
// 00007fff0000-000080010000 rw-pa-F 2x shadow of 000000000000
// 000080070000-0000800a0000 rw-pa-F 3x shadow of 0000003c0000
// 02008fff0000-020090020000 rw-pa-F 3x shadow of 10007ffc0000
// 020090060000-020090080000 rw-pa-F 2x shadow of 100080340000
// 0e007fff0000-0e0080010000 rw-pa-F 2x shadow of 6ffffffc0000
// 100006560000-100006580000 rw-pa-F 2x shadow of 7ffc32b40000
// 100080000000-100080050000 rw-pa-- 5x automap w/ 50 frame hole
// 100080370000-100080390000 rw-pa-- 2x automap w/ 1 frame hole
// 1000803a0000-1000803b0000 rw-pa-- 1x automap
// 6ffffffe0000-700000000000 rw-paSF 2x stack
// # 24 frames mapped w/ 51 frames gapped
TEST(ShowCrashReports, testMemoryLeakCrash) {
  size_t got;
  ssize_t rc;
  int ws, pid, fds[2];
  char *output, buf[512];
  if (!IsAsan()) {
    /* TODO(jart): How can we make this work without ASAN? */
    return;
  }
  ASSERT_NE(-1, pipe2(fds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv("bin/backtrace.com", (char *const[]){"bin/backtrace.com", "6", 0});
    _Exit(127);
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
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  // tinyprint(2, gc(IndentLines(output, -1, 0, 4)), "\n", NULL);
  EXPECT_EQ(78 << 8, ws);
  ASSERT_TRUE(!!strstr(output, "UNFREED MEMORY"));
  if (IsAsan()) {
    ASSERT_TRUE(OutputHasSymbol(output, "strdup") &&
                OutputHasSymbol(output, "MemoryLeakCrash"));
  }
  free(output);
}

// error: Uncaught SIGFPE (FPE_INTDIV) on nightmare pid 11724
//   /home/jart/cosmo/o/dbg/test/libc/log/backtrace_test.com.tmp.11721
//   ENOTTY[25]
//   Linux nightmare SMP Thu, 12 Aug 2021 06:16:45 UTC
//
// 0x0000000000414659: FpuCrash at test/libc/log/backtrace_test.c:35
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
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv("bin/backtrace.com", (char *const[]){"bin/backtrace.com", "1", 0});
    _Exit(127);
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
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  // tinyprint(2, gc(IndentLines(output, -1, 0, 4)), "\n", NULL);
  if (IsModeDbg()) {
    EXPECT_EQ(77 << 8, ws);
  } else {
    EXPECT_TRUE(WIFSIGNALED(ws));
    EXPECT_EQ(SIGFPE, WTERMSIG(ws));
  }
  /* NULL is stopgap until we can copy symbol tables into binary */
#ifdef __FNO_OMIT_FRAME_POINTER__
  ASSERT_TRUE(OutputHasSymbol(output, "FpuCrash"));
#endif
  if (strstr(output, "divrem overflow")) {
    // UBSAN handled it
  } else {
    // ShowCrashReports() handled it
    if (!strstr(output, gc(xasprintf("%d", pid)))) {
      fprintf(stderr, "ERROR: crash report didn't have pid\n%s\n",
              gc(IndentLines(output, -1, 0, 4)));
      __die();
    }
    if (!strstr(output, "SIGFPE")) {
      fprintf(stderr, "ERROR: crash report didn't have signal name\n%s\n",
              gc(IndentLines(output, -1, 0, 4)));
      __die();
    }
    // XXX: WSL doesn't save and restore x87 registers to ucontext_t
    if (!__iswsl1()) {
      if (!strstr(output, "3.141")) {
        fprintf(stderr, "ERROR: crash report didn't have fpu register\n%s\n",
                gc(IndentLines(output, -1, 0, 4)));
        __die();
      }
    }
    if (!strstr(output, "0f0e0d0c0b0a09080706050403020100")) {
      fprintf(stderr, "ERROR: crash report didn't have sse register\n%s\n",
              gc(IndentLines(output, -1, 0, 4)));
      __die();
    }
    if (!strstr(output, "3133731337")) {
      fprintf(stderr, "ERROR: crash report didn't have general register\n%s\n",
              gc(IndentLines(output, -1, 0, 4)));
      __die();
    }
  }
  free(output);
}

// clang-format off
//
// test/libc/log/backtrace_test.c:59: ubsan error: 'int' index 10 into 'char [10]' out of bounds
// 0x000000000040a352: __die at libc/log/die.c:40
// 0x0000000000489bc8: __ubsan_abort at libc/intrin/ubsan.c:196
// 0x0000000000489e1c: __ubsan_handle_out_of_bounds at libc/intrin/ubsan.c:242
// 0x0000000000423666: BssOverrunCrash at test/libc/log/backtrace_test.c:59
// 0x0000000000423e0a: SetUp at test/libc/log/backtrace_test.c:115
// 0x000000000049350b: testlib_runtestcases at libc/testlib/testrunner.c:98
// 0x000000000048ab50: testlib_runalltests at libc/testlib/runner.c:37
// 0x00000000004028d0: main at libc/testlib/testmain.c:94
// 0x0000000000403977: cosmo at libc/runtime/cosmo.S:69
// 0x00000000004021ae: _start at libc/crt/crt.S:78
//
// asan error: global redzone 1-byte store at 0x00000048cf2a shadow 0x0000800899e5
//                                         x
// ........................................OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//       |0      |0      |0      |0      |2      |-6     |-6     |-6     |-6
//                                ☺☻♥♦♣♠•◘○                                        
// 000000400000-000000460000 .text
// 000000460000-000000468000 .data
// 00007fff0000-00008000ffff
// 000080070000-00008009ffff ←shadow
// 02008fff0000-02009001ffff
// 020090060000-02009007ffff
// 0e007ffb0000-0e008000ffff
// 1000286b0000-1000286cffff
// 100080000000-10008009ffff
// 100080350000-10008036ffff
// 100080380000-10008038ffff
// 6fffffe00000-6fffffffffff
// 0x0000000000407af6: __die at libc/log/die.c:36
// 0x0000000000444f13: __asan_die at libc/intrin/asan.c:318
// 0x0000000000445bc8: __asan_report at libc/intrin/asan.c:667
// 0x0000000000445e41: __asan_report_memory_fault at libc/intrin/asan.c:672
// 0x0000000000446312: __asan_report_store at libc/intrin/asan.c:1008
// 0x0000000000444442: __asan_report_store1 at libc/intrin/somanyasan.S:118
// 0x0000000000416216: BssOverrunCrash at test/libc/log/backtrace_test.c:52
// 0x000000000041642a: SetUp at test/libc/log/backtrace_test.c:73
// 0x00000000004513eb: testlib_runtestcases at libc/testlib/testrunner.c:98
// 0x000000000044bbe0: testlib_runalltests at libc/testlib/runner.c:37
// 0x00000000004026db: main at libc/testlib/testmain.c:155
// 0x000000000040323f: cosmo at libc/runtime/cosmo.S:64
// 0x000000000040219b: _start at libc/crt/crt.S:67
//
// clang-format on

TEST(ShowCrashReports, testBssOverrunCrash) {
  if (!IsAsan()) return;
  size_t got;
  ssize_t rc;
  int ws, pid, fds[2];
  char *output, buf[512];
  ASSERT_NE(-1, pipe2(fds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv("bin/backtrace.com", (char *const[]){"bin/backtrace.com", "2", 0});
    _Exit(127);
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
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  // tinyprint(2, gc(IndentLines(output, -1, 0, 4)), "\n", NULL);
  EXPECT_EQ(77 << 8, ws);
  /* NULL is stopgap until we can copy symbol tablces into binary */
#ifdef __FNO_OMIT_FRAME_POINTER__
  ASSERT_TRUE(OutputHasSymbol(output, "BssOverrunCrash"));
#endif
  if (IsAsan()) {
    ASSERT_TRUE(
        !!strstr(output, "'int' index 10 into 'char [10]' out of bounds"));
  } else {
    ASSERT_TRUE(!!strstr(output, "☺☻♥♦♣♠•◘○"));
    ASSERT_TRUE(!!strstr(output, "global redzone"));
  }
  free(output);
}

// clang-format off
// asan error: null pointer dereference 1-byte load at 0x000000000000 shadow 0x00007fff8000
//                                         x
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅∅
// |-17    |-17    |-17    |-17    |-17    |-1     |-1     |-1     |-1     |-1
// ⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅⋅
// 000000400000-000000464000 .text
// 000000464000-00000046d000 .data
// 00007fff0000-00008000ffff ←shadow
// 000080070000-00008009ffff
// 02008fff0000-02009001ffff
// 020090060000-02009007ffff
// 0e007fff0000-0e008000ffff
// 10000d3e0000-10000d3fffff
// 100080000000-10008009ffff
// 100080370000-10008038ffff
// 1000803a0000-1000803affff
// 6ffffffe0000-6fffffffffff
// 0x0000000000407c84: __die at libc/log/die.c:37
// 0x000000000040b1ee: __asan_report_load at libc/intrin/asan.c:1083
// 0x000000000041639e: NpeCrash at test/libc/log/backtrace_test.c:87
// 0x0000000000416733: SetUp at test/libc/log/backtrace_test.c:120
// 0x00000000004541fb: testlib_runtestcases at libc/testlib/testrunner.c:98
// 0x000000000044d000: testlib_runalltests at libc/testlib/runner.c:37
// 0x00000000004026db: main at libc/testlib/testmain.c:94
// 0x000000000040327f: cosmo at libc/runtime/cosmo.S:64
// 0x000000000040219b: _start at libc/crt/crt.S:67
// clang-format on
TEST(ShowCrashReports, testNpeCrash) {
  if (!IsAsan()) return;
  size_t got;
  ssize_t rc;
  int ws, pid, fds[2];
  char *output, buf[512];
  ASSERT_NE(-1, pipe2(fds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv("bin/backtrace.com", (char *const[]){"bin/backtrace.com", "7", 0});
    _Exit(127);
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
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  // tinyprint(2, gc(IndentLines(output, -1, 0, 4)), "\n", NULL);
  EXPECT_EQ(77 << 8, ws);
  /* NULL is stopgap until we can copy symbol tables into binary */
  ASSERT_TRUE(!!strstr(output, "null pointer"));
#ifdef __FNO_OMIT_FRAME_POINTER__
  ASSERT_TRUE(OutputHasSymbol(output, "NpeCrash"));
#endif
  if (!strstr(output, "null pointer access")) {  // ubsan
    ASSERT_TRUE(!!strstr(output, "∅∅∅∅"));       // asan
  }
  free(output);
}

TEST(ShowCrashReports, testDataOverrunCrash) {
  if (!IsAsan()) return;
  size_t got;
  ssize_t rc;
  int ws, pid, fds[2];
  char *output, buf[512];
  ASSERT_NE(-1, pipe2(fds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv("bin/backtrace.com", (char *const[]){"bin/backtrace.com", "4", 0});
    _Exit(127);
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
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  // tinyprint(2, gc(IndentLines(output, -1, 0, 4)), "\n", NULL);
  EXPECT_EQ(77 << 8, ws);
  /* NULL is stopgap until we can copy symbol tablces into binary */
#ifdef __FNO_OMIT_FRAME_POINTER__
  ASSERT_TRUE(OutputHasSymbol(output, "DataOverrunCrash"));
#endif
  if (!strstr(output, "'int' index 10 into 'char [10]' out")) {  // ubsan
    ASSERT_TRUE(!!strstr(output, "☺☻♥♦♣♠•◘○"));                  // asan
    ASSERT_TRUE(!!strstr(output, "global redzone"));             // asan
  }
  free(output);
}

TEST(ShowCrashReports, testNpeCrashAfterFinalize) {
  /*
   * this test makes sure we're not doing things like depending on
   * environment variables after __cxa_finalize is called in cases
   * where putenv() is used
   */
  size_t got;
  ssize_t rc;
  int ws, pid, fds[2];
  char *output, buf[512];
  ASSERT_NE(-1, pipe2(fds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    dup2(fds[1], 1);
    dup2(fds[1], 2);
    execv("bin/backtrace.com", (char *const[]){"bin/backtrace.com", "8", 0});
    _Exit(127);
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
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  // tinyprint(2, gc(IndentLines(output, -1, 0, 4)), "\n", NULL);
  if (IsModeDbg()) {
    EXPECT_EQ(77 << 8, ws);
  } else {
    EXPECT_TRUE(WIFSIGNALED(ws));
    EXPECT_EQ(SIGSEGV, WTERMSIG(ws));
  }
  /* NULL is stopgap until we can copy symbol tables into binary */
  if (!strstr(output, IsAsan() ? "null pointer" : "Uncaught SIGSEGV (SEGV_")) {
    fprintf(stderr, "ERROR: crash report didn't diagnose the problem\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    __die();
  }
#ifdef __FNO_OMIT_FRAME_POINTER__
  if (!OutputHasSymbol(output, "NpeCrash")) {
    fprintf(stderr, "ERROR: crash report didn't have backtrace\n%s\n",
            gc(IndentLines(output, -1, 0, 4)));
    __die();
  }
#endif
  free(output);
}
#endif

#endif /* __x86_64__ */
