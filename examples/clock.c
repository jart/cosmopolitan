#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/struct/timespec.h"
#include "libc/stdio/stdio.h"
#include "libc/time/time.h"

/**
 * @fileoverview clock() function demo
 */

int main(int argc, char *argv[]) {
  unsigned long i;
  volatile unsigned long x;
  struct timespec now, start, next, interval;
  printf("hammering the cpu...\n");
  next = start = _timespec_mono();
  interval = _timespec_frommillis(500);
  next = _timespec_add(next, interval);
  for (;;) {
    for (i = 0;; ++i) {
      x *= 7;
      if (!(i % 256)) {
        now = _timespec_mono();
        if (_timespec_gte(now, next)) {
          break;
        }
      }
    }
    next = _timespec_add(next, interval);
    printf("consumed %10g seconds monotonic time and %10g seconds cpu time\n",
           _timespec_tonanos(_timespec_sub(now, start)) / 1000000000.,
           (double)clock() / CLOCKS_PER_SEC);
  }
}
