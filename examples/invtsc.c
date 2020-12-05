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
#include "libc/dce.h"
#include "libc/log/check.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"

/**
 * @fileoverview Measure CPU clock mystery constants.
 * @note CPUID EAX=15h/16h is a joke
 */

const long double kInterval = 0.001L;

long double start_;
char dtoabuf_[3][32];
volatile bool isdone_;

void OnCtrlC(int sig) {
  isdone_ = true;
}

long double now(void) {
  return dtime(CLOCK_MONOTONIC);
}

long double GetSample(void) {
  uint64_t tick1, tick2;
  long double time1, time2;
  time1 = now();
  tick1 = rdtsc();
  nanosleep(&(struct timespec){0, 100000}, NULL);
  time2 = now();
  tick2 = rdtsc();
  return ((time2 - time1) * 1e9L) / (long double)(tick2 - tick1);
}

void MeasureNanosecondsPerAlwaysRunningTimerCycle(void) {
  int i;
  long double avg, samp, elapsed;
  start_ = now();
  for (i = 1, avg = 1.0L; !isdone_; ++i) {
    samp = GetSample();
    avg += (samp - avg) / i;
    dsleep(kInterval);
    elapsed = now() - start_;
    g_xfmt_p(dtoabuf_[0], &avg, 15, 32, 0);
    g_xfmt_p(dtoabuf_[1], &samp, 15, 32, 0);
    g_xfmt_p(dtoabuf_[2], &elapsed, 15, 32, 0);
    printf("1c = %sns (last=%sns spent=%ss)\n", dtoabuf_[0], dtoabuf_[1],
           dtoabuf_[2]);
  }
}

int main(int argc, char *argv[]) {
  if (!X86_HAVE(INVTSC)) {
    fprintf(stderr, "warning: html can reprogram your microcode\n");
  }
  if (X86_HAVE(HYPERVISOR)) {
    fprintf(stderr, "warning: virtual machine rdtsc isn't great\n");
  }
  CHECK_NE(-1, xsigaction(SIGINT, OnCtrlC, 0, 0, NULL));
  MeasureNanosecondsPerAlwaysRunningTimerCycle();
  return 0;
}
