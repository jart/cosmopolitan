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
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

static void MmxPor(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] |= y[i];
  }
}

static void MmxPxor(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] ^= y[i];
  }
}

static void MmxPsubb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] -= y[i];
  }
}

static void MmxPaddb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] += y[i];
  }
}

static void MmxPand(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] &= y[i];
  }
}

static void MmxPandn(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = ~x[i] & y[i];
  }
}

static void MmxPavgb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = (x[i] + y[i] + 1) >> 1;
  }
}

static void MmxPabsb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = ABS((int8_t)y[i]);
  }
}

static void MmxPminub(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
}

static void MmxPmaxub(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
}

static void MmxPaddusb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = MIN(255, x[i] + y[i]);
  }
}

static void MmxPsubusb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = MIN(255, MAX(0, x[i] - y[i]));
  }
}

static void MmxPcmpeqb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = -(x[i] == y[i]);
  }
}

static void MmxPcmpgtb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = -((int8_t)x[i] > (int8_t)y[i]);
  }
}

static void MmxPsubsb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = MAX(-128, MIN(127, (int8_t)x[i] - (int8_t)y[i]));
  }
}

static void MmxPaddsb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    x[i] = MAX(-128, MIN(127, (int8_t)x[i] + (int8_t)y[i]));
  }
}

static void MmxPmulhrsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  int16_t a, b;
  for (i = 0; i < 4; ++i) {
    a = Read16(x + i * 2);
    b = Read16(y + i * 2);
    Write16(x + i * 2, (((a * b) >> 14) + 1) >> 1);
  }
}

static void MmxPmaddubsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2,
            MAX(-32768, MIN(32767, (x[i * 2 + 0] * (int8_t)y[i * 2 + 0] +
                                    x[i * 2 + 1] * (int8_t)y[i * 2 + 1]))));
  }
}

static void MmxPsraw(uint8_t x[8], unsigned k) {
  unsigned i;
  if (k > 15) k = 15;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, (int16_t)Read16(x + i * 2) >> k);
  }
}

static void MmxPsrad(uint8_t x[8], unsigned k) {
  unsigned i;
  if (k > 31) k = 31;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4, (int32_t)Read32(x + i * 4) >> k);
  }
}

static void MmxPsrlw(uint8_t x[8], unsigned k) {
  unsigned i;
  if (k < 16) {
    for (i = 0; i < 4; ++i) {
      Write16(x + i * 2, Read16(x + i * 2) >> k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsllw(uint8_t x[8], unsigned k) {
  unsigned i;
  if (k <= 15) {
    for (i = 0; i < 4; ++i) {
      Write16(x + i * 2, Read16(x + i * 2) << k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsrld(uint8_t x[8], unsigned k) {
  unsigned i;
  if (k <= 31) {
    for (i = 0; i < 2; ++i) {
      Write32(x + i * 4, Read32(x + i * 4) >> k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPslld(uint8_t x[8], unsigned k) {
  unsigned i;
  if (k <= 31) {
    for (i = 0; i < 2; ++i) {
      Write32(x + i * 4, Read32(x + i * 4) << k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsrlq(uint8_t x[8], unsigned k) {
  if (k <= 63) {
    Write64(x, Read64(x) >> k);
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsllq(uint8_t x[8], unsigned k) {
  if (k <= 63) {
    Write64(x, Read64(x) << k);
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPslldq(uint8_t x[8], unsigned k) {
  unsigned i;
  uint8_t t[8];
  if (k > 8) k = 8;
  for (i = 0; i < k; ++i) t[i] = 0;
  for (i = 0; i < 8 - k; ++i) t[k + i] = x[i];
  memcpy(x, t, 8);
}

static void MmxPsrldq(uint8_t x[8], unsigned k) {
  uint8_t t[8];
  if (k > 8) k = 8;
  memcpy(t, x + k, 8 - k);
  memset(t + (8 - k), 0, k);
  memcpy(x, t, 8);
}

static void MmxPalignr(uint8_t x[8], const uint8_t y[8], unsigned k) {
  uint8_t t[24];
  memcpy(t, y, 8);
  memcpy(t + 8, x, 8);
  memset(t + 16, 0, 8);
  memcpy(x, t + MIN(k, 16), 8);
}

static void MmxPsubw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, Read16(x + i * 2) - Read16(y + i * 2));
  }
}

static void MmxPaddw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, Read16(x + i * 2) + Read16(y + i * 2));
  }
}

static void MmxPsubd(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4, Read32(x + i * 4) - Read32(y + i * 4));
  }
}

static void MmxPaddd(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4, Read32(x + i * 4) + Read32(y + i * 4));
  }
}

static void MmxPaddq(uint8_t x[8], const uint8_t y[8]) {
  Write64(x, Read64(x) + Read64(y));
}

static void MmxPsubq(uint8_t x[8], const uint8_t y[8]) {
  Write64(x, Read64(x) - Read64(y));
}

static void MmxPaddsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + i * 2) +
                                               (int16_t)Read16(y + i * 2)))));
  }
}

static void MmxPsubsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + i * 2) -
                                               (int16_t)Read16(y + i * 2)))));
  }
}

static void MmxPaddusw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, MIN(65535, Read16(x + i * 2) + Read16(y + i * 2)));
  }
}

static void MmxPsubusw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2,
            MIN(65535, MAX(0, Read16(x + i * 2) - Read16(y + i * 2))));
  }
}

static void MmxPminsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2,
            MIN((int16_t)Read16(x + i * 2), (int16_t)Read16(y + i * 2)));
  }
}

static void MmxPmaxsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2,
            MAX((int16_t)Read16(x + i * 2), (int16_t)Read16(y + i * 2)));
  }
}

static void MmxPackuswb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint8_t t[8];
  for (i = 0; i < 4; ++i) {
    t[i + 0] = MIN(255, MAX(0, (int16_t)Read16(x + i * 2)));
  }
  for (i = 0; i < 4; ++i) {
    t[i + 4] = MIN(255, MAX(0, (int16_t)Read16(y + i * 2)));
  }
  memcpy(x, t, 8);
}

static void MmxPacksswb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint8_t t[8];
  for (i = 0; i < 4; ++i) {
    t[i + 0] = MAX(-128, MIN(127, (int16_t)Read16(x + i * 2)));
  }
  for (i = 0; i < 4; ++i) {
    t[i + 4] = MAX(-128, MIN(127, (int16_t)Read16(y + i * 2)));
  }
  memcpy(x, t, 8);
}

static void MmxPackssdw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint8_t t[8];
  for (i = 0; i < 2; ++i) {
    Write16(t + i * 2 + 0, MAX(-32768, MIN(32767, (int32_t)Read32(x + i * 4))));
  }
  for (i = 0; i < 2; ++i) {
    Write16(t + i * 2 + 4, MAX(-32768, MIN(32767, (int32_t)Read32(y + i * 4))));
  }
  memcpy(x, t, 8);
}

static void MmxPcmpgtw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2,
            -((int16_t)Read16(x + i * 2) > (int16_t)Read16(y + i * 2)));
  }
}

static void MmxPcmpeqw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, -(Read16(x + i * 2) == Read16(y + i * 2)));
  }
}

static void MmxPcmpgtd(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4,
            -((int32_t)Read32(x + i * 4) > (int32_t)Read32(y + i * 4)));
  }
}

static void MmxPcmpeqd(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4, -(Read32(x + i * 4) == Read32(y + i * 4)));
  }
}

static void MmxPsrawv(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint64_t k;
  k = Read64(y);
  if (k > 15) k = 15;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, (int16_t)Read16(x + i * 2) >> k);
  }
}

static void MmxPsradv(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint64_t k;
  k = Read64(y);
  if (k > 31) k = 31;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4, (int32_t)Read32(x + i * 4) >> k);
  }
}

static void MmxPsrlwv(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint64_t k;
  k = Read64(y);
  if (k < 16) {
    for (i = 0; i < 4; ++i) {
      Write16(x + i * 2, Read16(x + i * 2) >> k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsllwv(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint64_t k;
  k = Read64(y);
  if (k < 16) {
    for (i = 0; i < 4; ++i) {
      Write16(x + i * 2, Read16(x + i * 2) << k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsrldv(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint64_t k;
  k = Read64(y);
  if (k < 32) {
    for (i = 0; i < 2; ++i) {
      Write32(x + i * 4, Read32(x + i * 4) >> k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPslldv(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint64_t k;
  k = Read64(y);
  if (k < 32) {
    for (i = 0; i < 2; ++i) {
      Write32(x + i * 4, Read32(x + i * 4) << k);
    }
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsrlqv(uint8_t x[8], const uint8_t y[8]) {
  uint64_t k;
  k = Read64(y);
  if (k < 64) {
    Write64(x, Read64(x) >> k);
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPsllqv(uint8_t x[8], const uint8_t y[8]) {
  uint64_t k;
  k = Read64(y);
  if (k < 64) {
    Write64(x, Read64(x) << k);
  } else {
    memset(x, 0, 8);
  }
}

static void MmxPavgw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, (Read16(x + i * 2) + Read16(y + i * 2) + 1) >> 1);
  }
}

static void MmxPsadbw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i, s, t;
  for (s = i = 0; i < 4; ++i) s += ABS(x[i] - y[i]);
  for (t = 0; i < 8; ++i) t += ABS(x[i] - y[i]);
  Write32(x + 0, s);
  Write32(x + 4, t);
}

static void MmxPmaddwd(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4,
            ((int16_t)Read16(x + i * 4 + 0) * (int16_t)Read16(y + i * 4 + 0) +
             (int16_t)Read16(x + i * 4 + 2) * (int16_t)Read16(y + i * 4 + 2)));
  }
}

static void MmxPmulhuw(uint8_t x[8], const uint8_t y[8]) {
  uint32_t v;
  unsigned i;
  for (i = 0; i < 4; ++i) {
    v = Read16(x + i * 2);
    v *= Read16(y + i * 2);
    v >>= 16;
    Write16(x + i * 2, v);
  }
}

static void MmxPmulhw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2,
            ((int16_t)Read16(x + i * 2) * (int16_t)Read16(y + i * 2)) >> 16);
  }
}

static void MmxPmuludq(uint8_t x[8], const uint8_t y[8]) {
  Write64(x, (uint64_t)Read32(x) * Read32(y));
}

static void MmxPmullw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, (int16_t)Read16(x + i * 2) * (int16_t)Read16(y + i * 2));
  }
}

static void MmxPmulld(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write32(x + i * 4, Read32(x + i * 4) * Read32(y + i * 4));
  }
}

static void MmxPshufb(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  uint8_t t[8];
  for (i = 0; i < 8; ++i) {
    t[i] = (y[i] & 128) ? 0 : x[y[i] & 7];
  }
  memcpy(x, t, 8);
}

static void MmxPsignb(uint8_t x[8], const uint8_t y[8]) {
  int v;
  unsigned i;
  for (i = 0; i < 8; ++i) {
    v = (int8_t)y[i];
    if (!v) {
      x[i] = 0;
    } else if (v < 0) {
      x[i] = -(int8_t)x[i];
    }
  }
}

static void MmxPsignw(uint8_t x[8], const uint8_t y[8]) {
  int v;
  unsigned i;
  for (i = 0; i < 4; ++i) {
    v = (int16_t)Read16(y + i * 2);
    if (!v) {
      Write16(x + i * 2, 0);
    } else if (v < 0) {
      Write16(x + i * 2, -(int16_t)Read16(x + i * 2));
    }
  }
}

static void MmxPsignd(uint8_t x[8], const uint8_t y[8]) {
  int32_t v;
  unsigned i;
  for (i = 0; i < 2; ++i) {
    v = Read32(y + i * 4);
    if (!v) {
      Write32(x + i * 4, 0);
    } else if (v < 0) {
      Write32(x + i * 4, -Read32(x + i * 4));
    }
  }
}

static void MmxPabsw(uint8_t x[8], const uint8_t y[8]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write16(x + i * 2, ABS((int16_t)Read16(y + i * 2)));
  }
}

static void MmxPabsd(uint8_t x[8], const uint8_t y[8]) {
  int32_t v;
  unsigned i;
  for (i = 0; i < 2; ++i) {
    v = Read32(y + i * 4);
    Write32(x + i * 4, v >= 0 ? v : -(uint32_t)v);
  }
}

static void MmxPhaddw(uint8_t x[8], const uint8_t y[8]) {
  uint8_t t[8];
  Write16(t + 0 * 2, Read16(x + 0 * 2) + Read16(x + 1 * 2));
  Write16(t + 1 * 2, Read16(x + 2 * 2) + Read16(x + 3 * 2));
  Write16(t + 2 * 2, Read16(y + 0 * 2) + Read16(y + 1 * 2));
  Write16(t + 3 * 2, Read16(y + 2 * 2) + Read16(y + 3 * 2));
  memcpy(x, t, 8);
}

static void MmxPhsubw(uint8_t x[8], const uint8_t y[8]) {
  uint8_t t[8];
  Write16(t + 0 * 2, Read16(x + 0 * 2) - Read16(x + 1 * 2));
  Write16(t + 1 * 2, Read16(x + 2 * 2) - Read16(x + 3 * 2));
  Write16(t + 2 * 2, Read16(y + 0 * 2) - Read16(y + 1 * 2));
  Write16(t + 3 * 2, Read16(y + 2 * 2) - Read16(y + 3 * 2));
  memcpy(x, t, 8);
}

static void MmxPhaddd(uint8_t x[8], const uint8_t y[8]) {
  uint8_t t[8];
  Write32(t + 0 * 4, Read32(x + 0 * 4) + Read32(x + 1 * 4));
  Write32(t + 1 * 4, Read32(y + 0 * 4) + Read32(y + 1 * 4));
  memcpy(x, t, 8);
}

static void MmxPhsubd(uint8_t x[8], const uint8_t y[8]) {
  uint8_t t[8];
  Write32(t + 0 * 4, Read32(x + 0 * 4) - Read32(x + 1 * 4));
  Write32(t + 1 * 4, Read32(y + 0 * 4) - Read32(y + 1 * 4));
  memcpy(x, t, 8);
}

static void MmxPhaddsw(uint8_t x[8], const uint8_t y[8]) {
  uint8_t t[8];
  Write16(t + 0 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 0 * 2) +
                                             (int16_t)Read16(x + 1 * 2)))));
  Write16(t + 1 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 2 * 2) +
                                             (int16_t)Read16(x + 3 * 2)))));
  Write16(t + 2 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 0 * 2) +
                                             (int16_t)Read16(y + 1 * 2)))));
  Write16(t + 3 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 2 * 2) +
                                             (int16_t)Read16(y + 3 * 2)))));
  memcpy(x, t, 8);
}

static void MmxPhsubsw(uint8_t x[8], const uint8_t y[8]) {
  uint8_t t[8];
  Write16(t + 0 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 0 * 2) -
                                             (int16_t)Read16(x + 1 * 2)))));
  Write16(t + 1 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 2 * 2) -
                                             (int16_t)Read16(x + 3 * 2)))));
  Write16(t + 2 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 0 * 2) -
                                             (int16_t)Read16(x + 1 * 2)))));
  Write16(t + 3 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 2 * 2) -
                                             (int16_t)Read16(y + 3 * 2)))));
  memcpy(x, t, 8);
}

static void MmxPunpcklbw(uint8_t x[8], const uint8_t y[8]) {
  x[7] = y[3];
  x[6] = x[3];
  x[5] = y[2];
  x[4] = x[2];
  x[3] = y[1];
  x[2] = x[1];
  x[1] = y[0];
  x[0] = x[0];
}

static void MmxPunpckhbw(uint8_t x[8], const uint8_t y[8]) {
  x[0] = x[4];
  x[1] = y[4];
  x[2] = x[5];
  x[3] = y[5];
  x[4] = x[6];
  x[5] = y[6];
  x[6] = x[7];
  x[7] = y[7];
}

static void MmxPunpcklwd(uint8_t x[8], const uint8_t y[8]) {
  x[6] = y[2];
  x[7] = y[3];
  x[4] = x[2];
  x[5] = x[3];
  x[2] = y[0];
  x[3] = y[1];
  x[0] = x[0];
  x[1] = x[1];
}

static void MmxPunpckldq(uint8_t x[8], const uint8_t y[8]) {
  x[4] = y[0];
  x[5] = y[1];
  x[6] = y[2];
  x[7] = y[3];
  x[0] = x[0];
  x[1] = x[1];
  x[2] = x[2];
  x[3] = x[3];
}

static void MmxPunpckhwd(uint8_t x[8], const uint8_t y[8]) {
  x[0] = x[4];
  x[1] = x[5];
  x[2] = y[4];
  x[3] = y[5];
  x[4] = x[6];
  x[5] = x[7];
  x[6] = y[6];
  x[7] = y[7];
}

static void MmxPunpckhdq(uint8_t x[8], const uint8_t y[8]) {
  x[0] = x[4];
  x[1] = x[5];
  x[2] = x[6];
  x[3] = x[7];
  x[4] = y[4];
  x[5] = y[5];
  x[6] = y[6];
  x[7] = y[7];
}

static void MmxPunpcklqdq(uint8_t x[8], const uint8_t y[8]) {
}

static void MmxPunpckhqdq(uint8_t x[8], const uint8_t y[8]) {
}

static void SsePsubb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] -= y[i];
  }
}

static void SsePaddb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] += y[i];
  }
}

static void SsePor(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] |= y[i];
  }
}

static void SsePxor(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] ^= y[i];
  }
}

static void SsePand(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] &= y[i];
  }
}

static void SsePandn(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = ~x[i] & y[i];
  }
}

static void SsePcmpeqb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = -(x[i] == y[i]);
  }
}

static void SsePcmpgtb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = -((int8_t)x[i] > (int8_t)y[i]);
  }
}

static void SsePavgb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = (x[i] + y[i] + 1) >> 1;
  }
}

static void SsePabsb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = ABS((int8_t)y[i]);
  }
}

static void SsePminub(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = MIN(x[i], y[i]);
  }
}

static void SsePmaxub(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 16; ++i) {
    x[i] = MAX(x[i], y[i]);
  }
}

static void SsePslldq(uint8_t x[16], unsigned k) {
  unsigned i;
  uint8_t t[16];
  if (k > 16) k = 16;
  for (i = 0; i < k; ++i) t[i] = 0;
  for (i = 0; i < 16 - k; ++i) t[k + i] = x[i];
  memcpy(x, t, 16);
}

static void SsePsrldq(uint8_t x[16], unsigned k) {
  uint8_t t[16];
  if (k > 16) k = 16;
  memcpy(t, x + k, 16 - k);
  memset(t + (16 - k), 0, k);
  memcpy(x, t, 16);
}

static void SsePalignr(uint8_t x[16], const uint8_t y[16], unsigned k) {
  uint8_t t[48];
  memcpy(t, y, 16);
  memcpy(t + 16, x, 16);
  memset(t + 32, 0, 16);
  memcpy(x, t + MIN(k, 32), 16);
}

static void SsePsubw(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    Write16(x + i * 2, Read16(x + i * 2) - Read16(y + i * 2));
  }
}

static void SsePaddw(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    Write16(x + i * 2, Read16(x + i * 2) + Read16(y + i * 2));
  }
}

static void SsePsubd(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write32(x + i * 4, Read32(x + i * 4) - Read32(y + i * 4));
  }
}

static void SsePaddd(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 4; ++i) {
    Write32(x + i * 4, Read32(x + i * 4) + Read32(y + i * 4));
  }
}

static void SsePaddq(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write64(x + i * 8, Read64(x + i * 8) + Read64(y + i * 8));
  }
}

static void SsePsubq(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write64(x + i * 8, Read64(x + i * 8) - Read64(y + i * 8));
  }
}

static void SsePaddusw(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 8; ++i) {
    Write16(x + i * 2, MIN(65535, Read16(x + i * 2) + Read16(y + i * 2)));
  }
}

static void SsePackuswb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  uint8_t t[16];
  for (i = 0; i < 8; ++i) {
    t[i + 0] = MIN(255, MAX(0, (int16_t)Read16(x + i * 2)));
  }
  for (i = 0; i < 8; ++i) {
    t[i + 8] = MIN(255, MAX(0, (int16_t)Read16(y + i * 2)));
  }
  memcpy(x, t, 16);
}

static void SsePacksswb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  uint8_t t[16];
  for (i = 0; i < 8; ++i) {
    t[i + 0] = MAX(-128, MIN(127, (int16_t)Read16(x + i * 2)));
  }
  for (i = 0; i < 8; ++i) {
    t[i + 8] = MAX(-128, MIN(127, (int16_t)Read16(y + i * 2)));
  }
  memcpy(x, t, 16);
}

static void SsePackssdw(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  uint8_t t[16];
  for (i = 0; i < 4; ++i) {
    Write16(t + i * 2 + 0, MAX(-32768, MIN(32767, (int32_t)Read32(x + i * 4))));
  }
  for (i = 0; i < 4; ++i) {
    Write16(t + i * 2 + 8, MAX(-32768, MIN(32767, (int32_t)Read32(y + i * 4))));
  }
  memcpy(x, t, 16);
}

static void SsePsadbw(uint8_t x[16], const uint8_t y[16]) {
  unsigned i, s, t;
  for (s = i = 0; i < 8; ++i) s += ABS(x[i] - y[i]);
  for (t = 0; i < 16; ++i) t += ABS(x[i] - y[i]);
  Write64(x + 0, s);
  Write64(x + 8, t);
}

static void SsePmuludq(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  for (i = 0; i < 2; ++i) {
    Write64(x + i * 8, (uint64_t)Read32(x + i * 8) * Read32(y + i * 8));
  }
}

static void SsePshufb(uint8_t x[16], const uint8_t y[16]) {
  unsigned i;
  uint8_t t[16];
  for (i = 0; i < 16; ++i) {
    t[i] = (y[i] & 128) ? 0 : x[y[i] & 15];
  }
  memcpy(x, t, 16);
}

static void SsePhaddd(uint8_t x[16], const uint8_t y[16]) {
  uint8_t t[16];
  Write32(t + 0 * 4, Read32(x + 0 * 4) + Read32(x + 1 * 4));
  Write32(t + 1 * 4, Read32(x + 2 * 4) + Read32(x + 3 * 4));
  Write32(t + 2 * 4, Read32(y + 0 * 4) + Read32(y + 1 * 4));
  Write32(t + 3 * 4, Read32(y + 2 * 4) + Read32(y + 3 * 4));
  memcpy(x, t, 16);
}

static void SsePhsubd(uint8_t x[16], const uint8_t y[16]) {
  uint8_t t[16];
  Write32(t + 0 * 4, Read32(x + 0 * 4) - Read32(x + 1 * 4));
  Write32(t + 1 * 4, Read32(x + 2 * 4) - Read32(x + 3 * 4));
  Write32(t + 2 * 4, Read32(y + 0 * 4) - Read32(y + 1 * 4));
  Write32(t + 3 * 4, Read32(y + 2 * 4) - Read32(y + 3 * 4));
  memcpy(x, t, 16);
}

static void SsePhaddw(uint8_t x[16], const uint8_t y[16]) {
  uint8_t t[16];
  Write16(t + 0 * 2, Read16(x + 0 * 2) + Read16(x + 1 * 2));
  Write16(t + 1 * 2, Read16(x + 2 * 2) + Read16(x + 3 * 2));
  Write16(t + 2 * 2, Read16(x + 4 * 2) + Read16(x + 5 * 2));
  Write16(t + 3 * 2, Read16(x + 6 * 2) + Read16(x + 7 * 2));
  Write16(t + 4 * 2, Read16(y + 0 * 2) + Read16(y + 1 * 2));
  Write16(t + 5 * 2, Read16(y + 2 * 2) + Read16(y + 3 * 2));
  Write16(t + 6 * 2, Read16(y + 4 * 2) + Read16(y + 5 * 2));
  Write16(t + 7 * 2, Read16(y + 6 * 2) + Read16(y + 7 * 2));
  memcpy(x, t, 16);
}

static void SsePhsubw(uint8_t x[16], const uint8_t y[16]) {
  uint8_t t[16];
  Write16(t + 0 * 2, Read16(x + 0 * 2) - Read16(x + 1 * 2));
  Write16(t + 1 * 2, Read16(x + 2 * 2) - Read16(x + 3 * 2));
  Write16(t + 2 * 2, Read16(x + 4 * 2) - Read16(x + 5 * 2));
  Write16(t + 3 * 2, Read16(x + 6 * 2) - Read16(x + 7 * 2));
  Write16(t + 4 * 2, Read16(y + 0 * 2) - Read16(y + 1 * 2));
  Write16(t + 5 * 2, Read16(y + 2 * 2) - Read16(y + 3 * 2));
  Write16(t + 6 * 2, Read16(y + 4 * 2) - Read16(y + 5 * 2));
  Write16(t + 7 * 2, Read16(y + 6 * 2) - Read16(y + 7 * 2));
  memcpy(x, t, 16);
}

static void SsePhaddsw(uint8_t x[16], const uint8_t y[16]) {
  uint8_t t[16];
  Write16(t + 0 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 0 * 2) +
                                             (int16_t)Read16(x + 1 * 2)))));
  Write16(t + 1 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 2 * 2) +
                                             (int16_t)Read16(x + 3 * 2)))));
  Write16(t + 2 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 4 * 2) +
                                             (int16_t)Read16(x + 5 * 2)))));
  Write16(t + 3 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 6 * 2) +
                                             (int16_t)Read16(x + 7 * 2)))));
  Write16(t + 4 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 0 * 2) +
                                             (int16_t)Read16(y + 1 * 2)))));
  Write16(t + 5 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 2 * 2) +
                                             (int16_t)Read16(y + 3 * 2)))));
  Write16(t + 6 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 4 * 2) +
                                             (int16_t)Read16(y + 5 * 2)))));
  Write16(t + 7 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 6 * 2) +
                                             (int16_t)Read16(y + 7 * 2)))));
  memcpy(x, t, 16);
}

static void SsePhsubsw(uint8_t x[16], const uint8_t y[16]) {
  uint8_t t[16];
  Write16(t + 0 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 0 * 2) -
                                             (int16_t)Read16(x + 1 * 2)))));
  Write16(t + 1 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 2 * 2) -
                                             (int16_t)Read16(x + 3 * 2)))));
  Write16(t + 2 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 4 * 2) -
                                             (int16_t)Read16(x + 5 * 2)))));
  Write16(t + 3 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(x + 6 * 2) -
                                             (int16_t)Read16(x + 7 * 2)))));
  Write16(t + 4 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 0 * 2) -
                                             (int16_t)Read16(y + 1 * 2)))));
  Write16(t + 5 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 2 * 2) -
                                             (int16_t)Read16(y + 3 * 2)))));
  Write16(t + 6 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 4 * 2) -
                                             (int16_t)Read16(y + 5 * 2)))));
  Write16(t + 7 * 2, MAX(-32768, MIN(32767, ((int16_t)Read16(y + 6 * 2) -
                                             (int16_t)Read16(y + 7 * 2)))));
  memcpy(x, t, 16);
}

static void SsePunpcklbw(uint8_t x[16], const uint8_t y[16]) {
  x[0xf] = y[0x7];
  x[0xe] = x[0x7];
  x[0xd] = y[0x6];
  x[0xc] = x[0x6];
  x[0xb] = y[0x5];
  x[0xa] = x[0x5];
  x[0x9] = y[0x4];
  x[0x8] = x[0x4];
  x[0x7] = y[0x3];
  x[0x6] = x[0x3];
  x[0x5] = y[0x2];
  x[0x4] = x[0x2];
  x[0x3] = y[0x1];
  x[0x2] = x[0x1];
  x[0x1] = y[0x0];
  x[0x0] = x[0x0];
}

static void SsePunpckhbw(uint8_t x[16], const uint8_t y[16]) {
  x[0x0] = x[0x8];
  x[0x1] = y[0x8];
  x[0x2] = x[0x9];
  x[0x3] = y[0x9];
  x[0x4] = x[0xa];
  x[0x5] = y[0xa];
  x[0x6] = x[0xb];
  x[0x7] = y[0xb];
  x[0x8] = x[0xc];
  x[0x9] = y[0xc];
  x[0xa] = x[0xd];
  x[0xb] = y[0xd];
  x[0xc] = x[0xe];
  x[0xd] = y[0xe];
  x[0xe] = x[0xf];
  x[0xf] = y[0xf];
}

static void SsePunpcklwd(uint8_t x[16], const uint8_t y[16]) {
  x[0xe] = y[0x6];
  x[0xf] = y[0x7];
  x[0xc] = x[0x6];
  x[0xd] = x[0x7];
  x[0xa] = y[0x4];
  x[0xb] = y[0x5];
  x[0x8] = x[0x4];
  x[0x9] = x[0x5];
  x[0x6] = y[0x2];
  x[0x7] = y[0x3];
  x[0x4] = x[0x2];
  x[0x5] = x[0x3];
  x[0x2] = y[0x0];
  x[0x3] = y[0x1];
  x[0x0] = x[0x0];
  x[0x1] = x[0x1];
}

static void SsePunpckldq(uint8_t x[16], const uint8_t y[16]) {
  x[0xc] = y[0x4];
  x[0xd] = y[0x5];
  x[0xe] = y[0x6];
  x[0xf] = y[0x7];
  x[0x8] = x[0x4];
  x[0x9] = x[0x5];
  x[0xa] = x[0x6];
  x[0xb] = x[0x7];
  x[0x4] = y[0x0];
  x[0x5] = y[0x1];
  x[0x6] = y[0x2];
  x[0x7] = y[0x3];
  x[0x0] = x[0x0];
  x[0x1] = x[0x1];
  x[0x2] = x[0x2];
  x[0x3] = x[0x3];
}

static void SsePunpckhwd(uint8_t x[16], const uint8_t y[16]) {
  x[0x0] = x[0x8];
  x[0x1] = x[0x9];
  x[0x2] = y[0x8];
  x[0x3] = y[0x9];
  x[0x4] = x[0xa];
  x[0x5] = x[0xb];
  x[0x6] = y[0xa];
  x[0x7] = y[0xb];
  x[0x8] = x[0xc];
  x[0x9] = x[0xd];
  x[0xa] = y[0xc];
  x[0xb] = y[0xd];
  x[0xc] = x[0xe];
  x[0xd] = x[0xf];
  x[0xe] = y[0xe];
  x[0xf] = y[0xf];
}

static void SsePunpckhdq(uint8_t x[16], const uint8_t y[16]) {
  x[0x0] = x[0x8];
  x[0x1] = x[0x9];
  x[0x2] = x[0xa];
  x[0x3] = x[0xb];
  x[0x4] = y[0x8];
  x[0x5] = y[0x9];
  x[0x6] = y[0xa];
  x[0x7] = y[0xb];
  x[0x8] = x[0xc];
  x[0x9] = x[0xd];
  x[0xa] = x[0xe];
  x[0xb] = x[0xf];
  x[0xc] = y[0xc];
  x[0xd] = y[0xd];
  x[0xe] = y[0xe];
  x[0xf] = y[0xf];
}

static void SsePunpcklqdq(uint8_t x[16], const uint8_t y[16]) {
  x[0x8] = y[0x0];
  x[0x9] = y[0x1];
  x[0xa] = y[0x2];
  x[0xb] = y[0x3];
  x[0xc] = y[0x4];
  x[0xd] = y[0x5];
  x[0xe] = y[0x6];
  x[0xf] = y[0x7];
  x[0x0] = x[0x0];
  x[0x1] = x[0x1];
  x[0x2] = x[0x2];
  x[0x3] = x[0x3];
  x[0x4] = x[0x4];
  x[0x5] = x[0x5];
  x[0x6] = x[0x6];
  x[0x7] = x[0x7];
}

static void SsePunpckhqdq(uint8_t x[16], const uint8_t y[16]) {
  x[0x0] = x[0x8];
  x[0x1] = x[0x9];
  x[0x2] = x[0xa];
  x[0x3] = x[0xb];
  x[0x4] = x[0xc];
  x[0x5] = x[0xd];
  x[0x6] = x[0xe];
  x[0x7] = x[0xf];
  x[0x8] = y[0x8];
  x[0x9] = y[0x9];
  x[0xa] = y[0xa];
  x[0xb] = y[0xb];
  x[0xc] = y[0xc];
  x[0xd] = y[0xd];
  x[0xe] = y[0xe];
  x[0xf] = y[0xf];
}

static void SsePsrlw(uint8_t x[16], unsigned k) {
  MmxPsrlw(x + 0, k);
  MmxPsrlw(x + 8, k);
}

static void SsePsraw(uint8_t x[16], unsigned k) {
  MmxPsraw(x + 0, k);
  MmxPsraw(x + 8, k);
}

static void SsePsllw(uint8_t x[16], unsigned k) {
  MmxPsllw(x + 0, k);
  MmxPsllw(x + 8, k);
}

static void SsePsrld(uint8_t x[16], unsigned k) {
  MmxPsrld(x + 0, k);
  MmxPsrld(x + 8, k);
}

static void SsePsrad(uint8_t x[16], unsigned k) {
  MmxPsrad(x + 0, k);
  MmxPsrad(x + 8, k);
}

static void SsePslld(uint8_t x[16], unsigned k) {
  MmxPslld(x + 0, k);
  MmxPslld(x + 8, k);
}

static void SsePsrlq(uint8_t x[16], unsigned k) {
  MmxPsrlq(x + 0, k);
  MmxPsrlq(x + 8, k);
}

static void SsePsllq(uint8_t x[16], unsigned k) {
  MmxPsllq(x + 0, k);
  MmxPsllq(x + 8, k);
}

static void SsePsubsb(uint8_t x[16], const uint8_t y[16]) {
  MmxPsubsb(x + 0, y + 0);
  MmxPsubsb(x + 8, y + 8);
}

static void SsePaddsb(uint8_t x[16], const uint8_t y[16]) {
  MmxPaddsb(x + 0, y + 0);
  MmxPaddsb(x + 8, y + 8);
}

static void SsePsubsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPsubsw(x + 0, y + 0);
  MmxPsubsw(x + 8, y + 8);
}

static void SsePaddsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPaddsw(x + 0, y + 0);
  MmxPaddsw(x + 8, y + 8);
}

static void SsePaddusb(uint8_t x[16], const uint8_t y[16]) {
  MmxPaddusb(x + 0, y + 0);
  MmxPaddusb(x + 8, y + 8);
}

static void SsePsubusb(uint8_t x[16], const uint8_t y[16]) {
  MmxPsubusb(x + 0, y + 0);
  MmxPsubusb(x + 8, y + 8);
}

static void SsePsubusw(uint8_t x[16], const uint8_t y[16]) {
  MmxPsubusw(x + 0, y + 0);
  MmxPsubusw(x + 8, y + 8);
}

static void SsePminsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPminsw(x + 0, y + 0);
  MmxPminsw(x + 8, y + 8);
}

static void SsePmaxsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPmaxsw(x + 0, y + 0);
  MmxPmaxsw(x + 8, y + 8);
}

static void SsePsignb(uint8_t x[16], const uint8_t y[16]) {
  MmxPsignb(x + 0, y + 0);
  MmxPsignb(x + 8, y + 8);
}

static void SsePsignw(uint8_t x[16], const uint8_t y[16]) {
  MmxPsignw(x + 0, y + 0);
  MmxPsignw(x + 8, y + 8);
}

static void SsePsignd(uint8_t x[16], const uint8_t y[16]) {
  MmxPsignd(x + 0, y + 0);
  MmxPsignd(x + 8, y + 8);
}

static void SsePmulhrsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPmulhrsw(x + 0, y + 0);
  MmxPmulhrsw(x + 8, y + 8);
}

static void SsePabsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPabsw(x + 0, y + 0);
  MmxPabsw(x + 8, y + 8);
}

static void SsePabsd(uint8_t x[16], const uint8_t y[16]) {
  MmxPabsd(x + 0, y + 0);
  MmxPabsd(x + 8, y + 8);
}

static void SsePcmpgtw(uint8_t x[16], const uint8_t y[16]) {
  MmxPcmpgtw(x + 0, y + 0);
  MmxPcmpgtw(x + 8, y + 8);
}

static void SsePcmpeqw(uint8_t x[16], const uint8_t y[16]) {
  MmxPcmpeqw(x + 0, y + 0);
  MmxPcmpeqw(x + 8, y + 8);
}

static void SsePcmpgtd(uint8_t x[16], const uint8_t y[16]) {
  MmxPcmpgtd(x + 0, y + 0);
  MmxPcmpgtd(x + 8, y + 8);
}

static void SsePcmpeqd(uint8_t x[16], const uint8_t y[16]) {
  MmxPcmpeqd(x + 0, y + 0);
  MmxPcmpeqd(x + 8, y + 8);
}

static void SsePsrawv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsrawv(x + 0, y);
  MmxPsrawv(x + 8, y);
}

static void SsePsradv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsradv(x + 0, y);
  MmxPsradv(x + 8, y);
}

static void SsePsrlwv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsrlwv(x + 0, y);
  MmxPsrlwv(x + 8, y);
}

static void SsePsllwv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsllwv(x + 0, y);
  MmxPsllwv(x + 8, y);
}

static void SsePsrldv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsrldv(x + 0, y);
  MmxPsrldv(x + 8, y);
}

static void SsePslldv(uint8_t x[16], const uint8_t y[16]) {
  MmxPslldv(x + 0, y);
  MmxPslldv(x + 8, y);
}

static void SsePsrlqv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsrlqv(x + 0, y);
  MmxPsrlqv(x + 8, y);
}

static void SsePsllqv(uint8_t x[16], const uint8_t y[16]) {
  MmxPsllqv(x + 0, y);
  MmxPsllqv(x + 8, y);
}

static void SsePavgw(uint8_t x[16], const uint8_t y[16]) {
  MmxPavgw(x + 0, y + 0);
  MmxPavgw(x + 8, y + 8);
}

static void SsePmaddwd(uint8_t x[16], const uint8_t y[16]) {
  MmxPmaddwd(x + 0, y + 0);
  MmxPmaddwd(x + 8, y + 8);
}

static void SsePmulhuw(uint8_t x[16], const uint8_t y[16]) {
  MmxPmulhuw(x + 0, y + 0);
  MmxPmulhuw(x + 8, y + 8);
}

static void SsePmulhw(uint8_t x[16], const uint8_t y[16]) {
  MmxPmulhw(x + 0, y + 0);
  MmxPmulhw(x + 8, y + 8);
}

static void SsePmullw(uint8_t x[16], const uint8_t y[16]) {
  MmxPmullw(x + 0, y + 0);
  MmxPmullw(x + 8, y + 8);
}

static void SsePmulld(uint8_t x[16], const uint8_t y[16]) {
  MmxPmulld(x + 0, y + 0);
  MmxPmulld(x + 8, y + 8);
}

static void SsePmaddubsw(uint8_t x[16], const uint8_t y[16]) {
  MmxPmaddubsw(x + 0, y + 0);
  MmxPmaddubsw(x + 8, y + 8);
}

static void OpPsb(struct Machine *m, uint32_t rde,
                  void MmxKernel(uint8_t[8], unsigned),
                  void SseKernel(uint8_t[16], unsigned)) {
  if (Osz(rde)) {
    SseKernel(XmmRexbRm(m, rde), m->xedd->op.uimm0);
  } else {
    MmxKernel(XmmRexbRm(m, rde), m->xedd->op.uimm0);
  }
}

void Op171(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 2:
      OpPsb(m, rde, MmxPsrlw, SsePsrlw);
      break;
    case 4:
      OpPsb(m, rde, MmxPsraw, SsePsraw);
      break;
    case 6:
      OpPsb(m, rde, MmxPsllw, SsePsllw);
      break;
    default:
      OpUd(m, rde);
  }
}

void Op172(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 2:
      OpPsb(m, rde, MmxPsrld, SsePsrld);
      break;
    case 4:
      OpPsb(m, rde, MmxPsrad, SsePsrad);
      break;
    case 6:
      OpPsb(m, rde, MmxPslld, SsePslld);
      break;
    default:
      OpUd(m, rde);
  }
}

void Op173(struct Machine *m, uint32_t rde) {
  switch (ModrmReg(rde)) {
    case 2:
      OpPsb(m, rde, MmxPsrlq, SsePsrlq);
      break;
    case 3:
      OpPsb(m, rde, MmxPsrldq, SsePsrldq);
      break;
    case 6:
      OpPsb(m, rde, MmxPsllq, SsePsllq);
      break;
    case 7:
      OpPsb(m, rde, MmxPslldq, SsePslldq);
      break;
    default:
      OpUd(m, rde);
  }
}

void OpSsePalignr(struct Machine *m, uint32_t rde) {
  if (Osz(rde)) {
    SsePalignr(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead16(m, rde),
               m->xedd->op.uimm0);
  } else {
    MmxPalignr(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead8(m, rde),
               m->xedd->op.uimm0);
  }
}

static void OpSse(struct Machine *m, uint32_t rde,
                  void MmxKernel(uint8_t[8], const uint8_t[8]),
                  void SseKernel(uint8_t[16], const uint8_t[16])) {
  if (Osz(rde)) {
    SseKernel(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead16(m, rde));
  } else {
    MmxKernel(XmmRexrReg(m, rde), GetModrmRegisterXmmPointerRead8(m, rde));
  }
}

/* clang-format off */
void OpSsePunpcklbw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpcklbw, SsePunpcklbw); }
void OpSsePunpcklwd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpcklwd, SsePunpcklwd); }
void OpSsePunpckldq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpckldq, SsePunpckldq); }
void OpSsePacksswb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPacksswb, SsePacksswb); }
void OpSsePcmpgtb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPcmpgtb, SsePcmpgtb); }
void OpSsePcmpgtw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPcmpgtw, SsePcmpgtw); }
void OpSsePcmpgtd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPcmpgtd, SsePcmpgtd); }
void OpSsePackuswb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPackuswb, SsePackuswb); }
void OpSsePunpckhbw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpckhbw, SsePunpckhbw); }
void OpSsePunpckhwd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpckhwd, SsePunpckhwd); }
void OpSsePunpckhdq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpckhdq, SsePunpckhdq); }
void OpSsePackssdw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPackssdw, SsePackssdw); }
void OpSsePunpcklqdq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpcklqdq, SsePunpcklqdq); }
void OpSsePunpckhqdq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPunpckhqdq, SsePunpckhqdq); }
void OpSsePcmpeqb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPcmpeqb, SsePcmpeqb); }
void OpSsePcmpeqw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPcmpeqw, SsePcmpeqw); }
void OpSsePcmpeqd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPcmpeqd, SsePcmpeqd); }
void OpSsePsrlwv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsrlwv, SsePsrlwv); }
void OpSsePsrldv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsrldv, SsePsrldv); }
void OpSsePsrlqv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsrlqv, SsePsrlqv); }
void OpSsePaddq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddq, SsePaddq); }
void OpSsePmullw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmullw, SsePmullw); }
void OpSsePsubusb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubusb, SsePsubusb); }
void OpSsePsubusw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubusw, SsePsubusw); }
void OpSsePminub(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPminub, SsePminub); }
void OpSsePand(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPand, SsePand); }
void OpSsePaddusb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddusb, SsePaddusb); }
void OpSsePaddusw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddusw, SsePaddusw); }
void OpSsePmaxub(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmaxub, SsePmaxub); }
void OpSsePandn(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPandn, SsePandn); }
void OpSsePavgb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPavgb, SsePavgb); }
void OpSsePsrawv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsrawv, SsePsrawv); }
void OpSsePsradv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsradv, SsePsradv); }
void OpSsePavgw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPavgw, SsePavgw); }
void OpSsePmulhuw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmulhuw, SsePmulhuw); }
void OpSsePmulhw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmulhw, SsePmulhw); }
void OpSsePsubsb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubsb, SsePsubsb); }
void OpSsePsubsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubsw, SsePsubsw); }
void OpSsePminsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPminsw, SsePminsw); }
void OpSsePor(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPor, SsePor); }
void OpSsePaddsb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddsb, SsePaddsb); }
void OpSsePaddsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddsw, SsePaddsw); }
void OpSsePmaxsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmaxsw, SsePmaxsw); }
void OpSsePxor(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPxor, SsePxor); }
void OpSsePsllwv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsllwv, SsePsllwv); }
void OpSsePslldv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPslldv, SsePslldv); }
void OpSsePsllqv(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsllqv, SsePsllqv); }
void OpSsePmuludq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmuludq, SsePmuludq); }
void OpSsePmaddwd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmaddwd, SsePmaddwd); }
void OpSsePsadbw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsadbw, SsePsadbw); }
void OpSsePsubb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubb, SsePsubb); }
void OpSsePsubw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubw, SsePsubw); }
void OpSsePsubd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubd, SsePsubd); }
void OpSsePsubq(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsubq, SsePsubq); }
void OpSsePaddb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddb, SsePaddb); }
void OpSsePaddw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddw, SsePaddw); }
void OpSsePaddd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPaddd, SsePaddd); }
void OpSsePshufb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPshufb, SsePshufb); }
void OpSsePhaddw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPhaddw, SsePhaddw); }
void OpSsePhaddd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPhaddd, SsePhaddd); }
void OpSsePhaddsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPhaddsw, SsePhaddsw); }
void OpSsePmaddubsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmaddubsw, SsePmaddubsw); }
void OpSsePhsubw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPhsubw, SsePhsubw); }
void OpSsePhsubd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPhsubd, SsePhsubd); }
void OpSsePhsubsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPhsubsw, SsePhsubsw); }
void OpSsePsignb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsignb, SsePsignb); }
void OpSsePsignw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsignw, SsePsignw); }
void OpSsePsignd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPsignd, SsePsignd); }
void OpSsePmulhrsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmulhrsw, SsePmulhrsw); }
void OpSsePabsb(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPabsb, SsePabsb); }
void OpSsePabsw(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPabsw, SsePabsw); }
void OpSsePabsd(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPabsd, SsePabsd); }
void OpSsePmulld(struct Machine *m, uint32_t rde) { OpSse(m, rde, MmxPmulld, SsePmulld); }
/* clang-format on */
