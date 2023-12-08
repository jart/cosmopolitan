/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/dll.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/aspect.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"

struct Dll *testlib_aspects;
static pthread_mutex_t testlib_error_lock;

void testlib_finish(void) {
  char b1[12], b2[12];
  if (g_testlib_failed) {
    FormatInt32(b1, g_testlib_failed);
    FormatInt32(b2, g_testlib_ran);
    tinyprint(2, b1, " / ", b2, " tests failed\n", NULL);
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
  char host[64];
  struct Dll *e;
  const char *user;
  const testfn_t *fn;
  struct TestAspect *a;
  user = getenv("USER");
  strcpy(host, "unknown");
  gethostname(host, sizeof(host)), errno = 0;
  for (fn = start; fn != end; ++fn) {
    STRACE("");
    STRACE("# setting up %t", fn);
    for (e = dll_first(testlib_aspects); e; e = dll_next(testlib_aspects, e)) {
      a = TESTASPECT_CONTAINER(e);
      if (!a->once && a->setup) {
        a->setup(fn);
      }
    }
    if (_weaken(SetUp)) _weaken(SetUp)();
    errno = 0;
    if (IsWindows()) SetLastError(0);
    if (!IsWindows()) sys_getpid();
    if (warmup) warmup();
    testlib_clearxmmregisters();
    STRACE("");
    STRACE("# running test %t on %s@%s", fn, user, host);
    (*fn)();
    STRACE("");
    STRACE("# tearing down %t", fn);
    if (!IsWindows()) sys_getpid();
    if (_weaken(TearDown)) {
      _weaken(TearDown)();
    }
    for (e = dll_last(testlib_aspects); e; e = dll_prev(testlib_aspects, e)) {
      a = TESTASPECT_CONTAINER(e);
      if (!a->once && a->teardown) {
        a->teardown(fn);
      }
    }
  }
}
