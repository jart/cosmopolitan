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
#include "libc/calls/kntprioritycombos.internal.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/mlock.h"
#include "libc/testlib/testlib.h"

double g_avx2_juiceup_doubles_[4] aligned(32);
unsigned long long g_avx2_juiceup_quadwords_[4] aligned(32);

void testlib_benchwarmup(void) {
  /* get mathematical parts of cpu juiced up */
  if (X86_HAVE(AVX2) && X86_HAVE(FMA)) {
    asm("vmovdqa\t%1,%%ymm0\n\t"
        "vpmaddwd\t(%2),%%ymm0,%%ymm0\n\t"
        "vmovdqa\t%%ymm0,%0\n\t"
        "vzeroall"
        : "=m"(g_avx2_juiceup_quadwords_)
        : "m"(g_avx2_juiceup_quadwords_), "r"(&_base[0]));
    asm("vmovapd\t%1,%%ymm1\n\t"
        "vfmadd132pd\t(%2),%%ymm1,%%ymm1\n\t"
        "vmovapd\t%%ymm1,%0\n\t"
        "vzeroall"
        : "=m"(g_avx2_juiceup_doubles_)
        : "m"(g_avx2_juiceup_doubles_), "r"(&_base[32]));
  }
}

/**
 * Runs all benchmark functions in sorted order.
 *
 * @see BENCH()
 */
void testlib_runallbenchmarks(void) {
  peekall();
  mlockall(MCL_CURRENT);
  nice(-1);
  g_loglevel = kLogWarn;
  testlib_runtestcases(__bench_start, __bench_end, testlib_benchwarmup);
}
