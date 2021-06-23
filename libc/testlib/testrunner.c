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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

static int x;
static char cwd[PATH_MAX];
static char tmp[PATH_MAX];

void testlib_finish(void) {
  if (g_testlib_failed) {
    fprintf(stderr, "%u / %u %s\n", g_testlib_failed, g_testlib_ran,
            "tests failed");
  }
}

wontreturn void testlib_abort(void) {
  testlib_finish();
  exit(MIN(255, g_testlib_failed));
  unreachable;
}

static void SetupTmpDir(void) {
  snprintf(tmp, sizeof(tmp), "o/tmp/%s.%d.%d", program_invocation_short_name,
           getpid(), x++);
  CHECK_NE(-1, makedirs(tmp, 0755), "tmp=%s", tmp);
  CHECK_NE(-1, chdir(tmp), "tmp=%s", tmp);
}

static void TearDownTmpDir(void) {
  CHECK_NE(-1, chdir(cwd));
  CHECK_NE(-1, rmrf(tmp));
}

/**
 * Runs all test case functions in sorted order.
 */
testonly void testlib_runtestcases(testfn_t *start, testfn_t *end,
                                   testfn_t warmup) {
  /*
   * getpid() calls are inserted to help visually see tests in traces
   * which can be performed on Linux, FreeBSD, OpenBSD, and XNU:
   *
   *     strace -f o/default/test.com |& less
   *     truss o/default/test.com |& less
   *     ktrace -f trace o/default/test.com </dev/null; kdump -f trace | less
   *     dtruss o/default/test.com |& less
   *
   * Test cases are iterable via a decentralized section. Your TEST()
   * macro inserts .testcase.SUITENAME sections into the binary which
   * the linker sorts into an array.
   *
   * @see ape/ape.lds
   */
  const testfn_t *fn;
  CHECK_NOTNULL(getcwd(cwd, sizeof(cwd)));
  if (weaken(testlib_enable_tmp_setup_teardown_once)) SetupTmpDir();
  if (weaken(SetUpOnce)) weaken(SetUpOnce)();
  for (x = 0, fn = start; fn != end; ++fn) {
    if (weaken(testlib_enable_tmp_setup_teardown)) SetupTmpDir();
    if (weaken(SetUp)) weaken(SetUp)();
    errno = 0;
    SetLastError(0);
    sys_getpid();
    if (warmup) warmup();
    testlib_clearxmmregisters();
    (*fn)();
    sys_getpid();
    if (weaken(TearDown)) weaken(TearDown)();
    if (weaken(testlib_enable_tmp_setup_teardown)) TearDownTmpDir();
  }
  if (weaken(TearDownOnce)) weaken(TearDownOnce)();
  if (weaken(testlib_enable_tmp_setup_teardown_once)) TearDownTmpDir();
}
