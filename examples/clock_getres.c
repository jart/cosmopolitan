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
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"

int n;
int shown[64];

void show(int clock) {
  int i;
  struct timespec ts;
  if (clock == 127) return;
  for (i = 0; i < n; ++i) {
    if (shown[i] == clock) {
      return;
    }
  }
  shown[n++] = clock;
  if (clock_getres(clock, &ts) != -1) {
    kprintf("%s %'ld ns\n", DescribeClockName(clock), timespec_tonanos(ts));
  } else {
    kprintf("%s %s\n", DescribeClockName(clock), _strerrno(errno));
  }
}

int main(int argc, char *argv[]) {
  show(CLOCK_REALTIME);
  show(CLOCK_REALTIME_FAST);
  show(CLOCK_REALTIME_PRECISE);
  show(CLOCK_REALTIME_COARSE);
  show(CLOCK_MONOTONIC);
  show(CLOCK_MONOTONIC_RAW);
  show(CLOCK_MONOTONIC_FAST);
  show(CLOCK_MONOTONIC_PRECISE);
  show(CLOCK_MONOTONIC_COARSE);
  show(CLOCK_PROCESS_CPUTIME_ID);
  show(CLOCK_THREAD_CPUTIME_ID);
  show(CLOCK_PROF);
  show(CLOCK_BOOTTIME);
  show(CLOCK_REALTIME_ALARM);
  show(CLOCK_BOOTTIME_ALARM);
  show(CLOCK_TAI);
  show(CLOCK_UPTIME);
  show(CLOCK_UPTIME_PRECISE);
  show(CLOCK_UPTIME_FAST);
  show(CLOCK_SECOND);
}
