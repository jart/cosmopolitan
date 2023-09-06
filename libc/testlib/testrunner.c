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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

static char g_olddir[PATH_MAX];
static char g_tmpdir[PATH_MAX];
static pthread_mutex_t testlib_error_lock;

void testlib_finish(void) {
  if (g_testlib_failed) {
    kprintf("%u / %u %s\n", g_testlib_failed, g_testlib_ran, "tests failed");
  }
}

void testlib_error_enter(const char *file, const char *func) {
  ftrace_enabled(-1);
  strace_enabled(-1);
  pthread_mutex_lock(&testlib_error_lock);
  if (!IsWindows()) sys_getpid(); /* make strace easier to read */
  if (!IsWindows()) sys_getpid();
  if (g_testlib_shoulddebugbreak) {
    DebugBreak();
  }
  testlib_showerror_file = file;
  testlib_showerror_func = func;
}

void testlib_error_leave(void) {
  strace_enabled(+1);
  ftrace_enabled(+1);
  pthread_mutex_unlock(&testlib_error_lock);
}

wontreturn void testlib_abort(void) {
  testlib_finish();
  _Exit(MAX(1, MIN(255, g_testlib_failed)));
}

static void SetupTmpDir(void) {
  char number[21];
  FormatInt64(number, _rand64() & INT64_MAX);
  g_tmpdir[0] = 0;
  if (*kTmpPath != '/') {
    strlcat(g_tmpdir, g_olddir, sizeof(g_tmpdir));
    strlcat(g_tmpdir, "/", sizeof(g_tmpdir));
  }
  strlcat(g_tmpdir, kTmpPath, sizeof(g_tmpdir));
  strlcat(g_tmpdir, program_invocation_short_name, sizeof(g_tmpdir));
  strlcat(g_tmpdir, ".", sizeof(g_tmpdir));
  strlcat(g_tmpdir, number, sizeof(g_tmpdir));
  if (makedirs(g_tmpdir, 0755) || chdir(g_tmpdir)) {
    perror(g_tmpdir);
    tinyprint(2, "testlib failed to setup tmpdir\n", NULL);
    exit(1);
  }
}

static void TearDownTmpDir(void) {
  if (chdir(g_olddir)) {
    perror(g_olddir);
    exit(1);
  }
  if (rmrf(g_tmpdir)) {
    perror(g_tmpdir);
    tinyprint(2, "testlib failed to tear down tmpdir\n", NULL);
    exit(1);
  }
}

/**
 * Runs all test case functions in sorted order.
 */
void testlib_runtestcases(const testfn_t *start, const testfn_t *end,
                          testfn_t warmup) {
  // getpid() calls are inserted to help visually see tests in traces
  // which can be performed on Linux, FreeBSD, OpenBSD, and XNU:
  //
  //     strace -f o/default/test.com |& less
  //     truss o/default/test.com |& less
  //     ktrace -f trace o/default/test.com </dev/null; kdump -f trace | less
  //     dtruss o/default/test.com |& less
  //
  // Test cases are iterable via a decentralized section. Your TEST()
  // macro inserts .testcase.SUITENAME sections into the binary which
  // the linker sorts into an array.
  //
  // @see ape/ape.lds
  const testfn_t *fn;
  if (_weaken(testlib_enable_tmp_setup_teardown) ||
      _weaken(testlib_enable_tmp_setup_teardown_once)) {
    if (!getcwd(g_olddir, sizeof(g_olddir))) {
      perror("getcwd");
      exit(1);
    }
  }
  if (_weaken(testlib_enable_tmp_setup_teardown_once)) {
    SetupTmpDir();
  }
  if (_weaken(SetUpOnce)) _weaken(SetUpOnce)();
  for (fn = start; fn != end; ++fn) {
    STRACE("");
    STRACE("# setting up %t", fn);
    if (_weaken(testlib_enable_tmp_setup_teardown)) SetupTmpDir();
    if (_weaken(SetUp)) _weaken(SetUp)();
    errno = 0;
    if (IsWindows()) {
      SetLastError(0);
    }
    if (!IsWindows()) sys_getpid();
    if (warmup) warmup();
    testlib_clearxmmregisters();
    STRACE("");
    STRACE("# running test %t", fn);
    (*fn)();
    STRACE("");
    STRACE("# tearing down %t", fn);
    if (!IsWindows()) sys_getpid();
    if (_weaken(TearDown)) _weaken(TearDown)();
    if (_weaken(testlib_enable_tmp_setup_teardown)) TearDownTmpDir();
  }
  if (_weaken(TearDownOnce)) {
    _weaken(TearDownOnce)();
  }
  if (_weaken(testlib_enable_tmp_setup_teardown_once)) {
    TearDownTmpDir();
  }
}
