#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/assert.h"
#include "libc/calls/struct/timespec.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/clock.h"

/**
 * @fileoverview shows how accurate clock_nanosleep() is
 */

int main(int argc, char *argv[]) {
  long i, ns;
  struct timespec x, y, w;
  timespec_sleep(timespec_fromnanos(0));  // warmup

  printf("\nrelative sleep\n");
  for (i = 0; i < 28; ++i) {
    ns = 1l << i;
    x = timespec_real();
    timespec_sleep(timespec_fromnanos(ns));
    y = timespec_real();
    printf("%,11ld ns sleep took %,ld ns\n", ns,
           timespec_tonanos(timespec_sub(y, x)));
  }

  printf("\nabsolute sleep\n");
  for (i = 0; i < 28; ++i) {
    ns = 1l << i;
    x = timespec_real();
    timespec_sleep_until(timespec_add(x, timespec_fromnanos(ns)));
    y = timespec_real();
    printf("%,11ld ns sleep took %,ld ns\n", ns,
           timespec_tonanos(timespec_sub(y, x)));
  }
}
