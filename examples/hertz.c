#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/xsigaction.h"

const struct itimerval kHertz = {
    {1, 0},
    {0, 1},
};

bool hertz;

void OnAlrm(void) {
  hertz = true;
}

int main(int argc, char *argv[]) {
  CHECK_NE(-1, xsigaction(SIGALRM, OnAlrm, 0, 0, 0));
  CHECK_NE(-1, setitimer(ITIMER_REAL, &kHertz, NULL));
  for (;;) {
    CHECK_EQ(-1, pause());
    CHECK_EQ(EINTR, errno);
    if (hertz) {
      hertz = false;
      printf("ding\n");
    }
  }
  return 0;
}
