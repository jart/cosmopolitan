/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "dsp/scale/scale.h"
#include "libc/fmt/bing.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

TEST(magikarp, testConvolve) {
  signed char K[8] = {-1, -3, 3, 17, 17, 3, -3, -1};
  EXPECT_BINEQ(
      u"λλ  λλ  λλ  λλ  "
      u"λλ  λλ  λλ  λλ  "
      u"λλ  λλ  λλ  λλ  ",
      cDecimate2xUint8x8(32 * 3,
                         tgc(tunbing(u"λλλλ    λλλλ    λλλλ    λλλλ    "
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
                         tgc(tunbing(u"λλλλ    λλλλ    λλλλ    λλλλ    "
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
                         tgc(tunbing(u" ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼"
                                     u" !“#$%&‘()*+,-./0123456789:;<=>⁇"
                                     u"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                     u"`abcdefghijklmnopqrstuvwxyz{|}~⌂"
                                     u"ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥€ƒ"
                                     u"áíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐"
                                     u"└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀"
                                     u"αßΓπΣσμτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■λ")),
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
opppqqqqqppppoppoppppqqqqqppppo\n\
ppqqqqpppooooommmoooopppqqqqppp\n\
pqqqppponmmllllllllmmmnpppqqqqp\n\
qqqppoomllllllllllllllmnoppqqqq\n\
qqppoomlllllllmmmllllllmnoppqqq\n\
qppponlllllmoqttspnlllllmnoppqq\n\
qpponmllllmosyzz{vqnllllmmoppqq\n\
qpponmllllmorwzzyupnllllmmoppqq\n\
qqpponlllllloprrqomlllllmnoppqq\n\
qqpponnmlllllllllllllllmnnppqqq\n\
qqqppponmlllllllllllllnooppqqqp\n\
pqqqqpppoommmlllllmmmoopppqqqpp\n\
pppqqqqpppooooooooooppppqqqqppp\n\
oopppqqqqqqpppppppppqqqqqqpppoo\n\
noopopppqqqqqqqqqqqqqqqppoooonn",
               gc(bingblit(32, 61, M, 16, 31)));
}

#define HDX (1920 / 4)
#define HDY (1080 / 4)

BENCH(magikarp, bench) { /* 30ms */
  unsigned char kMagkern[16] = {4, 12, 12, 4};
  signed char kMagikarp[16] = {-1, -3, 3, 17, 17, 3, -3, -1};
  unsigned char(*Me)[HDY][HDX] = tgc(tmalloc((HDX + 1) * (HDY + 1)));
  unsigned char(*Mo)[HDY][HDX] = tgc(tmalloc((HDX + 1) * (HDY + 1)));
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
