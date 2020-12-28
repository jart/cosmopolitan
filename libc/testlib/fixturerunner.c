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
    _piro(PROT_READ | PROT_WRITE);
    fixture_start[i].fn();
    _piro(PROT_READ);
    testlib_runtestcases(test_start, test_end, NULL);
  }
}
