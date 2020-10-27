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
#include "dsp/tty/itoa8.h"
#include "libc/bits/bits.h"
#include "libc/bits/initializer.h"
#include "libc/str/str.h"

struct Itoa8 kItoa8;

static textstartup void itoa8_init(void) {
  int i;
  uint8_t z;
  uint32_t w;
  for (i = 0; i < 256; ++i) {
    if (i < 10) {
      z = 1;
      w = '0' + i;
    } else if (i < 100) {
      z = 2;
      w = ('0' + i / 10) | ('0' + i % 10) << 8;
    } else {
      z = 3;
      w = ('0' + i / 100) | ('0' + i % 100 / 10) << 8 |
          ('0' + i % 100 % 10) << 16;
    }
    kItoa8.size[i] = z;
    kItoa8.data[i] = w;
  }
}

const void *const itoa8_init_ctor[] initarray = {itoa8_init};
