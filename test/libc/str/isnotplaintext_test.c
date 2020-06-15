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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

#if 0
void *isnotplaintext(const void *, size_t)
    __attribute__((__pure__, __leaf__, __nothrow__));
#endif

TEST(isnotplaintext, test) {
  EXPECT_EQ(NULL, isnotplaintext(kHyperion, kHyperionSize));
  EXPECT_STREQ("", isnotplaintext(kHyperion, kHyperionSize + 1));
}

char *doit(char *data, size_t size) {
  data = isnotplaintext(data, size);
  asm volatile("" : "+r"(data));
  return data;
}

BENCH(isnotplaintext, bench) {
  EZBENCH(donothing, doit(kHyperion, kHyperionSize));
}
