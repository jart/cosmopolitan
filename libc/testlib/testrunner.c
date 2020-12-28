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
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/nt/process.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

void SetUp(void);
void TearDown(void);

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
  for (fn = start; fn != end; ++fn) {
    if (weaken(SetUp)) weaken(SetUp)();
    errno = 0;
    SetLastError(0);
    getpid$sysv();
    if (warmup) warmup();
    testlib_clearxmmregisters();
    (*fn)();
    getpid$sysv();
    if (weaken(TearDown)) weaken(TearDown)();
  }
}
