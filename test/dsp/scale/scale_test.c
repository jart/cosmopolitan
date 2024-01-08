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
#include "dsp/scale/scale.h"
#include "dsp/core/c1331.h"
#include "dsp/core/c161.h"
#include "dsp/core/core.h"
#include "dsp/core/half.h"
#include "libc/fmt/bing.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

short *bingblit(int ys, int xs, unsigned char[ys][xs], int, int);

void *AbsoluteDifference(int yn, int xn, unsigned char C[yn][xn], int ays,
                         int axs, const unsigned char A[ays][axs], int bys,
                         int bxs, const unsigned char B[bys][bxs]) {
  int y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      C[y][x] = ABS(A[y][x] - B[y][x]);
    }
  }
  return C;
}

const char kDieWelle[] = "\
pppppppppppppppppoooooooooooooooooooooooooooppppppppppppppppp\
pmpppppppppppooooooonnnnnnnnnnnnnnnnnnnnnooooooopppppppppppsp\
ppppppppppoooooonnnnnnnnmmmmmmmmmmmmmnnnnnnnnoooooopppppppppp\
ppppppppooooonnnnnnmmmmmmmmmmmmmmmmmmmmmmmnnnnnnooooopppppppp\
ppppppoooonnnnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnnnoooopppppp\
ppppoooonnnnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnnnoooopppp\
pppoooonnnnmmmmmmmmmmmmmmmnnnnnnnnnmmmmmmmmmmmmmmmnnnnooooppp\
ppooonnnnmmmmmmmmmmmmnnnnnnooooooonnnnnnmmmmmmmmmmmmnnnnooopp\
pooonnnnmmmmmmmmmmnnnnoooopppppppppoooonnnnmmmmmmmmmmnnnnooop\
ooonnnnmmmmmmmmmnnnooopppqqqqrrrqqqqpppooonnnmmmmmmmmmnnnnooo\
oonnnnmmmmmmmmmnnnooppqqrrsssssssssrrqqppoonnnmmmmmmmmmnnnnoo\
oonnnmmmmmmmmmnnooppqrrssttuuuuuuuttssrrqppoonnmmmmmmmmmnnnoo\
onnnnmmmmmmmmnnoopqqrsstuuvvvvvvvvvuutssrqqpoonnmmmmmmmmnnnno\
onnnmmmmmmmmnnnoppqrsttuvvwwwxxxwwwvvuttsrqpponnnmmmmmmmmnnno\
onnnmmmmmmmmnnoopqrrstuvvwxxxyyyxxxwvvutsrrqpoonnmmmmmmmmnnno\
onnnmmmmmmmmnnoopqrsttuvwwxxyyyyyxxwwvuttsrqpoonnmmmmmmmmnnno\
onnnmmmmmmmmnnoopqrsstuvwwxxyyyyyxxwwvutssrqpoonnmmmmmmmmnnno\
onnnmmmmmmmmnnoopqqrstuuvwwxxxxxxxwwvuutsrqqpoonnmmmmmmmmnnno\
onnnmmmmmmmmmnnoopqrrstuuvvwwwwwwwvvuutsrrqpoonnmmmmmmmmmnnno\
oonnnmmmmmmmmnnnoppqqrssttuuvvvvvuuttssrqqpponnnmmmmmmmmnnnoo\
oonnnmmmmmmmmmnnnooppqrrssstttttttsssrrqppoonnnmmmmmmmmmnnnoo\
ooonnnmmmmmmmmmmnnoooppqqrrrrrrrrrrrqqppooonnmmmmmmmmmmnnnooo\
oooonnnmmmmmmmmmmnnnnoooppppqqqqqppppooonnnnmmmmmmmmmmnnnoooo\
poooonnnnmmmmmmmmmmmnnnnooooooooooooonnnnmmmmmmmmmmmnnnnoooop\
ppoooonnnnmmmmmmmmmmmmmnnnnnnnnnnnnnnnmmmmmmmmmmmmmnnnnoooopp\
ppppoooonnnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnnoooopppp\
pppppoooonnnnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnnnooooppppp\
pppppppooooonnnnnmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnnnoooooppppppp\
pppppppppooooonnnnnnnmmmmmmmmmmmmmmmmmmmnnnnnnnoooooppppppppp\
pppppppppppooooooonnnnnnnnnnnnnnnnnnnnnnnnnoooooooppppppppppp\
prpppppppppppppoooooooonnnnnnnnnnnnnnnooooooooppppppppppppptp\
pppppppppppppppppppoooooooooooooooooooooooppppppppppppppppppp";

TEST(gyarados, testIdentityDifference) {
  static unsigned char A[1][32][61];
  static unsigned char B[1][32][61];
  ASSERT_EQ(sizeof(A), strlen(kDieWelle));
  memcpy(A, kDieWelle, sizeof(A));
  EzGyarados(1, 32, 61, B, 1, 32, 61, A, 0, 1, 32, 61, 32, 61, 1, 1, 0, 0);
  AbsoluteDifference(32, 61, B[0], 32, 61, B[0], 32, 61, A[0]);
  EXPECT_STREQ(u"\n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                             \n\
                                                           ☺ \n\
                                                             ",
               gc(bingblit(32, 61, B[0], 32, 61)));
}

TEST(gyarados, testHalfYX) {
  static unsigned char M[1][32][61];
  memcpy(M, kDieWelle, sizeof(M));
  EzGyarados(1, 32, 61, M, 1, 32, 61, M, 0, 1, 16, 31, 32, 61, 2, 2, 0, 0);
  EXPECT_STREQ(u"\n\
ppppppppoooooooooooooopppppppqp\n\
pppppoonnnmmmmmmmmmmmnnoooppppp\n\
ppponnnmmmmmmmmmmmmmmmmmnnooppp\n\
poonnmmmmmmmmnnnnnmmmmmmmmnnopp\n\
oonnmmmmmnnoppqqppoonnmmmmmnnop\n\
onnmmmmnooqrsttttsrqponmmmmmnno\n\
onmmmmnopqstvwwwwvutrqonmmmmnno\n\
onmmmmnoqrtvwxyyyxwusrpommmmmno\n\
onmmmmnoprtvwxyyyxvusqpnmmmmmno\n\
onmmmmmnpqrtuvwwvutsrponmmmmnno\n\
onnmmmmmnopqrssssrrqoonmmmmmnoo\n\
oonnmmmmmmnnoopppponnmmmmmmnoop\n\
pponnmmmmmmmmmmmmmmmmmmmmnnoopp\n\
pppoonnmmmmmmmmmmmmmmmmnnoopppp\n\
pppppooonnnmmmmmmmmnnnooopppppp\n\
pppppppppooooooooooooppppppppqp",
               gc(bingblit(32, 61, M[0], 16, 31)));
}

TEST(gyarados, testHalfY) {
  static unsigned char M[1][32][61];
  memcpy(M, kDieWelle, sizeof(M));
  EzGyarados(1, 32, 61, M, 1, 32, 61, M, 0, 1, 16, 61, 32, 61, 2, 1, 0, 0);
  EXPECT_STREQ(u"\n\
popppppppppppppppooooooooooooooooooooooooooopppppppppppppppqp\n\
ppppppppppooononnnnnmmmmmmmmmmmmmmmmmmmmmnnnnnonooopppppppppp\n\
ppppppoononnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnonoopppppp\n\
pppoononnmmmmmmmmmmmmmmmmmnnnnnnnnnmmmmmmmmmmmmmmmmmnnonooppp\n\
poonnnnmmmmmmmmmmmnnnnoooppppqqqppppooonnnnmmmmmmmmmmmnnnnoop\n\
oonnnnmmmmmmmmmnooppqqrrsstttttttttssrrqqppoonmmmmmmmmmnnnnoo\n\
onnnnmmmmmmmnnnoppqqsttuvvwwwwwwwwwvvuttsqqpponnnmmmmmmmnnnno\n\
onnnmmmmmmmmnnoopqrsttuvwwxxyyyyyxxwwvuttsrqpoonnmmmmmmmmnnno\n\
onnnmmmmmmmmnnoopqrsstuvwwxxyyyyyxxwwvutssrqpoonnmmmmmmmmnnno\n\
oonnnmmmmmmmmnnoopprrsttuuvvwwwwwvvuuttsrrppoonnmmmmmmmmnnnoo\n\
ooonnmmmmmmmmmmmnooooqqqrrrsssssssrrrqqqoooonmmmmmmmmmmmnnooo\n\
pooonnnmmmmmmmmmmmmmnnnnopoopppppooponnnnmmmmmmmmmmmmmnnnooop\n\
ppppoononnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnonoopppp\n\
pppppppoonoonnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnoonooppppppp\n\
pppppppppppoooooonnnnnnmmmmmmmmmmmmmmmnnnnnnoooooopppppppppqp\n\
pqpppppppppppppppppoooooooooooooooooooooooppppppppppppppppprp",
               gc(bingblit(32, 61, M[0], 16, 61)));
}

TEST(Magikarp2xY, testDecimateY) {
  static unsigned char M[1][32][61], G[1][16][61], D[1][16][61];
  memcpy(M, kDieWelle, sizeof(M));
  EzGyarados(1, 32, 61, G, 1, 32, 61, M, 0, 1, 16, 61, 32, 61, 2, 1, 0, 0);
  EXPECT_STREQ(u"\n\
popppppppppppppppooooooooooooooooooooooooooopppppppppppppppqp\n\
ppppppppppooononnnnnmmmmmmmmmmmmmmmmmmmmmnnnnnonooopppppppppp\n\
ppppppoononnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnonoopppppp\n\
pppoononnmmmmmmmmmmmmmmmmmnnnnnnnnnmmmmmmmmmmmmmmmmmnnonooppp\n\
poonnnnmmmmmmmmmmmnnnnoooppppqqqppppooonnnnmmmmmmmmmmmnnnnoop\n\
oonnnnmmmmmmmmmnooppqqrrsstttttttttssrrqqppoonmmmmmmmmmnnnnoo\n\
onnnnmmmmmmmnnnoppqqsttuvvwwwwwwwwwvvuttsqqpponnnmmmmmmmnnnno\n\
onnnmmmmmmmmnnoopqrsttuvwwxxyyyyyxxwwvuttsrqpoonnmmmmmmmmnnno\n\
onnnmmmmmmmmnnoopqrsstuvwwxxyyyyyxxwwvutssrqpoonnmmmmmmmmnnno\n\
oonnnmmmmmmmmnnoopprrsttuuvvwwwwwvvuuttsrrppoonnmmmmmmmmnnnoo\n\
ooonnmmmmmmmmmmmnooooqqqrrrsssssssrrrqqqoooonmmmmmmmmmmmnnooo\n\
pooonnnmmmmmmmmmmmmmnnnnopoopppppooponnnnmmmmmmmmmmmmmnnnooop\n\
ppppoononnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnonoopppp\n\
pppppppoonoonnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnoonooppppppp\n\
pppppppppppoooooonnnnnnmmmmmmmmmmmmmmmnnnnnnoooooopppppppppqp\n\
pqpppppppppppppppppoooooooooooooooooooooooppppppppppppppppprp",
               gc(bingblit(16, 61, G[0], 16, 61)));
  Magikarp2xY(32, 61, M[0], 32, 61);
  AbsoluteDifference(16, 61, D[0], 16, 61, G[0], 16, 61, M[0]);
  EXPECT_STREQ(u"\n\
popppppppppppppppooooooonnnnnnnnnnnnnooooooopppppppppppppppqp\n\
ppppppppppooooonnnnnmmmmmmmmmmmmmmmmmmmmmnnnnnooooopppppppppp\n\
ppppppoononnnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnnonoopppppp\n\
pppoononnmmmmmmmmmmmmmmmmmnnnnnnnnnmmmmmmmmmmmmmmmmmnnonooppp\n\
poonnnnmmmmmmmmmmmnnnnoooppppqqqppppooonnnnmmmmmmmmmmmnnnnoop\n\
oonnnnmmmmmmmmmnooppqqrrsstttttttttssrrqqppoonmmmmmmmmmnnnnoo\n\
onnnnmmmmmmmnnnoppqqsttuvvwwwwwwwwwvvuttsqqpponnnmmmmmmmnnnno\n\
onnnmmmmmmmmnnoopqrsttuvwwxxyyyyyxxwwvuttsrqpoonnmmmmmmmmnnno\n\
onnnmmmmmmmmnnoopqrsstuvwwxxyyyyyxxwwvutssrqpoonnmmmmmmmmnnno\n\
oonnnmmmmmmmmnnoopprrsttuuvvwwwwwvvuuttsrrppoonnmmmmmmmmnnnoo\n\
ooonnmmmmmmmmmmmnoopoqqqrrsssssssssrrqqqopoonmmmmmmmmmmmnnooo\n\
pooonnnmmmmmmmmmmmnmnnnoopoopppppoopoonnnmnmmmmmmmmmmmnnnooop\n\
ppppoononnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnonoopppp\n\
pppppppoonoonnmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmnnoonooppppppp\n\
pppppppppppoooooonnnnnnmmmmmmmmmmmmmmmnnnnnnooooooppppppppppp\n\
pqpppppppppppppppppoooooooooooooooooooooooppppppppppppppppprp",
               gc(bingblit(16, 61, M[0], 16, 61)));
}

TEST(Magikarp2xX, testDecimateX) {
  static unsigned char M[1][32][61], G[1][32][31], D[32][31];
  memcpy(M, kDieWelle, sizeof(M));
  EzGyarados(1, 32, 31, G, 1, 32, 61, M, 0, 1, 32, 31, 32, 61, 1, 2, 0, 0);
  EXPECT_STREQ(u"\n\
pppppppppoooooooooooooppppppppp\n\
oppppppooonnnnnnnnnnooooppppprp\n\
pppppooonnnnmmmmmmmnnnooopppppp\n\
ppppooonnmmmmmmmmmmmmnnnooppppp\n\
pppoonnmmmmmmmmmmmmmmmmnnnopppp\n\
ppoonnmmmmmmmmmmmmmmmmmmnnooppp\n\
ppoonnmmmmmmmnnnnmmmmmmmmnnoopp\n\
ponnnmmmmmmnnnooonnnmmmmmmnnopp\n\
ponnmmmmmnnooppppoonnmmmmmnnnop\n\
oonnmmmmnoopqqrrqqpoonmmmmmnnoo\n\
onnmmmmmnopqrsssssqponnmmmmnnoo\n\
onmmmmmnopqstuuuutsrqoommmmmnoo\n\
onnmmmmnpqrtuvvvvvtsrponmmmmnno\n\
onmmmmnopqtuvwxxwwutsqonmmmmmno\n\
onmmmmnoqrsvwxyyxxvusqpnmmmmmno\n\
onmmmmnopstvwxyyyxwutronmmmmmno\n\
onmmmmnopstvwxyyyxwtsrpnmmmmmno\n\
onmmmmnoprtuvxxxxwvurqpnmmmmmno\n\
onmmmmmnpqsuvwwwwvutrqonmmmmmno\n\
onmmmmmnoqrstuvvvutrqponmmmmnoo\n\
onmmmmmnnoqrsttttssqponmmmmmnoo\n\
oonmmmmmnooqrrrrrrqpoommmmmmnoo\n\
oonmmmmmmnnoppqqppoonnmmmmmnooo\n\
poonnmmmmmnnoooooonnmmmmmmnnoop\n\
poonnmmmmmmmnnnnnnnmmmmmmnnoopp\n\
ppoonnmmmmmmmmmmmmmmmmmmmnooppp\n\
pppoonnmmmmmmmmmmmmmmmmmnnooppp\n\
ppppoonnmmmmmmmmmmmmmmnnooopppp\n\
pppppoonnnmmmmmmmmmmnnnoooppppp\n\
ppppppooonnnnnnnnnnnnoooopppppp\n\
qpppppppoooonnnnnnnoooopppppprp\n\
ppppppppppooooooooooopppppppppp",
               gc(bingblit(32, 31, G[0], 32, 31)));
  Magikarp2xX(32, 61, M[0], 32, 61);
  EXPECT_STREQ(u"\n\
pppppppppoooooooooooooppppppppp\n\
oppppppooonnnnnnnnnnoooopppppqp\n\
pppppooonnnnmmmmmmnnnnooopppppp\n\
ppppooonnnmmmmmmmmmmmnnnooppppp\n\
pppoonnnmmmmmmmmmmmmmmmnnoopppp\n\
ppoonnnmmmmmmmmmmmmmmmmmnnooppp\n\
ppoonnmmmmmmmnnnnnmmmmmmmnnoopp\n\
ponnnmmmmmnnnoooonnnmmmmmmnnopp\n\
ponnmmmmmnnoopppppoonnmmmmnnnop\n\
oonnmmmmnoopqqrrqqpponmmmmmnnoo\n\
onnmmmmmnopqrsssssrqpnnmmmmnnoo\n\
onmmmmmnopqstuuuutsrqoommmmmnoo\n\
onnmmmmnpqrtuvvvvvtsrponmmmmnno\n\
onmmmmnopqsuvwxxwwutsqonmmmmmno\n\
onmmmmnoqrsuwxyyxwvusqpnmmmmmno\n\
onmmmmnopstvwxyyyxvusrpnmmmmmno\n\
onmmmmnoprtvwxyyyxwtsrpnmmmmmno\n\
onmmmmnoprtuvxxxxwvtrqpnmmmmmno\n\
onmmmmmnpqstvwwwwvutrqonmmmmmno\n\
onmmmmmnoqrstuvvvutrqponmmmmnoo\n\
onmmmmmnnoqrsttttssqponmmmmmnoo\n\
oonmmmmmnooqrrrrrrqpoommmmmmnoo\n\
oonmmmmmnnnoppqqqpponnmmmmmnooo\n\
poonnmmmmmnnooooooonnmmmmmnnoop\n\
poonnmmmmmmnnnnnnnnmmmmmmnnoopp\n\
ppoonnmmmmmmmmmmmmmmmmmmnnooppp\n\
pppoonnmmmmmmmmmmmmmmmmnnnooppp\n\
ppppoonnnmmmmmmmmmmmmmnnooopppp\n\
pppppoonnnnmmmmmmmmmnnnoooppppp\n\
ppppppooonnnnnnnnnnnnoooopppppp\n\
qpppppppoooonnnnnnnoooopppppprp\n\
ppppppppppooooooooooopppppppppp",
               gc(bingblit(32, 61, M[0], 32, 31)));
  AbsoluteDifference(32, 31, D, 32, 31, G[0], 32, 61, M[0]);
}

TEST(magikarp_vs_gyarados, testHalf) {
  static unsigned char M[1][32][61], G[1][16][31], D[16][31];
  memcpy(M, kDieWelle, sizeof(M));
  EzGyarados(1, 16, 31, G, 1, 32, 61, M, 0, 1, 16, 31, 32, 61, 32. / 16.,
             61. / 31., 0, 0);
  Magikarp2xY(32, 61, M[0], 32, 61);
  Magikarp2xX(32, 61, M[0], 16, 61);
  AbsoluteDifference(16, 31, D, 16, 31, G[0], 32, 61, M[0]);
  EXPECT_STREQ(u"\n\
pppppppppooonnnnnnooooppppppppp\n\
pppppooonnmmmmmmmmmmnnnoopppppp\n\
ppponnmmmmmmmmmmmmmmmmmmnnopppp\n\
pponmmmmmmmmmnnnnnmmmmmmmmnoopp\n\
pnnnmmmmmnnoppqqpponnnmmmmmnnop\n\
onnmmmmnopqrstttttsrqpnmmmmnnoo\n\
onnmmmnopqsuvwwwwwutrponmmmmnno\n\
onmmmmnopstvwxyyyxvusrpnmmmmmno\n\
onmmmmnoprtvwxyyyxwtsrpnmmmmmno\n\
onmmmmmooqstuvwwwvusrponmmmmnoo\n\
oommmmmmnopqrsssssrqponmmmmmnoo\n\
ponmmmmmmnnopoppooonnmmmmmmnoop\n\
ppoonmmmmmmmmmmmmmmmmmmmmmnoppp\n\
ppppoonmmmmmmmmmmmmmmmmmooopppp\n\
ppppppooonnnmmmmmmmnnnooopppppp\n\
ppppppppppoooooooooooppppppppqp",
               gc(bingblit(32, 61, M[0], 16, 31)));
  EXPECT_STREQ(u"\n\
ppppppppooooooooooooooopppppppp\n\
pppppoonnnmmmmmmmmmmmnnnooppppp\n\
pppoonnmmmmmmmmmmmmmmmmmnnooppp\n\
poonnmmmmmmmmnnnnnmmmmmmmmnnoop\n\
oonnmmmmmnnoppqqqpponnmmmmmnnoo\n\
onnmmmmnnoqrstttttsrqonnmmmmnno\n\
onmmmmnnpqstuwwwwwutsqpnnmmmmno\n\
onmmmmnoprtuwxyzyxwutrponmmmmno\n\
onmmmmnoprsuwxyyyxwusrponmmmmno\n\
onmmmmmnoqrtuvwwwvutrqonmmmmmno\n\
onnmmmmmnopqrsssssrqponmmmmmnno\n\
oonnmmmmmmnnoopppoonnmmmmmmnnoo\n\
ppoonmmmmmmmmmmmmmmmmmmmmmnoopp\n\
pppoonnmmmmmmmmmmmmmmmmmnnooppp\n\
pppppooonnnmmmmmmmmmnnnoooppppp\n\
pppppppppoooooooooooooppppppppq",
               gc(bingblit(16, 31, G[0], 16, 31)));
  EXPECT_STREQ(u"\n\
        ☺   ☺☺☺☺☺☺    ☺        \n\
       ☺            ☺  ☺ ☺     \n\
    ☺ ☺                    ☺   \n\
 ☺  ☺                      ☺ ☺ \n\
☺☺              ☺ ☺☺          ☺\n\
        ☺☺           ☺ ☺   ☺ ☺ \n\
  ☺    ☺   ☺☺       ☺☺☺ ☺   ☺  \n\
         ☺ ☺   ☺  ☺ ☺  ☺☺      \n\
          ☺☺       ☺   ☺☺      \n\
       ☺  ☺        ☺ ☺      ☺☺ \n\
 ☺☺                          ☺ \n\
☺  ☺     ☺ ☺☺   ☺           ☺ ☺\n\
                            ☺  \n\
   ☺ ☺                  ☺☺ ☺   \n\
     ☺  ☺  ☺       ☺  ☺  ☺     \n\
         ☺           ☺       ☺☺",
               gc(bingblit(16, 31, D, 16, 31)));
}

#define HDX (1920 / 4)
#define HDY (1080 / 4)

#if 0
BENCH(Magikarp, bench) {
  unsigned char(*Me)[HDY][HDX] = tgc(tmalloc(HDX * HDY));
  unsigned char(*Mo)[HDY][HDX] = tgc(tmalloc(HDX * HDY));
  EZBENCH2("Magikarp2xY [even]", donothing,
           Magikarp2xY(HDX, HDY, *Me, HDX, HDY));
  EZBENCH2("Magikarp2xY [odd]", donothing,
           Magikarp2xY(HDX, HDY, *Mo, HDX, HDY));
  EZBENCH2("Magikarp2xX [even]", donothing,
           Magikarp2xX(HDX, HDY, *Me, HDX, HDY));
  EZBENCH2("Magikarp2xX [odd]", donothing,
           Magikarp2xX(HDX, HDY, *Mo, HDX, HDY));
}
#endif
