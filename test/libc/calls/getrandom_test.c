/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "ape/sections.internal.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/lcg.internal.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#ifndef __aarch64__
// TODO(jart): Make this test less CPU intensive.

atomic_int done;
atomic_int ready;
pthread_t parent;
atomic_int gotsome;

void OnSig(int sig) {
  ++gotsome;
}

void *TortureWorker(void *arg) {
  sigset_t ss;
  sigfillset(&ss);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &ss, 0));
  ready = true;
  while (!done) {
    if (!IsWindows()) pthread_kill(parent, SIGUSR1);
    usleep(1);
    if (!IsWindows()) pthread_kill(parent, SIGUSR2);
    usleep(1);
  }
  return 0;
}

TEST(getrandom, test) {
  int i, n = 999;
  double e, w = 7.7;
  char *buf = gc(calloc(1, n));
  ASSERT_SYS(0, 0, getrandom(0, 0, 0));
  ASSERT_SYS(0, n, getrandom(buf, n, 0));
  ASSERT_SYS(EFAULT, -1, getrandom(0, n, 0));
  ASSERT_SYS(EINVAL, -1, getrandom(buf, n, -1));
  if ((e = MeasureEntropy(buf, n)) < w) {
    fprintf(stderr, "error: entropy is suspect! got %g but want >=%g\n", e, w);
    for (i = 0; i < n;) {
      if (!(i % 16)) fprintf(stderr, "%6x ", i);
      fprintf(stderr, "%lc", kCp437[buf[i] & 255]);
      if (!(++i % 16)) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    exit(1);
  }
}

TEST(getrandom, test2) {
  pthread_t child;
  double e, w = 7.7;
  struct sigaction sa;
  int i, k, m, n = 999;
  char *buf = gc(calloc(1, n));
  sa.sa_flags = 0;
  sa.sa_handler = OnSig;
  sigemptyset(&sa.sa_mask);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, 0));
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &sa, 0));
  parent = pthread_self();
  ASSERT_EQ(0, pthread_create(&child, 0, TortureWorker, 0));
  while (!ready) pthread_yield();
  for (k = 0; k < 10; ++k) {
    ASSERT_SYS(0, 0, getrandom(0, 0, 0));
    for (i = 0; i < n; i += m) {
      ASSERT_NE(-1, (m = getrandom(buf + i, n - i, 0)));
    }
    ASSERT_SYS(EFAULT, -1, getrandom(0, n, 0));
    ASSERT_SYS(EINVAL, -1, getrandom(buf, n, -1));
    if ((e = MeasureEntropy(buf, n)) < w) {
      fprintf(stderr, "error: entropy suspect! got %g but want >=%g\n", e, w);
      for (i = 0; i < n;) {
        if (!(i % 16)) fprintf(stderr, "%6x ", i);
        fprintf(stderr, "%lc", kCp437[buf[i] & 255]);
        if (!(++i % 16)) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
      done = true;
      pthread_join(child, 0);
      exit(1);
    }
  }
  done = true;
  ASSERT_EQ(0, pthread_join(child, 0));
  if (!IsWindows()) ASSERT_GT(gotsome, 0);
}

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
  if ((i += 8) > _end - __executable_start) i = 8;
  return READ64LE(__executable_start + i);
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
    x |= _rand64() & 255;
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
    {"rand64", _rand64, true},                                //
    {"Rand64LowByte", Rand64LowByte, true},                   //
    {"GetRandom", GetRandom, true},                           //
};

TEST(getrandom, sanityTest) {
  uint64_t q;
  size_t i, j, k;
  double montepi, scc, mean, chisq, ent;
  for (k = 0; k < 1; ++k) {
    for (j = 0; j < ARRAYLEN(kRandomFunctions); ++j) {
      rt_init(0);
      for (i = 0; i + 8 <= 100000; i += 8) {
        q = kRandomFunctions[j].f();
        rt_add(&q, 8);
      }
      rt_end(&ent, &chisq, &mean, &montepi, &scc);
#if 0
      double chip = pochisq(chisq, 255);
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
        CHECK_GT(chisq, 100, "%s", kRandomFunctions[j].s);
        CHECK_LT(chisq, 400, "%s", kRandomFunctions[j].s);
        CHECK_GE(ent * 10, 78, "%s", kRandomFunctions[j].s);
        CHECK_LT(fabs(scc) * 100, 5, "%s", kRandomFunctions[j].s);
        CHECK_LT(fabs(128 - mean), 3, "%s", kRandomFunctions[j].s);
      }
    }
  }
}

TEST(getrandom, badflags_einval) {
  ASSERT_SYS(EINVAL, -1, getrandom(0, 0, -1));
}

#endif /* __aarch64__ */
