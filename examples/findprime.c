#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"

static const struct itimerval kTimer = {{0, 10000}, {0, 10000}}; /* 10ms */
static volatile bool32 showprogress_;
static const char *magic_;

void OnInterrupt(int sig) {
  showprogress_ = true;
}

void ShowProgress(uintmax_t i, uintmax_t j, uintmax_t n) {
  fprintf(stderr, "%s%,40jd%,40jd%,40jd\r\n", magic_, i, j, n);
  showprogress_ = false;
}

static bool IsPrime(uintmax_t i) {
  uintmax_t j, n;
  for (j = 3, n = (unsigned long)floorl(sqrtl(i)); j <= n; j += 2) {
    if (showprogress_) ShowProgress(i, j, n);
    if (i % j == 0) return false;
  }
  return true;
}

static void TryNumber(uintmax_t x, uintmax_t i) {
  if (IsPrime(i)) {
    printf("%s%,jd%s%,jd\n", "the prime nearest ", x, " is ", i);
    exit(0);
  }
}

int main(int argc, char **argv) {
  uintmax_t x, i, j;
  CHECK_EQ(2, argc);
  signal(SIGALRM, OnInterrupt);
  setitimer(ITIMER_REAL, &kTimer, NULL);
  magic_ = cancolor() ? "\e[F\e[K" : "";
  if ((x = strtoumax(argv[1], NULL, 0)) % 2 == 0) ++x;
  TryNumber(x, x);
  for (i = x, j = x;;) {
    if (i < UINTMAX_MAX) TryNumber(x, i), i += 2;
    if (j > 1) TryNumber(x, j), j -= 2;
  }
}
