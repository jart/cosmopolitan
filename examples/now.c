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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"
#include "third_party/dtoa/dtoa.h"

char dtoabuf_[3][32];

static long double avg;

int main(int argc, char *argv[]) {
  long double t2, t1 = nowl();
  dsleep(0.3);
  for (;;) {
    t2 = nowl();
    printf("%s %s avg=%s\n", g_fmt(dtoabuf_[0], t2),
           g_fmt(dtoabuf_[1], t2 - t1), g_fmt(dtoabuf_[2], avg));
    t1 = t2;
    dsleep(0.3);
  }
  return 0;
}
