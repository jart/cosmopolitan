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
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/stdio/rand.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/ycbcr.h"

#define C1331(A, B, C, D)         \
  ({                              \
    unsigned short Ax, Bx;        \
    unsigned char Al, Bl, Cl, Dl; \
    Al = (A);                     \
    Bl = (B);                     \
    Cl = (C);                     \
    Dl = (D);                     \
    Bx = Bl;                      \
    Bx += Cl;                     \
    Bx *= 3;                      \
    Ax = Al;                      \
    Ax += Dl;                     \
    Ax += Bx;                     \
    Ax += 4;                      \
    Ax >>= 3;                     \
    Al = Ax;                      \
    Al;                           \
  })

TEST(C1331, test) {
  EXPECT_EQ(0, C1331(0, 0, 0, 0));
  EXPECT_EQ(255, C1331(255, 255, 255, 255));
  EXPECT_EQ(79, C1331(33, 100, 77, 69));
  EXPECT_EQ(80, C1331(39, 100, 77, 69));
}

#define C161(A, B, C)          \
  ({                           \
    short Dx;                  \
    unsigned char Dl;          \
    unsigned char Al, Bl, Cl;  \
    unsigned short Ax, Bx, Cx; \
    Al = (A);                  \
    Bl = (B);                  \
    Cl = (C);                  \
                               \
    Bx = Bl;                   \
    Bx += 3;                   \
    Bx >>= 1;                  \
    Bx += Bl;                  \
                               \
    Ax = Al;                   \
    Ax += 4;                   \
    Ax >>= 2;                  \
                               \
    Cx = Cl;                   \
    Cx += 4;                   \
    Cx >>= 2;                  \
                               \
    Dx = Bx;                   \
    Dx -= Ax;                  \
    Dx -= Cx;                  \
                               \
    Dx = MAX(0, Dx);           \
    Dx = MIN(255, Dx);         \
    Dl = Dx;                   \
    Dl;                        \
  })

TEST(C161, test) {
  EXPECT_EQ(0, C161(0, 0, 0));
  EXPECT_EQ(255, C161(255, 255, 255));
  EXPECT_EQ(124, C161(33, 100, 69)); /* 124.50 = 3/2.*100 -1/4.*33 -1/4.*69 */
  EXPECT_EQ(124, C161(33, 100, 70)); /* 124.25 = 3/2.*100 -1/4.*33 -1/4.*70 */
  EXPECT_EQ(126, C161(33, 101, 69)); /* 126.00 = 3/2.*101 -1/4.*33 -1/4.*69 */
}

#define C121(A, B, C)          \
  ({                           \
    short Dx;                  \
    unsigned char Dl;          \
    unsigned char Al, Bl, Cl;  \
    unsigned short Ax, Bx, Cx; \
    Al = (A);                  \
    Bl = (B);                  \
    Cl = (C);                  \
                               \
    Bx = Bl;                   \
    Bx += 3;                   \
    Bx >>= 1;                  \
    Bx += Bl;                  \
                               \
    Ax = Al;                   \
    Ax += 4;                   \
    Ax >>= 2;                  \
                               \
    Cx = Cl;                   \
    Cx += 4;                   \
    Cx >>= 2;                  \
                               \
    Dx = Bx;                   \
    Dx -= Ax;                  \
    Dx -= Cx;                  \
                               \
    Dx = MAX(0, Dx);           \
    Dx = MIN(255, Dx);         \
    Dl = Dx;                   \
    Dl;                        \
  })

TEST(C121, test) {
  EXPECT_EQ(0, C161(0, 0, 0));
  EXPECT_EQ(255, C161(255, 255, 255));
  EXPECT_EQ(124, C161(33, 100, 69)); /* 124.50 = 3/2.*100 -1/4.*33 -1/4.*69 */
  EXPECT_EQ(124, C161(33, 100, 70)); /* 124.25 = 3/2.*100 -1/4.*33 -1/4.*70 */
  EXPECT_EQ(126, C161(33, 101, 69)); /* 126.00 = 3/2.*101 -1/4.*33 -1/4.*69 */
}

#define BLERP(A, B, P)            \
  ({                              \
    unsigned char Al, Bl, Cl, Dl; \
    unsigned short Bx;            \
    Al = (A);                     \
    Bl = (B);                     \
    Cl = MAX(Al, Bl);             \
    Al = MIN(Al, Bl);             \
    Dl = Cl - Al;                 \
    Bl = (P);                     \
    Bx = Bl;                      \
    Bx *= Dl;                     \
    Bx += 255;                    \
    Bx >>= 8;                     \
    Bx += Al;                     \
    Al = Bx;                      \
    Al;                           \
  })

TEST(BLERP, test) {
  EXPECT_EQ(0, BLERP(0, 0, 128));
  EXPECT_EQ(255, BLERP(255, 255, 128));
  EXPECT_EQ(64, BLERP(0, 128, 128));
  EXPECT_EQ(64, BLERP(128, 0, 128));
  EXPECT_EQ(32, BLERP(0, 128, 64));
  EXPECT_EQ(32, BLERP(128, 0, 64));
  EXPECT_EQ(8, BLERP(0, 128, 16));
  EXPECT_EQ(8, BLERP(128, 0, 16));
  EXPECT_EQ(0, BLERP(0, 128, 0));
  EXPECT_EQ(0, BLERP(128, 0, 0));
  EXPECT_EQ(128, BLERP(0, 128, 255));
  EXPECT_EQ(128, BLERP(128, 0, 255));
}

#define MIX(A, B) \
  ({              \
    short Ax, Bx; \
    Ax = (A);     \
    Bx = (B);     \
    Ax += Bx;     \
    Ax += 1;      \
    Ax /= 2;      \
    Ax;           \
  })

TEST(MIX, test) {
  EXPECT_EQ(0, MIX(0, 0));
  EXPECT_EQ(255, MIX(255, 255));
  EXPECT_EQ(64, MIX(0, 128));
  EXPECT_EQ(64, MIX(128, 0));
  EXPECT_EQ(127, MIX(0, 254));
}

void ExpandLuminosityRange(unsigned n, unsigned char *Y) {
  unsigned i, j;
  unsigned char b[16];
  unsigned short s[16];
  CHECK_ALIGNED(16, Y);
  for (i = 0; i < n; i += 16) {
    memcpy(b, Y + i, 16);
    for (j = 0; j < 16; ++j) b[j] = MAX(0, b[j] - 16);
    for (j = 0; j < 16; ++j) s[j] = b[j];
    for (j = 0; j < 16; ++j) s[j] *= 150;
    for (j = 0; j < 16; ++j) s[j] /= 128;
    for (j = 0; j < 16; ++j) s[j] = MIN(255, s[j]);
    for (j = 0; j < 16; ++j) b[j] = s[j];
    memcpy(Y + i, b, 16);
  }
}

TEST(ExpandLuminosityRange, test) {
  unsigned char Y[32];
  Y[0] = 0;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(0, Y[0]);
  Y[0] = 16;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(0, Y[0]);
  Y[0] = 17;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(1, Y[0]);
  Y[0] = 128;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(131, Y[0]);
  Y[0] = 233;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(254, Y[0]);
  Y[0] = 235;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(255, Y[0]);
  Y[0] = 255;
  ExpandLuminosityRange(16, Y);
  EXPECT_EQ(255, Y[0]);
}

#if 1
void NtscItu601YCbCrToStandardRgb(unsigned n, unsigned char *restrict Y,
                                  unsigned char *restrict Cb,
                                  unsigned char *restrict Cr) {
  unsigned i;
  short r, g, b;
  unsigned char y;
  for (i = 0; i < n; ++i) {
    y = MIN(255, (MIN(235, MAX(16, Y[i])) - 16) * 150 / 128);
    r = y + ((Cr[i] + ((Cr[i] * 103) / 256)) - 179);
    g = y - (((Cb[i] * 88) / 256) - 44 + ((Cr[i] * 183) / 256) - 91);
    b = y + ((Cb[i] + ((Cb[i] * 198) / 256)) - 227);
    Y[i] = MIN(255, MAX(0, r));
    Cb[i] = MIN(255, MAX(0, g));
    Cr[i] = MIN(255, MAX(0, b));
  }
}
#else
void NtscItu601YCbCrToStandardRgb(size_t n, unsigned char *restrict Y,
                                  unsigned char *restrict Cb,
                                  unsigned char *restrict Cr) {
  unsigned i;
  short y, gb, gr, r, g, b;
  unsigned char gs, cb, cr;
  unsigned short bw, ky, kr, kb, kgb, kgr;
  for (i = 0; i < n; ++i) {
    y = Y[i];
    cb = Cb[i];
    cr = Cr[i];
    y -= 16; /* luminance (expand tv range [16,235] for pc [0,255]) */
    y = MAX(0, y);
    y = MIN(234 - 16, y);
    ky = y;
    ky *= 150;
    ky /= 128;
    gs = ky;
    kr = cr; /* red */
    kr *= 103;
    kr /= 256;
    kr += cr;
    r = kr;
    r -= 179;
    r += gs;
    kb = cb; /* blue */
    kb *= 198;
    kb /= 256;
    kb += cb;
    b = kb;
    b -= 227;
    b += gs;
    kgb = cb; /* green */
    kgb *= 88;
    kgb /= 256;
    gb = kgb;
    gb -= 44;
    kgr = cr;
    kgr *= 183;
    kgr /= 256;
    gr = kgr;
    gr -= 91;
    g = gs;
    g -= gr;
    g -= gb;
    r = MAX(0, r); /* clamp */
    g = MAX(0, g);
    b = MAX(0, b);
    r = MIN(255, r);
    g = MIN(255, g);
    b = MIN(255, b);
    Y[i] = r;
    Cb[i] = g;
    Cr[i] = b;
  }
}
#endif

TEST(NtscItu601YCbCrToStandardRgb, testWhite) {
  unsigned char a, b, c;
  a = 234;
  b = 128;
  c = 128;
  NtscItu601YCbCrToStandardRgb(1, &a, &b, &c);
  EXPECT_EQ(255, a);
  EXPECT_EQ(255, b);
  EXPECT_EQ(255, c);
  a = 235;
  b = 128;
  c = 128;
  NtscItu601YCbCrToStandardRgb(1, &a, &b, &c);
  EXPECT_EQ(255, a);
  EXPECT_EQ(255, b);
  EXPECT_EQ(255, c);
  a = 255;
  b = 128;
  c = 128;
  NtscItu601YCbCrToStandardRgb(1, &a, &b, &c);
  EXPECT_EQ(255, a);
  EXPECT_EQ(255, b);
  EXPECT_EQ(255, c);
}
