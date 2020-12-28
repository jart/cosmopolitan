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
#include "libc/nexgen32e/crc32.h"

extern const uint32_t kCrc32cTab[256];

/**
 * Computes Castagnoli CRC-32 on old computers.
 */
uint32_t crc32c$pure(uint32_t init, const void *data, size_t size) {
  const unsigned char *p = data;
  uint32_t h = init ^ 0xffffffff;
  unsigned i;
  for (i = 0; i < size; i++) {
    h = h >> 8 ^ kCrc32cTab[(h & 0xff) ^ p[i]];
  }
  return h ^ 0xffffffff;
}

/*
  bench_crc32c$pure for #c per n where c ≈ 0.293ns
  N                     x1            x8           x64	mBps
  ------------------------------------------------------------
  1               4305.000        91.375        44.203      74
  1                 75.000        55.875        44.703      73
  2                 46.500        35.188        24.617     132
  3                 40.333        26.625        19.193     169
  4                 32.250        19.969        16.215     200
  7                 18.429        15.089        12.033     270
  8                 20.625        13.547        11.607     280
  15                15.667        10.775         9.589     339
  16                17.562        10.695         9.419     345
  31                12.226         8.891         8.317     391
  32                13.219         8.480         8.078     402
  63                 9.571         8.065         7.731     420
  64                 9.672         7.955         7.633     426
  127                8.433         7.548         7.329     443
  128                8.492         7.528         7.352     442
  255                7.557         7.366         7.239     449
  256                7.699         7.342         7.305     445
  511                7.376         7.243         7.223     450
  512                7.408         7.233         7.225     450
  1023               7.188         7.192         7.098     458
  1024               7.171         7.194         7.097     458
  2047               7.130         7.172         7.085     459
  2048               7.117         7.170         7.169     453
  4095               7.063         7.076         7.085     459
  4096               7.078         7.161         7.081     459
  8191               7.041         7.095         7.055     461
  8192               7.051         7.098         7.087     459
  16383              7.039         7.114         7.067     460
  16384              6.876         6.931         7.133     456
  32767              7.055         7.108         7.290     446
  32768              6.868         6.887         6.974     466
  65535              6.984         6.885         6.967     467
  65536              6.877         6.924        10.994     296
  131071             7.166         7.141         7.011     464
  131072             6.853         6.971         7.694     422
  262143             6.853         7.213         7.406     439
  262144             6.852         6.968         7.290     446
  524287             7.398         7.389         7.166     454
  524288             6.851         7.094         7.159     454
*/
