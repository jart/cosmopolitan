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
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "tool/viz/lib/graphic.h"

static int perm[513];

static double lerp(double t, double a, double b) {
  return a + t * (b - a);
}

static double fade(double t) {
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

static double grad(int hash, double x, double y, double z) {
  int h;
  double u, v;
  h = hash & 15;
  u = h < 8 ? x : y;
  v = h < 4 ? y : (h == 12 || h == 14) ? x : z;
  return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

/**
 * Perlin's improved noise algorithm.
 *
 * @see http://mrl.nyu.edu/~perlin/noise/
 * @note it's bryce3d cgi basically
 */
double perlin3(double x, double y, double z) /* clang-format off */ {
  double u, v, w;
  int X, Y, Z, AA, AB, BA, BB;
  X = floor(x);
  Y = floor(y);
  Z = floor(z);
  X &= 0xff;
  Y &= 0xff;
  Z &= 0xff;
  x -= floor(x);
  y -= floor(y);
  z -= floor(z);
  u = fade(x);
  v = fade(y);
  w = fade(z);
  AA = perm[perm[X    ] + Y    ] + Z;
  AB = perm[perm[X    ] + Y + 1] + Z;
  BA = perm[perm[X + 1] + Y    ] + Z;
  BB = perm[perm[X + 1] + Y + 1] + Z;
  return lerp(w, lerp(v, lerp(u, grad(perm[AA    ], x    , y    , z    ),
                                 grad(perm[BA    ], x - 1, y    , z    )),
                         lerp(u, grad(perm[AB    ], x    , y - 1, z    ),
                                 grad(perm[BB    ], x - 1, y - 1, z    ))),
                 lerp(v, lerp(u, grad(perm[AA + 1], x    , y    , z - 1),
                                 grad(perm[BA + 1], x - 1, y    , z - 1)),
                         lerp(u, grad(perm[AB + 1], x    , y - 1, z - 1),
                                 grad(perm[BB + 1], x - 1, y - 1, z - 1))));
} /* clang-format on */

static const uint8_t kPerm[] = {
    151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
    225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
    6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
    35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
    171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
    231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
    245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
    76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
    202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
    58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
    154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
    19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
    228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
    145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
    84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
    222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
    180,
};

__attribute__((__constructor__)) static void init(void) {
  unsigned i;
  for (i = 0; i < 256; ++i) {
    perm[i] = kPerm[i];
    perm[i + 256] = kPerm[i];
  }
}
