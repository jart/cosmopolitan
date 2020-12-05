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
  /**
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
