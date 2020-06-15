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
#include "libc/macros.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

uint32_t crc32(uint32_t, const void *, int);
uint32_t crc32$pclmul(uint32_t, const void *, size_t);
uint32_t crc32$pclmul2(uint32_t, const void *, size_t);

TEST(crc32, testBigText) {
  size_t size;
  size = kHyperionSize;
  EXPECT_EQ(0xe9ded8e6, crc32(0, kHyperion, size));
  EXPECT_EQ(0xe9ded8e6, crc32_z(0, kHyperion, size));
  if (X86_HAVE(PCLMUL)) {
    size = ROUNDDOWN(size, 64);
    EXPECT_EQ(0xc7adc04f, crc32(0, kHyperion, size));
    EXPECT_EQ(0xc7adc04f, crc32_z(0, kHyperion, size));
    EXPECT_EQ(0xc7adc04f,
              0xffffffffu ^ crc32$pclmul(0 ^ 0xffffffffu, kHyperion, size));
  }
}
