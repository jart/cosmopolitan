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
#include "libc/intrin/pshufd.h"
#include "libc/intrin/pshufhw.h"
#include "libc/intrin/pshuflw.h"
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

#define T(f, m) \
  f(a, x, m);   \
  (f)(b, x, m); \
  EXPECT_EQ(0, memcmp(a, b, 16))

TEST(pshuflw, test) {
  int i, j;
  int16_t x[8], a[8], b[8];
  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 8; ++j) x[j] = rand();
    T(pshuflw, 0b00000000);
    T(pshuflw, 0b00000001);
    T(pshuflw, 0b00000011);
    T(pshuflw, 0b00001100);
    T(pshuflw, 0b00001101);
    T(pshuflw, 0b00001111);
    T(pshuflw, 0b00110000);
    T(pshuflw, 0b00110001);
    T(pshuflw, 0b00110011);
    T(pshuflw, 0b00111100);
    T(pshuflw, 0b00111101);
    T(pshuflw, 0b00111111);
    T(pshuflw, 0b01000000);
    T(pshuflw, 0b01000001);
    T(pshuflw, 0b01000011);
    T(pshuflw, 0b01001100);
    T(pshuflw, 0b01001101);
    T(pshuflw, 0b01001111);
    T(pshuflw, 0b01110000);
    T(pshuflw, 0b01110001);
    T(pshuflw, 0b01110011);
    T(pshuflw, 0b01111100);
    T(pshuflw, 0b01111101);
    T(pshuflw, 0b01111111);
    T(pshuflw, 0b11000000);
    T(pshuflw, 0b11000001);
    T(pshuflw, 0b11000011);
    T(pshuflw, 0b11001100);
    T(pshuflw, 0b11001101);
    T(pshuflw, 0b11001111);
    T(pshuflw, 0b11110000);
    T(pshuflw, 0b11110001);
    T(pshuflw, 0b11110011);
    T(pshuflw, 0b11111100);
    T(pshuflw, 0b11111101);
    T(pshuflw, 0b11111111);
  }
}

TEST(pshufhw, test) {
  int i, j;
  int16_t x[8], a[8], b[8];
  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 8; ++j) x[j] = rand();
    T(pshufhw, 0b00000000);
    T(pshufhw, 0b00000001);
    T(pshufhw, 0b00000011);
    T(pshufhw, 0b00001100);
    T(pshufhw, 0b00001101);
    T(pshufhw, 0b00001111);
    T(pshufhw, 0b00110000);
    T(pshufhw, 0b00110001);
    T(pshufhw, 0b00110011);
    T(pshufhw, 0b00111100);
    T(pshufhw, 0b00111101);
    T(pshufhw, 0b00111111);
    T(pshufhw, 0b01000000);
    T(pshufhw, 0b01000001);
    T(pshufhw, 0b01000011);
    T(pshufhw, 0b01001100);
    T(pshufhw, 0b01001101);
    T(pshufhw, 0b01001111);
    T(pshufhw, 0b01110000);
    T(pshufhw, 0b01110001);
    T(pshufhw, 0b01110011);
    T(pshufhw, 0b01111100);
    T(pshufhw, 0b01111101);
    T(pshufhw, 0b01111111);
    T(pshufhw, 0b11000000);
    T(pshufhw, 0b11000001);
    T(pshufhw, 0b11000011);
    T(pshufhw, 0b11001100);
    T(pshufhw, 0b11001101);
    T(pshufhw, 0b11001111);
    T(pshufhw, 0b11110000);
    T(pshufhw, 0b11110001);
    T(pshufhw, 0b11110011);
    T(pshufhw, 0b11111100);
    T(pshufhw, 0b11111101);
    T(pshufhw, 0b11111111);
  }
}

TEST(pshufd, test) {
  int i, j;
  int32_t x[4], a[4], b[4];
  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 4; ++j) x[j] = rand();
    T(pshufd, 0b00000000);
    T(pshufd, 0b00000001);
    T(pshufd, 0b00000011);
    T(pshufd, 0b00001100);
    T(pshufd, 0b00001101);
    T(pshufd, 0b00001111);
    T(pshufd, 0b00110000);
    T(pshufd, 0b00110001);
    T(pshufd, 0b00110011);
    T(pshufd, 0b00111100);
    T(pshufd, 0b00111101);
    T(pshufd, 0b00111111);
    T(pshufd, 0b01000000);
    T(pshufd, 0b01000001);
    T(pshufd, 0b01000011);
    T(pshufd, 0b01001100);
    T(pshufd, 0b01001101);
    T(pshufd, 0b01001111);
    T(pshufd, 0b01110000);
    T(pshufd, 0b01110001);
    T(pshufd, 0b01110011);
    T(pshufd, 0b01111100);
    T(pshufd, 0b01111101);
    T(pshufd, 0b01111111);
    T(pshufd, 0b11000000);
    T(pshufd, 0b11000001);
    T(pshufd, 0b11000011);
    T(pshufd, 0b11001100);
    T(pshufd, 0b11001101);
    T(pshufd, 0b11001111);
    T(pshufd, 0b11110000);
    T(pshufd, 0b11110001);
    T(pshufd, 0b11110011);
    T(pshufd, 0b11111100);
    T(pshufd, 0b11111101);
    T(pshufd, 0b11111111);
  }
}
