
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
#include "dsp/tty/tty.h"
#include "libc/mem/alg.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/str/str.h"

static int histcmp(const uint8_t *i1p, const uint8_t *i2p,
                   uint32_t histogram[hasatleast 256]) {
  if (histogram[*i1p] > histogram[*i2p]) {
    return -1;
  } else if (histogram[*i1p] < histogram[*i2p]) {
    return 1;
  } else {
    return 0;
  }
}

void ttyhisto(uint32_t histogram[hasatleast 256],
              uint8_t dominant[hasatleast 256], const uint8_t *xtcolors,
              const uint8_t *eqmask, size_t size) {
  size_t i;
  uint64_t q;
  memset(histogram, 0, sizeof(uint32_t) * 256);
  for (i = 0; i < size / 8; ++i) {
    memcpy(&q, &xtcolors[i * 8], 8);
    if (q == (xtcolors[i * 8] & 0xff) * 0x0101010101010101ul) {
      histogram[xtcolors[i * 8]]++;
    }
  }
  for (i = 0; i < 256; ++i) {
    dominant[i] = i;
  }
  qsort_r(dominant, 256, 1, (void *)histcmp, histogram);
}
