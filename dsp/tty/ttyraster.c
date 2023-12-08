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
#include "dsp/core/twixt8.h"
#include "dsp/tty/quant.h"
#include "dsp/tty/tty.h"
#include "dsp/tty/ttyrgb.h"
#include "dsp/tty/windex.h"
#include "libc/assert.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

#define SQR(X)        ((X) * (X))
#define DIST(X, Y)    ((X) - (Y))
#define QSUB(A, B, C) ABS(DIST(A##C, B##C))
#define DIFF(A, B)    (QSUB(A, B, r) + QSUB(A, B, g) + QSUB(A, B, b))
#define JUDGE(M)      (DIFF(ttl, M) + DIFF(ttr, M) + DIFF(tbl, M) + DIFF(tbr, M))

static const struct Glyph {
  char c1, c2, c3, len;
} kGlyphs[2][11] = {
    {{0x20, 0x00, 0x00, 1},  /*   */
     {0xE2, 0x96, 0x84, 3},  /* ▄ */
     {0xE2, 0x96, 0x8C, 3},  /* ▌ */
     {0xE2, 0x96, 0x9D, 3},  /* ▝ */
     {0xE2, 0x96, 0x97, 3},  /* ▗ */
     {0xE2, 0x96, 0x96, 3},  /* ▖ */
     {0xE2, 0x96, 0x9E, 3},  /* ▞ */
     {0xE2, 0x96, 0x98, 3},  /* ▘ */
     {0xE2, 0x96, 0x91, 3},  /* ░ */
     {0xE2, 0x96, 0x93, 3},  /* ▓ */
     {0xE2, 0x96, 0x92, 3}}, /* ▒ */
    {{0xE2, 0x96, 0x88, 3},  /* █ */
     {0xE2, 0x96, 0x80, 3},  /* ▀ */
     {0xE2, 0x96, 0x90, 3},  /* ▐ */
     {0xE2, 0x96, 0x99, 3},  /* ▙ */
     {0xE2, 0x96, 0x9B, 3},  /* ▛ */
     {0xE2, 0x96, 0x9C, 3},  /* ▜ */
     {0xE2, 0x96, 0x9A, 3},  /* ▚ */
     {0xE2, 0x96, 0x9F, 3},  /* ▟ */
     {0xE2, 0x96, 0x93, 3},  /* ▓ */
     {0xE2, 0x96, 0x91, 3},  /* ░ */
     {0xE2, 0x96, 0x92, 3}}, /* ▒ */
};

static const struct Pick {
  unsigned char fg, bg, k;
} kPicksUnicode[96] = {
    {-1, BL, 0},  /*   */
    {BR, BL, 4},  /* ▗ */
    {TL, BL, 4},  /* ▗ */
    {TR, BL, 4},  /* ▗ */
    {BR, BL, 5},  /* ▖ */
    {BR, BL, 1},  /* ▄ */
    {TL, BL, 5},  /* ▖ */
    {TL, BL, 1},  /* ▄ */
    {TR, BL, 5},  /* ▖ */
    {TR, BL, 1},  /* ▄ */
    {BR, BL, 3},  /* ▝ */
    {BL, BR, 2},  /* ▌ */
    {BR, BL, 6},  /* ▞ */
    {BL, BR, 7},  /* ▘ */
    {TL, BL, 3},  /* ▝ */
    {BL, TL, 2},  /* ▌ */
    {TL, BL, 6},  /* ▞ */
    {BL, TL, 7},  /* ▘ */
    {TR, BL, 3},  /* ▝ */
    {BL, TR, 2},  /* ▌ */
    {TR, BL, 6},  /* ▞ */
    {BL, TR, 7},  /* ▘ */
    {BR, BL, 7},  /* ▘ */
    {BL, BR, 6},  /* ▞ */
    {BR, BL, 2},  /* ▌ */
    {BL, BR, 3},  /* ▝ */
    {BL, BR, 1},  /* ▄ */
    {BL, BR, 5},  /* ▖ */
    {BL, BR, 4},  /* ▗ */
    {-1, BR, 0},  /*   */
    {TL, BR, 4},  /* ▗ */
    {TR, BR, 4},  /* ▗ */
    {TL, BR, 5},  /* ▖ */
    {TL, BR, 1},  /* ▄ */
    {TR, BR, 5},  /* ▖ */
    {TR, BR, 1},  /* ▄ */
    {TL, BR, 3},  /* ▝ */
    {BR, TL, 2},  /* ▌ */
    {TL, BR, 6},  /* ▞ */
    {BR, TL, 7},  /* ▘ */
    {TR, BR, 3},  /* ▝ */
    {BR, TR, 2},  /* ▌ */
    {TR, BR, 6},  /* ▞ */
    {BR, TR, 7},  /* ▘ */
    {TL, BL, 7},  /* ▘ */
    {BL, TL, 6},  /* ▞ */
    {TL, BL, 2},  /* ▌ */
    {BL, TL, 3},  /* ▝ */
    {TL, BR, 7},  /* ▘ */
    {BR, TL, 6},  /* ▞ */
    {TL, BR, 2},  /* ▌ */
    {BR, TL, 3},  /* ▝ */
    {BL, TL, 1},  /* ▄ */
    {BL, TL, 5},  /* ▖ */
    {BR, TL, 1},  /* ▄ */
    {BR, TL, 5},  /* ▖ */
    {BL, TL, 4},  /* ▗ */
    {BR, TL, 4},  /* ▗ */
    {-1, TL, 0},  /*   */
    {TR, TL, 4},  /* ▗ */
    {TR, TL, 5},  /* ▖ */
    {TR, TL, 1},  /* ▄ */
    {TR, TL, 3},  /* ▝ */
    {TL, TR, 2},  /* ▌ */
    {TR, TL, 6},  /* ▞ */
    {TL, TR, 7},  /* ▘ */
    {TR, BL, 7},  /* ▘ */
    {BL, TR, 6},  /* ▞ */
    {TR, BL, 2},  /* ▌ */
    {BL, TR, 3},  /* ▝ */
    {TR, BR, 7},  /* ▘ */
    {BR, TR, 6},  /* ▞ */
    {TR, BR, 2},  /* ▌ */
    {BR, TR, 3},  /* ▝ */
    {TR, TL, 7},  /* ▘ */
    {TL, TR, 6},  /* ▞ */
    {TR, TL, 2},  /* ▌ */
    {TL, TR, 3},  /* ▝ */
    {BL, TR, 1},  /* ▄ */
    {BL, TR, 5},  /* ▖ */
    {BR, TR, 1},  /* ▄ */
    {BR, TR, 5},  /* ▖ */
    {TL, TR, 1},  /* ▄ */
    {TL, TR, 5},  /* ▖ */
    {BL, TR, 4},  /* ▗ */
    {BR, TR, 4},  /* ▗ */
    {TL, TR, 4},  /* ▗ */
    {-1, TR, 0},  /*   */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
};

static const struct Pick kPicksCp437[32] = {
    {-1, BL, 0},  /*   */
    {BR, BL, 1},  /* ▄ */
    {TL, BL, 1},  /* ▄ */
    {TR, BL, 1},  /* ▄ */
    {BL, BR, 1},  /* ▄ */
    {BL, TL, 1},  /* ▄ */
    {BL, TR, 1},  /* ▄ */
    {BL, BR, 2},  /* ▌ */
    {BL, TL, 2},  /* ▌ */
    {BL, TR, 2},  /* ▌ */
    {BR, BL, 2},  /* ▌ */
    {-1, BR, 0},  /*   */
    {TL, BR, 1},  /* ▄ */
    {TR, BR, 1},  /* ▄ */
    {BR, TL, 2},  /* ▌ */
    {BR, TR, 2},  /* ▌ */
    {TL, BL, 2},  /* ▌ */
    {TL, BR, 2},  /* ▌ */
    {BR, TL, 1},  /* ▄ */
    {-1, TL, 0},  /*   */
    {TR, TL, 1},  /* ▄ */
    {TL, TR, 2},  /* ▌ */
    {TR, BL, 2},  /* ▌ */
    {TR, BR, 2},  /* ▌ */
    {TR, TL, 2},  /* ▌ */
    {BR, TR, 1},  /* ▄ */
    {TL, TR, 1},  /* ▄ */
    {-1, TR, 0},  /*   */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
    {-1, -1, -1}, /* X */
};

static const struct Pick kPicksMixBlock[32] = {
    {BR, BL, 8}, /* ░ */
    {BR, BL, 9}, /* ▓ */
    {TL, BL, 8}, /* ░ */
    {TL, BL, 9}, /* ▓ */
    {TR, BL, 8}, /* ░ */
    {TR, BL, 9}, /* ▓ */
    {TL, BR, 8}, /* ░ */
    {TL, BR, 9}, /* ▓ */
    {TR, BR, 8}, /* ░ */
    {TR, BR, 9}, /* ▓ */
    {BL, TL, 8}, /* ░ */
    {BL, TL, 9}, /* ▓ */
    {BL, TL, 8}, /* ░ */
    {BL, TL, 9}, /* ▓ */
    {TR, TL, 8}, /* ░ */
    {TR, TL, 9}, /* ▓ */
    {BL, TR, 8}, /* ░ */
    {BL, TR, 9}, /* ▓ */
    {BR, TR, 8}, /* ░ */
    {BR, TR, 9}, /* ▓ */
    {TL, TR, 8}, /* ░ */
    {TL, TR, 9}, /* ▓ */
    /**/
    {BL, TL, 8}, /* ░ */
    {BL, TL, 9}, /* ▓ */
    {TR, TL, 8}, /* ░ */
    {TR, TL, 9}, /* ▓ */
    {BL, TR, 8}, /* ░ */
    {BL, TR, 9}, /* ▓ */
    {BR, TR, 8}, /* ░ */
    {BR, TR, 9}, /* ▓ */
    {TL, TR, 8}, /* ░ */
    {TL, TR, 9}, /* ▓ */
};

static unsigned short GetBlockDist(struct TtyRgb a, struct TtyRgb b,
                                   struct TtyRgb c, struct TtyRgb d,
                                   struct TtyRgb w, struct TtyRgb x,
                                   struct TtyRgb y, struct TtyRgb z) {
  unsigned short dist;
  dist = 0;
  dist += ABS(a.r - w.r);
  dist += ABS(a.g - w.g);
  dist += ABS(a.b - w.b);
  dist += ABS(b.r - x.r);
  dist += ABS(b.g - x.g);
  dist += ABS(b.b - x.b);
  dist += ABS(c.r - y.r);
  dist += ABS(c.g - y.g);
  dist += ABS(c.b - y.b);
  dist += ABS(d.r - z.r);
  dist += ABS(d.g - z.g);
  dist += ABS(d.b - z.b);
  return dist;
}

static uint16_t *MixBlock(uint16_t *p, struct TtyRgb ttl, struct TtyRgb ttr,
                          struct TtyRgb tbl, struct TtyRgb tbr,
                          struct TtyRgb qtl, struct TtyRgb qtr,
                          struct TtyRgb qbl, struct TtyRgb qbr) {
  unsigned char ttlr, ttlg, ttlb, qtlr, qtlg, qtlb, ttrr, ttrg, ttrb, qtrr,
      qtrg, qtrb, tblr, tblg, tblb, qblr, qblg, qblb, tbrr, tbrg, tbrb, qbrr,
      qbrg, qbrb, l00r, l00g, l00b, l01r, l01g, l01b, l02r, l02g, l02b, l03r,
      l03g, l03b, l04r, l04g, l04b, l05r, l05g, l05b, l06r, l06g, l06b, l07r,
      l07g, l07b, l08r, l08g, l08b, l09r, l09g, l09b, l10r, l10g, l10b, l11r,
      l11g, l11b, l12r, l12g, l12b, l13r, l13g, l13b, l14r, l14g, l14b, l15r,
      l15g, l15b, l16r, l16g, l16b, l17r, l17g, l17b, l18r, l18g, l18b, l19r,
      l19g, l19b, l20r, l20g, l20b, l21r, l21g, l21b, l22r, l22g, l22b, l23r,
      l23g, l23b, l24r, l24g, l24b, l25r, l25g, l25b, l26r, l26g, l26b, l27r,
      l27g, l27b, l28r, l28g, l28b, l29r, l29g, l29b, l30r, l30g, l30b, l31r,
      l31g, l31b;
  unsigned short p00, p01, p02, p03, p04, p05, p06, p07, p08, p09, p10, p11,
      p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26,
      p27, p28, p29, p30, p31;
  ttlr = ttl.r;
  ttlg = ttl.g;
  ttlb = ttl.b;
  qtlr = qtl.r;
  qtlg = qtl.g;
  qtlb = qtl.b;
  ttrr = ttr.r;
  ttrg = ttr.g;
  ttrb = ttr.b;
  qtrr = qtr.r;
  qtrg = qtr.g;
  qtrb = qtr.b;
  tblr = tbl.r;
  tblg = tbl.g;
  tblb = tbl.b;
  qblr = qbl.r;
  qblg = qbl.g;
  qblb = qbl.b;
  tbrr = tbr.r;
  tbrg = tbr.g;
  tbrb = tbr.b;
  qbrr = qbr.r;
  qbrg = qbr.g;
  qbrb = qbr.b;
  l00r = twixt8(qblr, qbrr, 0100);
  l00g = twixt8(qblg, qbrg, 0100);
  l00b = twixt8(qblb, qbrb, 0100);
  l01r = twixt8(qblr, qbrr, 0300);
  l01g = twixt8(qblg, qbrg, 0300);
  l01b = twixt8(qblb, qbrb, 0300);
  l02r = twixt8(qblr, qbrr, 0200);
  l02g = twixt8(qblg, qbrg, 0200);
  l02b = twixt8(qblb, qbrb, 0200);
  l03r = twixt8(qblr, qtlr, 0100);
  l03g = twixt8(qblg, qtlg, 0100);
  l03b = twixt8(qblb, qtlb, 0100);
  l04r = twixt8(qblr, qtlr, 0300);
  l04g = twixt8(qblg, qtlg, 0300);
  l04b = twixt8(qblb, qtlb, 0300);
  l05r = twixt8(qblr, qtlr, 0200);
  l05g = twixt8(qblg, qtlg, 0200);
  l05b = twixt8(qblb, qtlb, 0200);
  l06r = twixt8(qblr, qtrr, 0100);
  l06g = twixt8(qblg, qtrg, 0100);
  l06b = twixt8(qblb, qtrb, 0100);
  l07r = twixt8(qblr, qtrr, 0300);
  l07g = twixt8(qblg, qtrg, 0300);
  l07b = twixt8(qblb, qtrb, 0300);
  l08r = twixt8(qblr, qtrr, 0200);
  l08g = twixt8(qblg, qtrg, 0200);
  l08b = twixt8(qblb, qtrb, 0200);
  l09r = twixt8(qbrr, qtlr, 0100);
  l09g = twixt8(qbrg, qtlg, 0100);
  l09b = twixt8(qbrb, qtlb, 0100);
  l10r = twixt8(qbrr, qtlr, 0300);
  l10g = twixt8(qbrg, qtlg, 0300);
  l10b = twixt8(qbrb, qtlb, 0300);
  l11r = twixt8(qbrr, qtlr, 0200);
  l11g = twixt8(qbrg, qtlg, 0200);
  l11b = twixt8(qbrb, qtlb, 0200);
  l12r = twixt8(qbrr, qtrr, 0100);
  l12g = twixt8(qbrg, qtrg, 0100);
  l12b = twixt8(qbrb, qtrb, 0100);
  l13r = twixt8(qbrr, qtrr, 0300);
  l13g = twixt8(qbrg, qtrg, 0300);
  l13b = twixt8(qbrb, qtrb, 0300);
  l14r = twixt8(qbrr, qtrr, 0200);
  l14g = twixt8(qbrg, qtrg, 0200);
  l14b = twixt8(qbrb, qtrb, 0200);
  l15r = twixt8(qtlr, qblr, 0100);
  l15g = twixt8(qtlg, qblg, 0100);
  l15b = twixt8(qtlb, qblb, 0100);
  l16r = twixt8(qtlr, qblr, 0300);
  l16g = twixt8(qtlg, qblg, 0300);
  l16b = twixt8(qtlb, qblb, 0300);
  l17r = twixt8(qtlr, qblr, 0200);
  l17g = twixt8(qtlg, qblg, 0200);
  l17b = twixt8(qtlb, qblb, 0200);
  l18r = twixt8(qtlr, qbrr, 0100);
  l18g = twixt8(qtlg, qbrg, 0100);
  l18b = twixt8(qtlb, qbrb, 0100);
  l19r = twixt8(qtlr, qbrr, 0300);
  l19g = twixt8(qtlg, qbrg, 0300);
  l19b = twixt8(qtlb, qbrb, 0300);
  l20r = twixt8(qtlr, qbrr, 0200);
  l20g = twixt8(qtlg, qbrg, 0200);
  l20b = twixt8(qtlb, qbrb, 0200);
  l21r = twixt8(qtlr, qtrr, 0100);
  l21g = twixt8(qtlg, qtrg, 0100);
  l21b = twixt8(qtlb, qtrb, 0100);
  l22r = twixt8(qtlr, qtrr, 0300);
  l22g = twixt8(qtlg, qtrg, 0300);
  l22b = twixt8(qtlb, qtrb, 0300);
  l23r = twixt8(qtlr, qtrr, 0200);
  l23g = twixt8(qtlg, qtrg, 0200);
  l23b = twixt8(qtlb, qtrb, 0200);
  l24r = twixt8(qtrr, qblr, 0100);
  l24g = twixt8(qtrg, qblg, 0100);
  l24b = twixt8(qtrb, qblb, 0100);
  l25r = twixt8(qtrr, qblr, 0300);
  l25g = twixt8(qtrg, qblg, 0300);
  l25b = twixt8(qtrb, qblb, 0300);
  l26r = twixt8(qtrr, qblr, 0200);
  l26g = twixt8(qtrg, qblg, 0200);
  l26b = twixt8(qtrb, qblb, 0200);
  l27r = twixt8(qtrr, qbrr, 0100);
  l27g = twixt8(qtrg, qbrg, 0100);
  l27b = twixt8(qtrb, qbrb, 0100);
  l28r = twixt8(qtrr, qbrr, 0300);
  l28g = twixt8(qtrg, qbrg, 0300);
  l28b = twixt8(qtrb, qbrb, 0300);
  l29r = twixt8(qtrr, qbrr, 0200);
  l29g = twixt8(qtrg, qbrg, 0200);
  l29b = twixt8(qtrb, qbrb, 0200);
  l30r = twixt8(qtrr, qtlr, 0100);
  l30g = twixt8(qtrg, qtlg, 0100);
  l30b = twixt8(qtrb, qtlb, 0100);
  l31r = twixt8(qtrr, qtlr, 0300);
  l31g = twixt8(qtrg, qtlg, 0300);
  l31b = twixt8(qtrb, qtlb, 0300);
  p00 = JUDGE(l00);
  p01 = JUDGE(l01);
  p02 = JUDGE(l02);
  p03 = JUDGE(l03);
  p04 = JUDGE(l04);
  p05 = JUDGE(l05);
  p06 = JUDGE(l06);
  p07 = JUDGE(l07);
  p08 = JUDGE(l08);
  p09 = JUDGE(l09);
  p10 = JUDGE(l10);
  p11 = JUDGE(l11);
  p12 = JUDGE(l12);
  p13 = JUDGE(l13);
  p14 = JUDGE(l14);
  p15 = JUDGE(l15);
  p16 = JUDGE(l16);
  p17 = JUDGE(l17);
  p18 = JUDGE(l18);
  p19 = JUDGE(l19);
  p20 = JUDGE(l20);
  p21 = JUDGE(l21);
  p22 = JUDGE(l22);
  p23 = JUDGE(l23);
  p24 = JUDGE(l24);
  p25 = JUDGE(l25);
  p26 = JUDGE(l26);
  p27 = JUDGE(l27);
  p28 = JUDGE(l28);
  p29 = JUDGE(l29);
  p30 = JUDGE(l30);
  p31 = JUDGE(l31);
  *p++ = p00;
  *p++ = p01;
  *p++ = p02;
  *p++ = p03;
  *p++ = p04;
  *p++ = p05;
  *p++ = p06;
  *p++ = p07;
  *p++ = p08;
  *p++ = p09;
  *p++ = p10;
  *p++ = p11;
  *p++ = p12;
  *p++ = p13;
  *p++ = p14;
  *p++ = p15;
  *p++ = p16;
  *p++ = p17;
  *p++ = p18;
  *p++ = p19;
  *p++ = p20;
  *p++ = p21;
  *p++ = p22;
  *p++ = p23;
  *p++ = p24;
  *p++ = p25;
  *p++ = p26;
  *p++ = p27;
  *p++ = p28;
  *p++ = p29;
  *p++ = p30;
  *p++ = p31;
  return p;
}

static struct TtyRgb GetQuant(struct TtyRgb rgb) {
  return g_ansi2rgb_[rgb.xt];
}

static uint16_t *PickUnicode(uint16_t *p, struct TtyRgb tl, struct TtyRgb tr,
                             struct TtyRgb bl, struct TtyRgb br,
                             struct TtyRgb tl2, struct TtyRgb tr2,
                             struct TtyRgb bl2, struct TtyRgb br2) {
#define PICK(A, B, C, D) *p++ = GetBlockDist(tl, tr, bl, br, A, B, C, D)
  PICK(bl2, bl2, bl2, bl2); /*   k=0  bg=bl fg=NULL */
  PICK(bl2, bl2, bl2, br2); /* ▗ k=4  bg=bl fg=br   */
  PICK(bl2, bl2, bl2, tl2); /* ▗ k=4  bg=bl fg=tl   */
  PICK(bl2, bl2, bl2, tr2); /* ▗ k=4  bg=bl fg=tr   */
  PICK(bl2, bl2, br2, bl2); /* ▖ k=5  bg=bl fg=br   */
  PICK(bl2, bl2, br2, br2); /* ▄ k=1  bg=bl fg=br   */
  PICK(bl2, bl2, tl2, bl2); /* ▖ k=5  bg=bl fg=tl   */
  PICK(bl2, bl2, tl2, tl2); /* ▄ k=1  bg=bl fg=tl   */
  PICK(bl2, bl2, tr2, bl2); /* ▖ k=5  bg=bl fg=tr   */
  PICK(bl2, bl2, tr2, tr2); /* ▄ k=1  bg=bl fg=tr   */
  PICK(bl2, br2, bl2, bl2); /* ▝ k=3  bg=bl fg=br   */
  PICK(bl2, br2, bl2, br2); /* ▌ k=2  bg=br fg=bl   */
  PICK(bl2, br2, br2, bl2); /* ▞ k=6  bg=bl fg=br   */
  PICK(bl2, br2, br2, br2); /* ▘ k=7  bg=br fg=bl   */
  PICK(bl2, tl2, bl2, bl2); /* ▝ k=3  bg=bl fg=tl   */
  PICK(bl2, tl2, bl2, tl2); /* ▌ k=2  bg=tl fg=bl   */
  PICK(bl2, tl2, tl2, bl2); /* ▞ k=6  bg=bl fg=tl   */
  PICK(bl2, tl2, tl2, tl2); /* ▘ k=7  bg=tl fg=bl   */
  PICK(bl2, tr2, bl2, bl2); /* ▝ k=3  bg=bl fg=tr   */
  PICK(bl2, tr2, bl2, tr2); /* ▌ k=2  bg=tr fg=bl   */
  PICK(bl2, tr2, tr2, bl2); /* ▞ k=6  bg=bl fg=tr   */
  PICK(bl2, tr2, tr2, tr2); /* ▘ k=7  bg=tr fg=bl   */
  PICK(br2, bl2, bl2, bl2); /* ▘ k=7  bg=bl fg=br   */
  PICK(br2, bl2, bl2, br2); /* ▞ k=6  bg=br fg=bl   */
  PICK(br2, bl2, br2, bl2); /* ▌ k=2  bg=bl fg=br   */
  PICK(br2, bl2, br2, br2); /* ▝ k=3  bg=br fg=bl   */
  PICK(br2, br2, bl2, bl2); /* ▄ k=1  bg=br fg=bl   */
  PICK(br2, br2, bl2, br2); /* ▖ k=5  bg=br fg=bl   */
  PICK(br2, br2, br2, bl2); /* ▗ k=4  bg=br fg=bl   */
  PICK(br2, br2, br2, br2); /*   k=0  bg=br fg=NULL */
  PICK(br2, br2, br2, tl2); /* ▗ k=4  bg=br fg=tl   */
  PICK(br2, br2, br2, tr2); /* ▗ k=4  bg=br fg=tr   */
  PICK(br2, br2, tl2, br2); /* ▖ k=5  bg=br fg=tl   */
  PICK(br2, br2, tl2, tl2); /* ▄ k=1  bg=br fg=tl   */
  PICK(br2, br2, tr2, br2); /* ▖ k=5  bg=br fg=tr   */
  PICK(br2, br2, tr2, tr2); /* ▄ k=1  bg=br fg=tr   */
  PICK(br2, tl2, br2, br2); /* ▝ k=3  bg=br fg=tl   */
  PICK(br2, tl2, br2, tl2); /* ▌ k=2  bg=tl fg=br   */
  PICK(br2, tl2, tl2, br2); /* ▞ k=6  bg=br fg=tl   */
  PICK(br2, tl2, tl2, tl2); /* ▘ k=7  bg=tl fg=br   */
  PICK(br2, tr2, br2, br2); /* ▝ k=3  bg=br fg=tr   */
  PICK(br2, tr2, br2, tr2); /* ▌ k=2  bg=tr fg=br   */
  PICK(br2, tr2, tr2, br2); /* ▞ k=6  bg=br fg=tr   */
  PICK(br2, tr2, tr2, tr2); /* ▘ k=7  bg=tr fg=br   */
  PICK(tl2, bl2, bl2, bl2); /* ▘ k=7  bg=bl fg=tl   */
  PICK(tl2, bl2, bl2, tl2); /* ▞ k=6  bg=tl fg=bl   */
  PICK(tl2, bl2, tl2, bl2); /* ▌ k=2  bg=bl fg=tl   */
  PICK(tl2, bl2, tl2, tl2); /* ▝ k=3  bg=tl fg=bl   */
  PICK(tl2, br2, br2, br2); /* ▘ k=7  bg=br fg=tl   */
  PICK(tl2, br2, br2, tl2); /* ▞ k=6  bg=tl fg=br   */
  PICK(tl2, br2, tl2, br2); /* ▌ k=2  bg=br fg=tl   */
  PICK(tl2, br2, tl2, tl2); /* ▝ k=3  bg=tl fg=br   */
  PICK(tl2, tl2, bl2, bl2); /* ▄ k=1  bg=tl fg=bl   */
  PICK(tl2, tl2, bl2, tl2); /* ▖ k=5  bg=tl fg=bl   */
  PICK(tl2, tl2, br2, br2); /* ▄ k=1  bg=tl fg=br   */
  PICK(tl2, tl2, br2, tl2); /* ▖ k=5  bg=tl fg=br   */
  PICK(tl2, tl2, tl2, bl2); /* ▗ k=4  bg=tl fg=bl   */
  PICK(tl2, tl2, tl2, br2); /* ▗ k=4  bg=tl fg=br   */
  PICK(tl2, tl2, tl2, tl2); /*   k=0  bg=tl fg=NULL */
  PICK(tl2, tl2, tl2, tr2); /* ▗ k=4  bg=tl fg=tr   */
  PICK(tl2, tl2, tr2, tl2); /* ▖ k=5  bg=tl fg=tr   */
  PICK(tl2, tl2, tr2, tr2); /* ▄ k=1  bg=tl fg=tr   */
  PICK(tl2, tr2, tl2, tl2); /* ▝ k=3  bg=tl fg=tr   */
  PICK(tl2, tr2, tl2, tr2); /* ▌ k=2  bg=tr fg=tl   */
  PICK(tl2, tr2, tr2, tl2); /* ▞ k=6  bg=tl fg=tr   */
  PICK(tl2, tr2, tr2, tr2); /* ▘ k=7  bg=tr fg=tl   */
  PICK(tr2, bl2, bl2, bl2); /* ▘ k=7  bg=bl fg=tr   */
  PICK(tr2, bl2, bl2, tr2); /* ▞ k=6  bg=tr fg=bl   */
  PICK(tr2, bl2, tr2, bl2); /* ▌ k=2  bg=bl fg=tr   */
  PICK(tr2, bl2, tr2, tr2); /* ▝ k=3  bg=tr fg=bl   */
  PICK(tr2, br2, br2, br2); /* ▘ k=7  bg=br fg=tr   */
  PICK(tr2, br2, br2, tr2); /* ▞ k=6  bg=tr fg=br   */
  PICK(tr2, br2, tr2, br2); /* ▌ k=2  bg=br fg=tr   */
  PICK(tr2, br2, tr2, tr2); /* ▝ k=3  bg=tr fg=br   */
  PICK(tr2, tl2, tl2, tl2); /* ▘ k=7  bg=tl fg=tr   */
  PICK(tr2, tl2, tl2, tr2); /* ▞ k=6  bg=tr fg=tl   */
  PICK(tr2, tl2, tr2, tl2); /* ▌ k=2  bg=tl fg=tr   */
  PICK(tr2, tl2, tr2, tr2); /* ▝ k=3  bg=tr fg=tl   */
  PICK(tr2, tr2, bl2, bl2); /* ▄ k=1  bg=tr fg=bl   */
  PICK(tr2, tr2, bl2, tr2); /* ▖ k=5  bg=tr fg=bl   */
  PICK(tr2, tr2, br2, br2); /* ▄ k=1  bg=tr fg=br   */
  PICK(tr2, tr2, br2, tr2); /* ▖ k=5  bg=tr fg=br   */
  PICK(tr2, tr2, tl2, tl2); /* ▄ k=1  bg=tr fg=tl   */
  PICK(tr2, tr2, tl2, tr2); /* ▖ k=5  bg=tr fg=tl   */
  PICK(tr2, tr2, tr2, bl2); /* ▗ k=4  bg=tr fg=bl   */
  PICK(tr2, tr2, tr2, br2); /* ▗ k=4  bg=tr fg=br   */
  PICK(tr2, tr2, tr2, tl2); /* ▗ k=4  bg=tr fg=tl   */
  PICK(tr2, tr2, tr2, tr2); /*   k=0  bg=tr fg=NULL */
#undef PICK
  return p;
}

static uint16_t *PickCp437(uint16_t *p, struct TtyRgb tl, struct TtyRgb tr,
                           struct TtyRgb bl, struct TtyRgb br,
                           struct TtyRgb tl2, struct TtyRgb tr2,
                           struct TtyRgb bl2, struct TtyRgb br2) {
#define PICK(A, B, C, D) *p++ = GetBlockDist(tl, tr, bl, br, A, B, C, D)
  PICK(bl2, bl2, bl2, bl2); /*   k=0  bg=bl fg=NULL */
  PICK(bl2, bl2, br2, br2); /* ▄ k=1  bg=bl fg=br   */
  PICK(bl2, bl2, tl2, tl2); /* ▄ k=1  bg=bl fg=tl   */
  PICK(bl2, bl2, tr2, tr2); /* ▄ k=1  bg=bl fg=tr   */
  PICK(br2, br2, bl2, bl2); /* ▄ k=1  bg=br fg=bl   */
  PICK(tl2, tl2, bl2, bl2); /* ▄ k=1  bg=tl fg=bl   */
  PICK(tr2, tr2, bl2, bl2); /* ▄ k=1  bg=tr fg=bl   */
  PICK(bl2, br2, bl2, br2); /* ▌ k=2  bg=br fg=bl   */
  PICK(bl2, tl2, bl2, tl2); /* ▌ k=2  bg=tl fg=bl   */
  PICK(bl2, tr2, bl2, tr2); /* ▌ k=2  bg=tr fg=bl   */
  PICK(br2, bl2, br2, bl2); /* ▌ k=2  bg=bl fg=br   */
  PICK(br2, br2, br2, br2); /*   k=0  bg=br fg=NULL */
  PICK(br2, br2, tl2, tl2); /* ▄ k=1  bg=br fg=tl   */
  PICK(br2, br2, tr2, tr2); /* ▄ k=1  bg=br fg=tr   */
  PICK(br2, tl2, br2, tl2); /* ▌ k=2  bg=tl fg=br   */
  PICK(br2, tr2, br2, tr2); /* ▌ k=2  bg=tr fg=br   */
  PICK(tl2, bl2, tl2, bl2); /* ▌ k=2  bg=bl fg=tl   */
  PICK(tl2, br2, tl2, br2); /* ▌ k=2  bg=br fg=tl   */
  PICK(tl2, tl2, br2, br2); /* ▄ k=1  bg=tl fg=br   */
  PICK(tl2, tl2, tl2, tl2); /*   k=0  bg=tl fg=NULL */
  PICK(tl2, tl2, tr2, tr2); /* ▄ k=1  bg=tl fg=tr   */
  PICK(tl2, tr2, tl2, tr2); /* ▌ k=2  bg=tr fg=tl   */
  PICK(tr2, bl2, tr2, bl2); /* ▌ k=2  bg=bl fg=tr   */
  PICK(tr2, br2, tr2, br2); /* ▌ k=2  bg=br fg=tr   */
  PICK(tr2, tl2, tr2, tl2); /* ▌ k=2  bg=tl fg=tr   */
  PICK(tr2, tr2, br2, br2); /* ▄ k=1  bg=tr fg=br   */
  PICK(tr2, tr2, tl2, tl2); /* ▄ k=1  bg=tr fg=tl   */
  PICK(tr2, tr2, tr2, tr2); /*   k=0  bg=tr fg=NULL */
#undef PICK
  return p;
}

static struct Pick PickBlockUnicodeAnsi(struct TtyRgb tl, struct TtyRgb tr,
                                        struct TtyRgb bl, struct TtyRgb br) {
  struct TtyRgb tl2 = GetQuant(tl);
  struct TtyRgb tr2 = GetQuant(tr);
  struct TtyRgb bl2 = GetQuant(bl);
  struct TtyRgb br2 = GetQuant(br);
  unsigned p1, p2;
  uint16_t picks1[96] forcealign(32);
  uint16_t picks2[32] forcealign(32);
  memset(picks1, 0x79, sizeof(picks1));
  memset(picks2, 0x79, sizeof(picks2));
  PickUnicode(picks1, tl, tr, bl, br, tl2, tr2, bl2, br2);
  MixBlock(picks2, tl, tr, bl, br, tl2, tr2, bl2, br2);
  p1 = windex(picks1, 96);
  p2 = windex(picks2, 32);
  return picks1[p1] <= picks2[p2] ? kPicksUnicode[p1] : kPicksMixBlock[p2];
}

static struct Pick PickBlockUnicodeTrue(struct TtyRgb tl, struct TtyRgb tr,
                                        struct TtyRgb bl, struct TtyRgb br) {
  unsigned i;
  uint16_t picks[96] forcealign(32);
  memset(picks, 0x79, sizeof(picks));
  PickUnicode(picks, tl, tr, bl, br, tl, tr, bl, br);
  i = windex(picks, 96);
  return kPicksUnicode[i];
}

static struct Pick PickBlockCp437Ansi(struct TtyRgb tl, struct TtyRgb tr,
                                      struct TtyRgb bl, struct TtyRgb br) {
  struct TtyRgb tl2 = GetQuant(tl);
  struct TtyRgb tr2 = GetQuant(tr);
  struct TtyRgb bl2 = GetQuant(bl);
  struct TtyRgb br2 = GetQuant(br);
  unsigned p1, p2;
  uint16_t picks1[32] forcealign(32);
  uint16_t picks2[32] forcealign(32);
  memset(picks1, 0x79, sizeof(picks1));
  memset(picks2, 0x79, sizeof(picks2));
  PickCp437(picks1, tl, tr, bl, br, tl2, tr2, bl2, br2);
  MixBlock(picks2, tl, tr, bl, br, tl2, tr2, bl2, br2);
  p1 = windex(picks1, 32);
  p2 = windex(picks2, 32);
  return picks1[p1] <= picks2[p2] ? kPicksCp437[p1] : kPicksMixBlock[p2];
}

static struct Pick PickBlockCp437True(struct TtyRgb tl, struct TtyRgb tr,
                                      struct TtyRgb bl, struct TtyRgb br) {
  uint16_t picks[32] forcealign(32);
  memset(picks, 0x79, sizeof(picks));
  PickCp437(picks, tl, tr, bl, br, tl, tr, bl, br);
  return kPicksCp437[windex(picks, 32)];
}

static char *CopyGlyph(char *v, struct Glyph glyph) {
  memcpy(v, &glyph, 4);
  return v + glyph.len;
}

static char *CopyBlock(char *v, const struct TtyRgb chunk[hasatleast 4],
                       struct Pick pick, struct TtyRgb *bg, struct TtyRgb *fg,
                       struct Glyph *glyph) {
  unsigned i;
  CHECK_LT(pick.bg, 4);
  if (pick.fg != 0xff) CHECK_LT(pick.fg, 4);
  i = 0;
  if (pick.fg == 0xff) {
    if (!ttyeq(*bg, chunk[pick.bg])) {
      if (ttyeq(*fg, chunk[pick.bg])) {
        if (memcmp(glyph, &kGlyphs[1][0], sizeof(*glyph)) == 0) {
          v = setbg(v, (*bg = *fg));
        } else {
          i = 1;
        }
      } else {
        v = setbg(v, (*bg = chunk[pick.bg]));
      }
    }
  } else if (ttyeq(chunk[pick.bg], chunk[pick.fg])) {
    pick.k = 0;
    if (!ttyeq(*bg, chunk[pick.bg])) {
      if (ttyeq(*fg, chunk[pick.bg])) {
        if (memcmp(glyph, &kGlyphs[1][0], sizeof(*glyph)) == 0) {
          v = setbg(v, (*bg = *fg));
        } else {
          i = 1;
        }
      } else {
        v = setbg(v, (*bg = chunk[pick.bg]));
      }
    }
  } else if (!ttyeq(*fg, chunk[pick.fg])) {
    if (!ttyeq(*bg, chunk[pick.bg])) {
      if (ttyeq(*fg, chunk[pick.bg]) && ttyeq(*bg, chunk[pick.fg])) {
        if (pick.k == 0 && memcmp(glyph, &kGlyphs[1][0], sizeof(*glyph)) == 0) {
          v = setbg(v, (*bg = *fg));
        } else {
          i = 1;
        }
      } else {
        v = setbgfg(v, (*bg = chunk[pick.bg]), (*fg = chunk[pick.fg]));
      }
    } else {
      v = setfg(v, (*fg = chunk[pick.fg]));
    }
  } else if (!ttyeq(*bg, chunk[pick.bg])) {
    v = setbg(v, (*bg = chunk[pick.bg]));
  }
  return CopyGlyph(v, (*glyph = kGlyphs[i][pick.k]));
}

static bool ChunkEq(struct TtyRgb c[hasatleast 4],
                    struct TtyRgb c2[hasatleast 4]) {
  return ttyeq(c[TL], c[TR]) && ttyeq(c[BL], c[BR]) && ttyeq(c2[TL], c2[TR]) &&
         ttyeq(c2[BL], c2[BR]);
}

static struct TtyRgb *CopyChunk(struct TtyRgb chunk[hasatleast 4],
                                const struct TtyRgb *c, size_t xn) {
  chunk[TL] = c[00 + 0];
  chunk[TR] = c[00 + 1];
  chunk[BL] = c[xn + 0];
  chunk[BR] = c[xn + 1];
  return chunk;
}

static dontinline char *CopyRun(char *v, size_t n,
                                struct TtyRgb lastchunk[hasatleast 4],
                                const struct TtyRgb **c, size_t *x,
                                struct TtyRgb *bg, struct TtyRgb *fg,
                                struct Glyph *glyph) {
  struct TtyRgb chunk[4];
  if (memcmp(glyph, &kGlyphs[1][0], sizeof(*glyph)) == 0) {
    if (!ttyeq(*bg, *fg)) {
      v = setbg(v, (*bg = *fg));
    }
    *glyph = kGlyphs[0][0];
  }
  do {
    v = CopyGlyph(v, *glyph);
    *x += 2;
    *c += 2;
    if (*x >= n) break;
    CopyChunk(chunk, *c, n);
  } while (ChunkEq(chunk, lastchunk));
  *x -= 2;
  *c -= 2;
  return v;
}

/**
 * Maps 2×2 pixel chunks onto ANSI UNICODE cells.
 * @note h/t Nick Black for his quadrant blitting work on notcurses
 * @note yn and xn need to be even
 */
char *ttyraster(char *v, const struct TtyRgb *c, size_t yn, size_t xn,
                struct TtyRgb bg, struct TtyRgb fg) {
  unsigned y, x;
  struct Pick p;
  struct Glyph glyph;
  struct TtyRgb chun[4], lastchunk[4];
  for (y = 0; y < yn; y += 2, c += xn) {
    if (y) {
      v = stpcpy(v, "\e[0m\r\n");
      v = setbgfg(v, bg, fg);
    }
    for (x = 0; x < xn; x += 2, c += 2) {
      CopyChunk(chun, c, xn);
      if (ttyquant()->alg == kTtyQuantTrue) {
        if (ttyquant()->blocks == kTtyBlocksCp437) {
          p = PickBlockCp437True(chun[TL], chun[TR], chun[BL], chun[BR]);
        } else {
          p = PickBlockUnicodeTrue(chun[TL], chun[TR], chun[BL], chun[BR]);
        }
      } else {
        if (ttyquant()->blocks == kTtyBlocksCp437) {
          p = PickBlockCp437Ansi(chun[TL], chun[TR], chun[BL], chun[BR]);
        } else {
          p = PickBlockUnicodeAnsi(chun[TL], chun[TR], chun[BL], chun[BR]);
        }
      }
      v = CopyBlock(v, chun, p, &bg, &fg, &glyph);
      memcpy(lastchunk, chun, sizeof(chun));
    }
  }
  v = stpcpy(v, "\e[0m");
  return v;
}
