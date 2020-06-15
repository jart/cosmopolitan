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
#include "libc/calls/calls.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/missioncritical.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

/**
 * Runs all TEST(), FIXTURE(), etc. recorded by ld.
 * @see libc/testlib/testlib.h
 * @see ape/ape.lds
 */
testonly void testlib_runalltests(void) {
  if ((intptr_t)__testcase_end > (intptr_t)__testcase_start) {
    if (testlib_countfixtures(__combo_start, __combo_end)) {
      testlib_runcombos(__testcase_start, __testcase_end, __combo_start,
                        __combo_end);
    } else {
      testlib_runtestcases(__testcase_start, __testcase_end, NULL);
      testlib_runfixtures(__testcase_start, __testcase_end, __fixture_start,
                          __fixture_end);
      testlib_finish();
    }
  }
}
