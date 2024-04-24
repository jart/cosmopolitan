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

// The modes below use various unicodes for 'progressive' pixelization:
// each mode supersets the previous to increase resolution more and more.
// Ideally, a fully dense mapping of the (Y*X) space defined by kGlyph size
// would produce a picture perfect image, but at the cost of sampling speed.
// Therefore, supersets are parsimonious: they only add the minimal set of
// missing shapes that can increase resolution.
// Ideally, this should be based on a study of the residual, but some logic
// can go a long way: after some block pixelization, will need diagonals
// FIXME: then shouldn't box drawing go right after braille?

// TODO: explain the differences between each mode:
// Mode A is full, empty, half blocks top and bottom: , █,▄,▀
// Mode B superset: with quadrants:  ▐,▌,▝,▙,▗,▛,▖,▜,▘,▟,▞,▚,
// Mode C superset: with fractional eights along X and Y
//  _,▁,▂,▃,▄,▅,▆,▇ :█:▉,▊,▋,▌,▍,▎,▏
// Mode X use box drawing, mode X use diagonal blocks, mode X use braille etc

#define W(B, S) B##U << S
#define G(AA, AB, AC, AD, BA, BB, BC, BD, CA, CB, CC, CD, DA, DB, DC, DD, EA, \
          EB, EC, ED, FA, FB, FC, FD, GA, GB, GC, GD, HA, HB, HC, HD)         \
  (W(AA, 000) | W(AB, 001) | W(AC, 002) | W(AD, 003) | W(BA, 004) |           \
   W(BB, 005) | W(BC, 006) | W(BD, 007) | W(CA, 010) | W(CB, 011) |           \
   W(CC, 012) | W(CD, 013) | W(DA, 014) | W(DB, 015) | W(DC, 016) |           \
   W(DD, 017) | W(EA, 020) | W(EB, 021) | W(EC, 022) | W(ED, 023) |           \
   W(FA, 024) | W(FB, 025) | W(FC, 026) | W(FD, 027) | W(GA, 030) |           \
   W(GB, 031) | W(GC, 032) | W(GD, 033) | W(HA, 034) | W(HB, 035) |           \
   W(HC, 036) | W(HD, 037))

// The glyph size it set by the resolution of the most precise mode, ex:
// - Mode C: along the X axis, need >= 8 steps for the 8 fractional width
// FIXME: now we can only use 4 chars instead of the extra ▉,▊,▋,▌,▍,▎,▏
//
// - Mode X: along the Y axis, need >= 8 steps to separate the maximal 6 dots
// from the space left below, seen by overimposing an underline  ⠿_
// along the 3 dots, the Y axis is least 1,0,1,0,1,0,0,1 so 8 steps
//
// Problem: fonts are taller than wider, and terminals are traditionally 80x24, so
// - we shouldn't use square glyphs, 8x16 seems to be the minimal size
// - we should adapt the conversion to BMP to avoid accidental Y downsampling

const uint32_t kGlyphs[] = /* clang-format off */ {
    /* U+0020 ' ' empty block [ascii:20,cp437:20] */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
    /* U+2588 '█' full block [cp437] */
    G(1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1),
    /* U+2584 '▄' lower half block [cp437:dc] */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1),
    /* U+2580 '▀' upper half block [cp437] */
    G(1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
    // Mode B
    /* U+2590 '▐' right half block [cp437:de] */
    G(0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1),
    /* U+258c '▌' left half block [cp437] */
    G(1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0),
    /* U+259d '▝' quadrant upper right */
    G(0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
    /* U+2599 '▙' quadrant upper left and lower left and lower right */
    G(1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,0),
    /* U+2597 '▗' quadrant lower right */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1),
    /* U+259b '▛' quadrant upper left and upper right and lower left */
    G(1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,1),
    /* U+2596 '▖' quadrant lower left */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0),
    /* U+259c '▜' quadrant upper left and upper right and lower right */
    G(1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,0),
    /* U+2598 '▘' quadrant upper left */
    G(1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
    /* U+259F '▟' quadrant upper right and lower left and lower right */
    G(0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,0),
    /* U+259e '▞' quadrant upper right and lower left */
    G(0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0),
    /* U+259a '▚' quadrant upper left and lower right */
    G(1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      1,1,0,0,
      0,0,1,1,
      0,0,1,1,
      0,0,1,1,
      0,0,1,0),
    // Mode C
    /* U+2594 '▔' upper one eighth block */
    G(1,1,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
    /* U+2581 '▁' lower one eighth block */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1),
    /* U+2582 '▂' lower one quarter block */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,1,1),
    /* U+2583 '▃' lower three eighths block */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1),
    /* U+2585 '▃' lower five eighths block */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1),
    /* U+2586 '▆' lower three quarters block */
    G(0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1),
    /* U+2587 '▇' lower seven eighths block */
    G(0,0,0,0,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1,
      1,1,1,1),
    /* U+258e '▎' left one quarter block */
    G(1,0,0,0,
      1,0,0,0,
      1,0,0,0,
      1,0,0,0,
      1,0,0,0,
      1,0,0,0,
      1,0,0,0,
      1,0,0,0),
    /* U+258a '▊' left three quarters block */
    G(1,1,1,0,
      1,1,1,0,
      1,1,1,0,
      1,1,1,0,
      1,1,1,0,
      1,1,1,0,
      1,1,1,0,
      1,1,1,0),
      /* ▁ *\
    2501▕━▎box drawings heavy horizontal
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
   25019▕┉▎box drawings heavy quadruple dash horizontal
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,0,1,0,
      0,1,0,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    2503▕┃▎box drawings heavy vertical
      \* ▔ */
    G(0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0),
      /* ▁ *\
    254b▕╋▎box drawings heavy vertical and horizontal
      \* ▔ */
    G(0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      1,1,1,1,
      1,1,1,1,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0),
      /* ▁ *\
    2579▕╹▎box drawings heavy up
      \* ▔ */
    G(0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    257a▕╺▎box drawings heavy right
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,1,1,
      0,0,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    257b▕╻▎box drawings heavy down
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0),
      /* ▁ *\
    2578▕╸▎box drawings heavy left
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,0,0,
      1,1,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    250f▕┏▎box drawings heavy down and right
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,1,1,1,
      0,1,1,1,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0),
      /* ▁ *\
    251b▕┛▎box drawings heavy up and left
      \* ▔ */
    G(0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      1,1,1,0,
      1,1,1,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    2513▕┓▎box drawings heavy down and left
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,0,
      1,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,0),
      /* ▁ *\
    2517▕┗▎box drawings heavy up and right
      \* ▔ */
    G(0,1,1,0,
      0,1,1,0,
      0,1,1,0,
      0,1,1,1,
      0,1,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    25E2▕◢▎black lower right triangle
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,1,
      0,0,1,1,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    25E3▕◣▎black lower left triangle
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,0,0,0,
      1,1,0,0,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    25E4▕◥▎black upper right triangle
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      0,0,1,1,
      0,0,0,1,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    25E5▕◤▎black upper left triangle
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      1,1,0,0,
      1,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    2500▕═▎box drawings double horizontal
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      0,0,0,0,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    23BB▕⎻▎horizontal scan line 3
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0),
      /* ▁ *\
    23BD▕⎼▎horizontal scan line 9
      \* ▔ */
    G(0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      1,1,1,1,
      0,0,0,0,
      0,0,0,0),
} /* clang-format on */;

const char16_t kRunes[] = {
    u' ', /* 0020 empty block [ascii:20,cp437:20] */
    u'█', /* 2588 full block [cp437] */
    u'▄', /* 2584 lower half block [cp437:dc] */
    u'▀', /* 2580 upper half block [cp437] */
    u'▐', /* 2590 right half block [cp437:de] */
    u'▌', /* 258C left half block */
    u'▝', /* 259D quadrant upper right */
    u'▙', /* 2599 quadrant upper left and lower left and lower right */
    u'▗', /* 2597 quadrant lower right */
    u'▛', /* 259B quadrant upper left and upper right and lower left */
    u'▖', /* 2596 quadrant lower left */
    u'▜', /* 259C quadrant upper left and upper right and lower right */
    u'▘', /* 2598 quadrant upper left */
    u'▟', /* 259F quadrant upper right and lower left and lower right */
    u'▞', /* 259E quadrant upper right and lower left */
    u'▚', /* 259A quadrant upper left and lower right */
    u'▔', /* 2594 upper one eighth block */
    u'▁', /* 2581 lower one eighth block */
    u'▂', /* 2582 lower one quarter block */
    u'▃', /* 2583 lower three eighths block */
    u'▅', /* 2585 lower five eighths block */
    u'▆', /* 2586 lower three quarters block */
    u'▇', /* 2587 lower seven eighths block */
    u'▎', /* 258E left one quarter block */
    u'▊', /* 258A left three quarters block */
    u'━', /* 2501 box drawings heavy horizontal */
    u'┉', /* 2509 box drawings heavy quadruple dash horizontal */
    u'┃', /* 2503 box drawings heavy vertical */
    u'╋', /* 254B box drawings heavy vertical & horiz. */
    u'╹', /* 2579 box drawings heavy up */
    u'╺', /* 257A box drawings heavy right */
    u'╻', /* 257B box drawings heavy down */
    u'╸', /* 2578 box drawings heavy left */
    u'┏', /* 250F box drawings heavy down and right */
    u'┛', /* 251B box drawings heavy up and left */
    u'┓', /* 2513 box drawings heavy down and left */
    u'┗', /* 2517 box drawings heavy up and right */
    u'◢', /* 25E2 black lower right triangle */
    u'◣', /* 25E3 black lower left triangle */
    u'◥', /* 25E4 black upper right triangle */
    u'◤', /* 25E5 black upper left triangle */
    u'═', /* 2550 box drawings double horizontal */
    u'⎻', /* 23BB horizontal scan line 3 */
    u'⎼', /* 23BD horizontal scan line 9 */
};
