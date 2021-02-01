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
#include "libc/str/internal.h"

/**
 * Hashes data with hardware acceleration at 10GBps.
 * @note needs Nehalem+ c. 2008 or Bulldozer+ c. 2011
 */
optimizespeed uint32_t crc32c$sse42(uint32_t init, const void *data, size_t n) {
  const unsigned char *p = (const unsigned char *)data;
  const unsigned char *pe = (const unsigned char *)data + n;
  uint32_t h = init ^ 0xffffffff;
  if (n >= 16 + 8) {
    while ((uintptr_t)p & 7) asm("crc32b\t%1,%0" : "+r"(h) : "rm"(*p++));
    uint64_t hl = h;
    while (p < pe - 16ul) {
      asm("crc32q\t%1,%0" : "+r"(hl) : "rm"(*(const uint64_t *)p));
      p += 8;
      asm("crc32q\t%1,%0" : "+r"(hl) : "rm"(*(const uint64_t *)p));
      p += 8;
    }
    h = (uint32_t)hl;
  }
  while (p < pe) asm("crc32b\t%1,%0" : "+r"(h) : "rm"(*p++));
  return h ^ 0xffffffff;
}

/*
  bench_crc32c$sse42 for #c per n where c ≈ 0.293ns
  N                     x1            x8           x64	  mBps
  ------------------------------------------------------------
  1                877.000        43.375        40.359      81
  1                 45.000        39.625        40.484      80
  2                 34.500        27.562        20.461     159
  3                 23.000        16.708        14.245     228
  4                 18.250        13.094        11.449     284
  7                 10.429         8.339         8.185     397
  8                 42.125         8.734         6.850     475
  15                 9.400         5.375         4.884     665
  16                 7.312         5.070         4.882     666
  31                 5.258         2.923         2.680    1213
  32                 3.969         2.676         2.562    1269
  63                 3.095         1.581         1.428    2276
  64                 2.234         1.623         1.478    2199
  127                1.205         0.901         0.900    3610
  128                1.164         0.960         0.915    3552
  255                0.922         0.651         0.618    5260
  256                0.715         0.650         0.609    5341
  511                0.558         0.482         0.477    6819
  512                0.529         0.475         0.469    6932
  1023               0.425         0.400         0.396    8204
  1024               0.417         0.392         0.388    8383
  2047               0.367         0.355         0.353    9199
  2048               0.374         0.366         0.364    8929
  4095               0.351         0.338         0.337    9644
  4096               0.353         0.338         0.338    9624
  8191               0.335         0.338         0.337    9641
  8192               0.335         0.329         0.329    9870
  16383              0.336         0.325         0.325   10011
  16384              0.336         0.326         0.375    8666
  32767              0.329         0.323         0.323   10070
  32768              0.327         0.324         0.323   10062
  65535              0.322         0.322         0.322   10103
  65536              0.321         0.322         0.322   10102
  131071             0.322         0.321         0.321   10125
  131072             0.321         0.321         0.321   10124
  262143             0.322         0.321         0.335    9699
  262144             0.321         0.321         0.321   10134
  524287             0.321         0.321         0.499    6516
  524288             0.321         0.321         0.339    9575
  1048575            0.322         0.321         0.322   10095
  1048576            0.320         1.001         0.323   10048
  2097151            0.325         0.321         0.322   10086
  2097152            0.330         0.320         0.323   10076
  4194303            0.331         0.322         0.321   10128
  4194304            0.332         0.321         0.325   10004
  8388607            0.334         0.332         0.331    9829
  8388608            0.334         0.329         0.327    9934
*/
