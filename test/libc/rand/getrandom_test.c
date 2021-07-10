/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/bits.h"
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/lcg.internal.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

/* JustReturnZero                   */
/* entropy:            0            */
/* chi-square:         2.55e+07     */
/* chi-square percent: 0            */
/* mean:               0            */
/* monte-carlo-pi:     27.324       */
/* serial-correlation: -100000      */

/* JustIncrement                    */
/* entropy:            2.63951      */
/* chi-square:         1.443e+07    */
/* chi-square percent: 0            */
/* mean:               18.8803      */
/* monte-carlo-pi:     27.324       */
/* serial-correlation: 0.0092003    */

/* UNIX Sixth Edition               */
/* entropy:            8            */
/* chi-square:         0.1536       */
/* chi-square percent: 1            */
/* mean:               127.502      */
/* monte-carlo-pi:     3.4192       */
/* serial-correlation: -0.470645    */

/* UNIX Seventh Edition             */
/* entropy:            7.99818      */
/* chi-square:         251.843      */
/* chi-square percent: 0.544128     */
/* mean:               127.955      */
/* monte-carlo-pi:     0.675703     */
/* serial-correlation: -0.00207669  */

/* KnuthLcg                         */
/* entropy:            7.99835      */
/* chi-square:         228.383      */
/* chi-square percent: 0.883476     */
/* mean:               127.1        */
/* monte-carlo-pi:     0.561935     */
/* serial-correlation: -0.0038954   */

/* rand64                           */
/* entropy:            7.99832      */
/* chi-square:         233.267      */
/* chi-square percent: 0.831821     */
/* mean:               127.427      */
/* monte-carlo-pi:     0.0271532    */
/* serial-correlation: -0.00255319  */

/* Rand64LowByte                    */
/* entropy:            7.99798      */
/* chi-square:         278.344      */
/* chi-square percent: 0.150796     */
/* mean:               127.88       */
/* monte-carlo-pi:     0.00340573   */
/* serial-correlation: 0.00162231   */

/* GetRandomNoSystem                */
/* entropy:            7.99819      */
/* chi-square:         249.743      */
/* chi-square percent: 0.58114      */
/* mean:               127.124      */
/* monte-carlo-pi:     0.293716     */
/* serial-correlation: 0.00198516   */

/* GetRandomNoRdrrnd                */
/* entropy:            7.99816      */
/* chi-square:         254.797      */
/* chi-square percent: 0.491811     */
/* mean:               127.308      */
/* monte-carlo-pi:     0.0118738    */
/* serial-correlation: 0.000197669  */

/* GetRandom                        */
/* entropy:            7.99808      */
/* chi-square:         266.737      */
/* chi-square percent: 0.294131     */
/* mean:               127.178      */
/* monte-carlo-pi:     0.0577122    */
/* serial-correlation: 0.00598793   */

typedef uint64_t (*random_f)(void);

static uint32_t randx = 1;

uint64_t JustReturnZero(void) {
  return 0;
}

uint64_t JustIncrement(void) {
  static uint64_t x;
  return x++;
}

uint16_t SixthEditionRand(void) {
  static int16_t gorp;
  gorp = (gorp + 625) & 077777;
  return gorp;
}

uint64_t SixthEditionLowByte(void) {
  unsigned i;
  uint64_t x;
  for (x = i = 0; i < 8; ++i) {
    x <<= 8;
    x |= SixthEditionRand() & 255;
  }
  return x;
}

uint64_t MobyDick(void) {
  static int i;
  if ((i += 8) > kMobySize) i = 8;
  return READ64LE(kMoby + i);
}

uint64_t ExecutableImage(void) {
  static int i;
  if ((i += 8) > _end - _base) i = 8;
  return READ64LE(_base + i);
}

uint32_t SeventhEditionRand(void) {
  return ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
}

uint64_t SeventhEditionLowByte(void) {
  unsigned i;
  uint64_t x;
  for (x = i = 0; i < 8; ++i) {
    x <<= 8;
    x |= SeventhEditionRand() & 255;
  }
  return x;
}

uint64_t KnuthLcg(void) {
  unsigned i;
  uint64_t x;
  for (x = i = 0; i < 8; ++i) {
    x <<= 8;
    x |= rand() & 255;
  }
  return x;
}

uint64_t Rand64LowByte(void) {
  unsigned i;
  uint64_t x;
  for (x = i = 0; i < 8; ++i) {
    x <<= 8;
    x |= rand64() & 255;
  }
  return x;
}

uint64_t GetRandomNoRdrrnd(void) {
  uint64_t x;
  ASSERT_EQ(8, getrandom(&x, 8, GRND_NORDRND));
  return x;
}

uint64_t GetRandomNoSystem(void) {
  uint64_t x;
  if (X86_HAVE(RDRND) || X86_HAVE(RDSEED)) {
    ASSERT_EQ(8, getrandom(&x, 8, GRND_NOSYSTEM));
  } else {
    ASSERT_EQ(8, getrandom(&x, 8, 0));
  }
  return x;
}

uint64_t GetRandom(void) {
  uint64_t x;
  ASSERT_EQ(8, getrandom(&x, 8, 0));
  return x;
}

static const struct RandomFunction {
  const char *s;
  random_f f;
  bool r;
} kRandomFunctions[] = {
    {"JustReturnZero", JustReturnZero, false},                //
    {"JustIncrement", JustIncrement, false},                  //
    {"MobyDick", MobyDick, false},                            //
    {"ExecutableImage", ExecutableImage, false},              //
    {"SixthEditionLowByte", SixthEditionLowByte, false},      //
    {"SeventhEditionLowByte", SeventhEditionLowByte, false},  //
    {"KnuthLcg", KnuthLcg, false},                            //
    {"rand64", rand64, true},                                 //
    {"Rand64LowByte", Rand64LowByte, true},                   //
    {"GetRandomNoRdrrnd", GetRandomNoRdrrnd, true},           //
    {"GetRandomNoSystem", GetRandomNoSystem, true},           //
    {"GetRandom", GetRandom, true},                           //
};

TEST(getrandom, sanityTest) {
  uint64_t q;
  size_t i, j, k;
  double montepi, chip, scc, mean, chisq, ent;
  for (k = 0; k < 1; ++k) {
    for (j = 0; j < ARRAYLEN(kRandomFunctions); ++j) {
      rt_init(0);
      for (i = 0; i + 8 <= 100000; i += 8) {
        q = kRandomFunctions[j].f();
        rt_add(&q, 8);
      }
      rt_end(&ent, &chisq, &mean, &montepi, &scc);
      chip = pochisq(chisq, 255);
#if 0
      fprintf(stderr, "\n");
      fprintf(stderr, "/* %-32s */\n", kRandomFunctions[j].s);
      fprintf(stderr, "/* entropy:            %-12g */\n", ent);
      fprintf(stderr, "/* chi-square:         %-12g */\n", chisq);
      fprintf(stderr, "/* chi-square percent: %-12g */\n", chip);
      fprintf(stderr, "/* mean:               %-12g */\n", mean);
      fprintf(stderr, "/* monte-carlo-pi:     %-12g */\n",
              100 * fabs(M_PI - montepi) / M_PI);
      fprintf(stderr, "/* serial-correlation: %-12g */\n", scc);
#endif
      if (kRandomFunctions[j].r) {
        CHECK_GE(chisq, 180, "%s", kRandomFunctions[j].s);
        CHECK_GE(ent * 10, 78, "%s", kRandomFunctions[j].s);
        CHECK_LT(fabs(scc) * 100, 5, "%s", kRandomFunctions[j].s);
        CHECK_LT(fabs(128 - mean), 3, "%s", kRandomFunctions[j].s);
      }
    }
  }
}

TEST(MeasureEntropy, test) {
  MeasureEntropy(kMoby, 1000);
  EXPECT_EQ(00, lroundl(10 * MeasureEntropy("            ", 12)));
  EXPECT_EQ(16, lroundl(10 * MeasureEntropy("abcabcabcabc", 12)));
  EXPECT_EQ(36, lroundl(10 * MeasureEntropy("abcdefghijkl", 12)));
  EXPECT_EQ(49, lroundl(10 * MeasureEntropy(kMoby, 512)));
}

TEST(MeasureEntropy, testElfExecutable) {
  EXPECT_EQ(19, lroundl(10 * MeasureEntropy("\
\x7f\x45\x4c\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x03\x00\x3e\x00\x01\x00\x00\x00\x18\xd7\x00\x00\x00\x00\x00\x00\
\x40\x00\x00\x00\x00\x00\x00\x00\xc8\xd0\x0c\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x40\x00\x38\x00\x09\x00\x40\x00\x16\x00\x15\x00\
\x06\x00\x00\x00\x04\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\
\x40\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\
\xf8\x01\x00\x00\x00\x00\x00\x00\xf8\x01\x00\x00\x00\x00\x00\x00\
\x08\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00\
\x38\x02\x00\x00\x00\x00\x00\x00\x38\x02\x00\x00\x00\x00\x00\x00\
\x38\x02\x00\x00\x00\x00\x00\x00\x19\x00\x00\x00\x00\x00\x00\x00\
\x19\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\
\x01\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x90\xbc\x00\x00\x00\x00\x00\x00\x90\xbc\x00\x00\x00\x00\x00\x00\
\x00\x10\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x05\x00\x00\x00\
\x00\xc0\x00\x00\x00\x00\x00\x00\x00\xc0\x00\x00\x00\x00\x00\x00\
\x00\xc0\x00\x00\x00\x00\x00\x00\x62\xb2\x09\x00\x00\x00\x00\x00\
\x62\xb2\x09\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\
\x01\x00\x00\x00\x04\x00\x00\x00\x00\x80\x0a\x00\x00\x00\x00\x00\
\x00\x80\x0a\x00\x00\x00\x00\x00\x00\x80\x0a\x00\x00\x00\x00\x00\
\xe4\x09\x02\x00\x00\x00\x00\x00\xe4\x09\x02\x00\x00\x00\x00\x00\
\x00\x10\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x06\x00\x00\x00\
\xc0\x95\x0c\x00\x00\x00\x00\x00\xc0\xa5\x0c\x00\x00\x00\x00\x00\
\xc0\xa5\x0c\x00\x00\x00\x00\x00\x64\x3a\x00\x00\x00\x00\x00\x00\
\xa8\x42\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\
\x02\x00\x00\x00\x06\x00\x00\x00\xa0\xc2\x0c\x00\x00\x00\x00\x00\
\xa0\xd2\x0c\x00\x00\x00\x00\x00\xa0\xd2\x0c\x00\x00\x00\x00\x00\
\x80\x01\x00\x00\x00\x00\x00\x00\x80\x01\x00\x00\x00\x00\x00\x00\
\x08\x00\x00\x00\x00\x00\x00\x00\x51\xe5\x74\x64\x06\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00",
                                            512)));
}

TEST(MeasureEntropy, testApeExecutable) {
  EXPECT_EQ(53, lroundl(10 * MeasureEntropy("\
\x7f\x45\x4c\x46\x02\x01\x01\x09\x00\x00\x00\x00\x00\x00\x00\x00\
\x02\x00\x3e\x00\x01\x00\x00\x00\x0a\x11\x40\x00\x00\x00\x00\x00\
\xe0\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x40\x00\x38\x00\x04\x00\x00\x00\x00\x00\x00\x00\
\xb2\x40\xeb\x00\xeb\x14\x90\x90\xeb\x06\x48\x83\xec\x08\x31\xd2\
\xbd\x00\x00\xeb\x05\xe9\xb0\x10\x00\x00\xfc\x0f\x1f\x87\x3e\xe0\
\xbf\x00\x70\x31\xc9\x8e\xc1\xfa\x8e\xd7\x89\xcc\xfb\x0e\x1f\xe8\
\x00\x00\x5e\x81\xee\x72\x00\xb8\x00\x02\x50\x50\x07\x31\xff\xb9\
\x00\x02\xf3\xa4\x0f\x1f\x87\xd2\xff\xea\x8e\x20\x00\x00\x8e\xd9\
\xb9\x00\x1b\xb8\x50\x00\x8e\xc0\x31\xc0\x31\xff\xf3\xaa\x80\xfa\
\x40\x74\x13\xe8\x15\x00\x07\xb0\x01\x31\xc9\x30\xf6\xbf\x40\x03\
\xe8\x48\x00\x4f\x75\xfa\xea\x20\x25\x00\x00\x53\x52\xb4\x08\xcd\
\x13\x72\x2c\x88\xcf\x80\xe7\x3f\x80\xe1\xc0\xd0\xc1\xd0\xc1\x86\
\xcd\x1e\x06\x1f\x31\xf6\x8e\xc6\xbe\x10\x15\x87\xf7\xa5\xa5\xa5\
\xa5\xa5\xa4\x1f\x93\xab\x91\xab\x92\xab\x58\xaa\x92\x5b\xc3\x5a\
\x80\xf2\x80\x31\xc0\xcd\x13\x72\xf7\xeb\xc1\x50\x51\x86\xcd\xd0\
\xc9\xd0\xc9\x08\xc1\x31\xdb\xb0\x01\xb4\x02\xcd\x13\x59\x58\x72\
\x1d\x8c\xc6\x83\xc6\x20\x8e\xc6\xfe\xc0\x3a\x06\x1c\x15\x76\x0d\
\xb0\x01\xfe\xc6\x3a\x36\x20\x15\x76\x03\x30\xf6\x41\xc3\x50\x31\
\xc0\xcd\x13\x58\xeb\xc5\x89\xfe\xac\x84\xc0\x74\x09\xbb\x07\x00\
\xb4\x0e\xcd\x10\xeb\xf2\xc3\x57\xbf\x78\x24\xe8\xe8\xff\x5f\xe8\
\xe4\xff\xbf\x80\x24\xe8\xde\xff\xf3\x90\xeb\xfc\xb9\x04\x00\xbe\
\x00\x04\xad\x85\xc0\x74\x0b\x51\x56\x97\xbe\x9e\x24\xe8\x05\x00\
\x5e\x59\xe2\xee\xc3\x89\xfa\x85\xd2\x74\x14\x52\x56\x31\xc9\xb1\
\x03\x01\xca\xac\x5e\x0c\x80\xee\x5a\xac\xee\x42\x49\x79\xfa\xc3\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x55\xaa",
                                            512)));
}

TEST(MeasureEntropy, testDevUrandom) {
  EXPECT_EQ(76, lroundl(10 * MeasureEntropy("\
\x67\xa6\x8f\x6f\x6f\x01\xa4\x60\xa5\x5a\x6a\xfa\x06\xfd\xbb\xbe\
\xe4\x73\x2f\xc4\x20\x25\xd9\xb2\x78\x7d\x9f\xaf\x5d\x03\x49\x01\
\x90\x94\x8a\xc0\x33\x78\xe0\x65\xd8\x9c\x19\x96\x25\x90\xc8\x6e\
\xf2\x65\xac\x72\xd9\x8e\x58\x05\x26\x22\xa3\x58\x41\x40\xee\x3c\
\x3a\xb0\x08\x7c\x70\x0e\x23\xdc\x52\x10\x21\x5c\xb7\x3d\x3a\xce\
\x7c\xd7\x2a\x8c\x35\x0c\x21\x2b\x03\x75\xbe\x0c\x49\x5b\xdb\x7a\
\x7e\xeb\x27\x03\x56\xca\x9f\xf8\x00\xb4\x4d\x5b\xd6\xde\x41\xd6\
\x86\x7b\x3c\x13\xd4\x2d\xd4\xe9\x79\x05\x48\xcc\xa5\x17\xf8\xb6\
\x74\xdf\x39\x70\x32\x64\x70\x10\xfb\x53\x06\x87\xef\xff\x9f\x4c\
\x07\xee\x09\x54\xb8\xda\x1a\x49\x9b\x12\x3a\xf9\xc3\x55\xbc\xa5\
\xad\x6c\x3d\x1f\x39\x84\xc6\xac\x45\x14\x4f\xa9\xfc\xd6\xfb\xce\
\x41\xf8\x78\x85\xe5\x72\x9c\x0c\xd3\x81\x9b\xa4\x72\xc9\xd5\xc8\
\xc2\x3c\xcc\x36\x58\xf6\x23\x2a\x2e\x9c\x38\xea\x73\x17\xf0\x72\
\x2d\x57\xf8\xc5\x62\x84\xb7\xce\x24\x7b\x46\x65\xc7\xf3\x78\x88\
\x77\x36\x93\x25\x5d\x78\xc9\xfb\x24\xbb\xec\x2f\x77\x4f\x82\xd8\
\x63\x23\xd1\x39\x54\x78\x14\x7d\x24\xc8\x1a\xed\x32\x2d\x7b\xdc\
\xf4\x92\xb1\xaf\x0d\x9b\xba\xb6\x72\xfb\xb2\x7f\x8f\xd5\x4c\x5e\
\x05\xa7\x05\x15\xc5\x51\xd0\x86\x0a\xce\x04\x84\x1e\xba\x44\xf3\
\xbc\x09\xa9\x4e\xc1\xc7\xbd\x7e\x45\x38\x04\xa3\x6c\xfa\x46\x57\
\x40\x93\xbf\xdd\x12\x05\x6c\xb0\xee\x08\x40\x74\xc9\xda\xe7\x30\
\xfa\x1f\x12\xc8\x31\x33\x53\xe4\x65\x2a\xe8\xbf\x2b\x3c\xd6\xcc\
\xec\x8f\x9a\x6f\xe1\xe0\xe6\x81\x0a\xf5\x46\x07\xeb\xcd\xba\xcb\
\xa1\xe5\x4a\x42\xd6\x20\xce\xb6\x16\xcf\x73\x30\x25\x17\xe3\x00\
\x2b\x58\x93\x86\x74\x57\x48\x8b\x2a\x35\x88\xc7\x84\x18\x53\x23\
\xba\xc3\x06\x0a\xd7\x09\xf2\xcb\xe1\xb1\x39\x07\xaf\x2d\xb2\xbc\
\x7d\x71\x91\x6f\x71\x53\x82\xed\x51\x96\xbf\x90\xb4\x4a\x9a\x20\
\x21\x8a\x72\xa3\xbc\xfc\xb9\xcd\x47\x5e\x38\x9c\xd2\xf9\xae\x7f\
\xb2\x1a\x2a\x81\x68\x31\x53\xb2\x11\xfa\x80\x71\x31\xdd\xde\x56\
\x9c\x5f\x3f\x50\xb5\x5f\x99\x5d\x36\x34\x23\x13\xa9\xf0\x04\x3d\
\xa0\xee\x1c\xe5\xe3\x8d\x60\x76\x62\x5a\xd2\xa3\xa2\x51\xea\x75\
\xab\x1f\x2f\xc4\x08\x35\x5d\xf3\x28\x5d\x59\x67\x88\xf0\x61\x6c\
\x8b\x5f\xaf\x90\xa9\x90\xfe\x36\x29\xcc\x02\xf7\x2f\xa7\x19\x0e",
                                            512)));
}

BENCH(getrandom, bench) {
  EZBENCH2("MeasureEntropy", donothing, MeasureEntropy(kMoby, 512));
}
