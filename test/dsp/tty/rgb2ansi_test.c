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
#include "dsp/tty/quant.h"
#include "libc/testlib/testlib.h"

struct TtyRgb res;

TEST(rgb2ansi, testDesaturatedPurple_isQuantizedBetterThanEuclideanDistance) {
  ttyquantsetup(kTtyQuantXterm256, kTtyQuantRgb, kTtyBlocksUnicode);

  /*
   * the challenge to the xterm256 palette is that it was likely
   * intended for just syntax highlighting, rather than accurately
   * modeling the natural phenomenon of illumination.
   *
   * as a syntax highlighting palette, it focuses mostly on bright
   * saturated colors, while also providing a really good greyscale for
   * everything else.
   *
   * as such, if one were to project the colors of this palette into a
   * three-dimensional space, we might see something like an HSV cone,
   * where all the color samples are projected mostly around the outside
   * of the cone, and the greyscale dots tracing through the middle.
   *
   * if we want to convert an a real color into an xterm color, we can
   * use euclidean distance functions to pick the closest color, such as
   * sum of squared distance. however this will only work well if it's
   * either a pure grey color, or a bright saturated one.
   *
   * but euclidean distance doesnt work well for the sorts of colors
   * that are generally used for things like film, which conservatively
   * edits for the colors more towards the middle of the space; and as
   * such, which basically causes the distance function to pick greys
   * for almost everything.
   */

  res = rgb2tty(0x56, 0x38, 0x66);

  /* EXPECT_NE(0x4e, res.r); */
  /* EXPECT_NE(0x4e, res.g); */
  /* EXPECT_NE(0x4e, res.b); */
  /* EXPECT_NE(239, res.xt); */

  /* EXPECT_EQ(0x5f, res.r); */
  /* EXPECT_EQ(0x00, res.g); */
  /* EXPECT_EQ(0x5f, res.b); */
  /* EXPECT_EQ(53, res.xt); */
}
