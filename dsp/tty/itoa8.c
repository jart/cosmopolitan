/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "dsp/tty/itoa8.h"
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
