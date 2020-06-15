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
#include "libc/fmt/fmt.h"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

testonly int testlib_countfixtures(const struct TestFixture *start,
                                   const struct TestFixture *end) {
  return ((intptr_t)end - (intptr_t)start) / sizeof(struct TestFixture);
}

/**
 * Runs test cases for each FIXTURE() registered with the linker.
 * @see ape/ape.lds
 * @see libc/testlib/testlib.h
 */
testonly void testlib_runfixtures(testfn_t *test_start, testfn_t *test_end,
                                  const struct TestFixture *fixture_start,
                                  const struct TestFixture *fixture_end) {
  unsigned i, count;
  count = testlib_countfixtures(fixture_start, fixture_end);
  for (i = 0; i < count && !g_testlib_failed; ++i) {
    snprintf(g_fixturename, sizeof(g_fixturename), "%s_%s",
             fixture_start[i].group, fixture_start[i].name);
    __piro(PROT_READ | PROT_WRITE);
    fixture_start[i].fn();
    __piro(PROT_READ);
    testlib_runtestcases(test_start, test_end, NULL);
  }
}
