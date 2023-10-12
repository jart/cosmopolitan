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
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"

volatile bool gotalrm;

void OnSigAlrm(int sig, siginfo_t *si, void *ctx) {
  gotalrm = true;
}

int main() {

  //////////////////////////////////////////////////////////////////////////////
  printf("first tutorial: set singleshot alarm for 1.5 seconds from now\n");

  // setup alarm in 1.5 seconds
  // this timer tears itself down once it's handled
  struct itimerval shot = {{0, 0}, {1, 500000}};
  struct sigaction handler = {.sa_sigaction = OnSigAlrm,
                              .sa_flags = SA_RESETHAND | SA_SIGINFO};
  unassert(!sigaction(SIGALRM, &handler, 0));
  unassert(!setitimer(ITIMER_REAL, &shot, 0));

  // wait for alarm
  pause();
  printf("got singleshot alarm!\n\n");

  //////////////////////////////////////////////////////////////////////////////
  printf("second tutorial: interval timer\n");

  // setup timer every 1.5 seconds
  struct sigaction oldalrm;
  struct sigaction sigalrm = {.sa_sigaction = OnSigAlrm,
                              .sa_flags = SA_SIGINFO};
  unassert(!sigaction(SIGALRM, &sigalrm, &oldalrm));
  struct itimerval oldtimer;
  struct itimerval timer = {{1, 500000}, {1, 500000}};
  unassert(!setitimer(ITIMER_REAL, &timer, &oldtimer));

  // wait for three timeouts
  int i = 0;
  int n = 3;
  while (i < n) {
    pause();
    if (gotalrm) {
      ++i;
      printf("got timeout %d out of %d\n", i, n);
      gotalrm = false;
    }
  }

  // teardown timer
  unassert(!setitimer(ITIMER_REAL, &oldtimer, 0));
  unassert(!sigaction(SIGALRM, &oldalrm, 0));
}
