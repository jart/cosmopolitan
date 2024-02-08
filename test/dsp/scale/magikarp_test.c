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
#include "dsp/core/q.h"
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "dsp/scale/scale.h"
#include "libc/assert.h"
#include "libc/fmt/bing.internal.h"
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

short *bingblit(int ys, int xs, unsigned char[ys][xs], int, int);

TEST(magikarp, testConvolve) {
  signed char K[8] = {-1, -3, 3, 17, 17, 3, -3, -1};
  EXPECT_BINEQ(
      u"λλ  λλ  λλ  λλ  "
      u"λλ  λλ  λλ  λλ  "
      u"λλ  λλ  λλ  λλ  ",
      cDecimate2xUint8x8(32 * 3,
                         gc(xunbing(u"λλλλ    λλλλ    λλλλ    λλλλ    "
                                    u"λλλλ    λλλλ    λλλλ    λλλλ    "
                                    u"λλλλ    λλλλ    λλλλ    λλλλ    ")),
                         K));
}

TEST(magikarp, testConvolveMin1) {
  signed char K[8] = {-1, -3, 3, 17, 17, 3, -3, -1};
  EXPECT_BINEQ(
      u"λλ  λλ  λλ  λλ  "
      u"λλ  λλ  λλ  λλ  "
      u"λλ  λλ  λλ  λλ  ",
      cDecimate2xUint8x8(32 * 3 - 1,
                         gc(xunbing(u"λλλλ    λλλλ    λλλλ    λλλλ    "
                                    u"λλλλ    λλλλ    λλλλ    λλλλ    "
                                    u"λλλλ    λλλλ    λλλλ    λλλλ   ")),
                         K));
}

TEST(magikarp, testConvolve2) {
  signed char K[8] = {-1, -3, 3, 17, 17, 3, -3, -1};
  EXPECT_BINEQ(
      u" ☻♣•○♂♪☼◄‼§↨↓←↔▼"
      u"!#%‘)+-/13579;=⁇"
      u"ACEGIKMOQSUWY[]_"
      u"acegikmoqsuwy{}⌂"
      u"üâàçëïìÅæôòùÖ¢¥ƒ"
      u"íúÑº⌐½¡»▒│╡╖╣╗╜┐"
      u"┴├┼╟╔╦═╧╤╙╒╫┘█▌▀"
      u"ßπστΘδφ∩±≤⌡≈∙√²λ",
      cDecimate2xUint8x8(256,
                         gc(xunbing(u" ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼"
                                    u" !“#$%&‘()*+,-./0123456789:;<=>⁇"
                                    u"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                    u"`abcdefghijklmnopqrstuvwxyz{|}~⌂"
                                    u"ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥€ƒ"
                                    u"áíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐"
                                    u"└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀"
                                    u"αßΓπΣσμτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙×√ⁿ²■λ")),
                         K));
}

const char kDieWelle[] = "\
nnooooooppppppppppqqqqqqqqqqqqqqqqqqqqqqqqqppppppppppoooooonn\
ooooooppppppppqqqqqqqqqqpppppppppppppqqqqqqqqqqppppppppoooooo\
oooopppppppqqqqqqqpppppppppppppppppppppppppqqqqqqqpppppppoooo\
oopppppppqqqqqqppppppppppoooooooooooppppppppppqqqqqqpppppppoo\
pppppppqqqqqpppppppoooooooonnnnnnnoooooooopppppppqqqqqppppppp\
pppppqqqqqppppppooooonnnnnnnmmmmmnnnnnnnoooooppppppqqqqqppppp\
ppppqqqqppppppooonnnnnmmmmmmmmmmmmmmmmmnnnnnoooppppppqqqqpppp\
ppqqqqqpppppooonnnmmmmmlllllllllllllllmmmmmnnnooopppppqqqqqpp\
pqqqqqppppooonnnmmmmlllllllllllllllllllllmmmmnnnoooppppqqqqqp\
qqqqpppppooonnmmmlllllllllllllllllllllllllllmmmnnooopppppqqqq\
qqqqppppooonnmmmllllllllllllmmmmmllllllllllllmmmnnoooppppqqqq\
qqqppppooonnmmmlllllllllmmmnnnnnnnmmmlllllllllmmmnnoooppppqqq\
qqpppppoonnmmmllllllllmmnoopqqqqqpoonmmllllllllmmmnnoopppppqq\
qqppppooonnmmllllllllmnnpqrsuvvvusrqpnnmllllllllmmnnoooppppqq\
qqppppoonnmmmlllllllmmnpqsuxyyyyyxusqpnmmlllllllmmmnnooppppqq\
qqppppoonnmmmlllllllmnoprtxyyyyyyyxtrponmlllllllmmmnnooppppqq\
qqppppoonnmmmlllllllmnoprtwyyyyyyywtrponmlllllllmmmnnooppppqq\
qqppppoonnnmmlllllllmmnoprtvxyyyxvtrponmmlllllllmmnnnooppppqq\
qqppppooonnmmllllllllmmnopqrsssssrqponmmllllllllmmnnoooppppqq\
qqqppppoonnnmmlllllllllmmnnoopppoonnmmlllllllllmmnnnooppppqqq\
qqqpppppoonnmmmllllllllllmmmmmmmmmmmllllllllllmmmnnoopppppqqq\
qqqqppppooonnnmmmlllllllllllllllllllllllllllmmmnnnoooppppqqqq\
pqqqqpppppoonnnmmmlllllllllllllllllllllllllmmmnnnoopppppqqqqp\
ppqqqqpppppooonnnmmmmlllllllllllllllllllmmmmnnnooopppppqqqqpp\
pppqqqqppppppooonnnnmmmmmlllllllllllmmmmmnnnnoooppppppqqqqppp\
ppppqqqqqppppppoooonnnnnmmmmmmmmmmmmmnnnnnooooppppppqqqqqpppp\
ppppppqqqqqpppppppooooonnnnnnnnnnnnnnnooooopppppppqqqqqpppppp\
opppppppqqqqqppppppppoooooooooooooooooooppppppppqqqqqpppppppo\
ooopppppppqqqqqqpppppppppppppppppppppppppppppqqqqqqpppppppooo\
ooooopppppppqqqqqqqqqpppppppppppppppppppqqqqqqqqqpppppppooooo\
noooooopppppppppqqqqqqqqqqqqqqqqqqqqqqqqqqqqqpppppppppoooooon\
nnnooooooopppppppppppqqqqqqqqqqqqqqqqqqqpppppppppppooooooonnn";

TEST(magikarp, testHalfYX) {
  static unsigned char M[32][61];
  memcpy(M, kDieWelle, sizeof(M));
  Magikarp2xY(32, 61, M, 32, 61);
  Magikarp2xX(32, 61, M, 16, 61);
  EXPECT_STREQ(u"\n\
nooppppqqqqqqqqqqqqqqqqqpppooon\n\
opppqqqqqpppppoooppppqqqqqppppo\n\
ppqqqqpppoooonnnnnooopppqqqqppp\n\
pqqqppponnmllllllllmnnnoppqqqqp\n\
qqqppoommlllllllllllllmnoppqqqq\n\
qqpponmllllllmmmmllllllmnoppqqq\n\
qppponlllllmoqttspolllllmnoppqq\n\
qpponmllllmosxzzyvqnllllmmoppqq\n\
qpponmllllmorwyyyupnllllmmoppqq\n\
qqpponllllllnprrqomlllllmnoppqq\n\
qqpponnmlllllllllllllllmnnppqqq\n\
qqqppponmllllllllllllmmooppqqqp\n\
pqqqqppoonmmmlllllmmnoopppqqqpp\n\
pppqqqqpppooooooooooopppqqqqppp\n\
oopppqqqqqqpppppppppqqqqqqpppoo\n\
noooopppqqqqqqqqqqqqqqqppoooonn",
               gc(bingblit(32, 61, M, 16, 31)));
}

#define HDX (1920 / 4)
#define HDY (1080 / 4)

BENCH(magikarp, bench) { /* 30ms */
  signed char kMagikarp[16] = {-1, -3, 3, 17, 17, 3, -3, -1};
  unsigned char(*Me)[HDY][HDX] = gc(malloc((HDX + 1) * (HDY + 1)));
  unsigned char(*Mo)[HDY][HDX] = gc(malloc((HDX + 1) * (HDY + 1)));
  if (X86_HAVE(AVX)) {
    EZBENCH2("Decimate2xUint8x8", donothing,
             cDecimate2xUint8x8((HDX * HDY - 16 - 8) / 2 / 8 * 8, (void *)Me,
                                kMagikarp));
  }
  EZBENCH2("cDecimate2xUint8x8", donothing,
           cDecimate2xUint8x8((HDX * HDY - 16 - 8) / 2 / 8 * 8, (void *)Me,
                              kMagikarp));
  EZBENCH2("Magikarp2xY", donothing, Magikarp2xY(HDY, HDX, *Me, HDY, HDX));
  EZBENCH2("Magikarp2xX", donothing, Magikarp2xX(HDY, HDX, *Mo, HDY, HDX));
}
