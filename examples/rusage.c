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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"

#define PREFIX "\e[1mRL\e[0m: "

static void PrintResourceReport(struct rusage *ru) {
  long utime, stime;
  long double ticks;
  if (ru->ru_maxrss) {
    fprintf(stderr, "%sballooned to %,ldkb in size\n", PREFIX, ru->ru_maxrss);
  }
  if ((utime = ru->ru_utime.tv_sec * 1000000 + ru->ru_utime.tv_usec) |
      (stime = ru->ru_stime.tv_sec * 1000000 + ru->ru_stime.tv_usec)) {
    ticks = ceill((long double)(utime + stime) / (1000000.L / CLK_TCK));
    fprintf(stderr, "%sneeded %,ldµs cpu (%d%% kernel)\n", PREFIX,
            utime + stime, (int)((long double)stime / (utime + stime) * 100));
    if (ru->ru_idrss) {
      fprintf(stderr, "%sneeded %,ldkb memory on average\n", PREFIX,
              lroundl(ru->ru_idrss / ticks));
    }
    if (ru->ru_isrss) {
      fprintf(stderr, "%sneeded %,ldkb stack on average\n", PREFIX,
              lroundl(ru->ru_isrss / ticks));
    }
    if (ru->ru_ixrss) {
      fprintf(stderr, "%smapped %,ldkb shared on average\n", PREFIX,
              lroundl(ru->ru_ixrss / ticks));
    }
  }
  if (ru->ru_minflt || ru->ru_majflt) {
    fprintf(stderr, "%scaused %,ld page faults (%d%% memcpy)\n", PREFIX,
            ru->ru_minflt + ru->ru_majflt,
            (int)((long double)ru->ru_minflt / (ru->ru_minflt + ru->ru_majflt) *
                  100));
  }
  if (ru->ru_nvcsw + ru->ru_nivcsw > 1) {
    fprintf(stderr, "%s%,ld context switches (%d%% consensual)\n", PREFIX,
            ru->ru_nvcsw + ru->ru_nivcsw,
            (int)((long double)ru->ru_nvcsw / (ru->ru_nvcsw + ru->ru_nivcsw) *
                  100));
  }
  if (ru->ru_msgrcv || ru->ru_msgsnd) {
    fprintf(stderr, "%sreceived %,ld message%s and sent %,ld\n", PREFIX,
            ru->ru_msgrcv, ru->ru_msgrcv == 1 ? "" : "s", ru->ru_msgsnd);
  }
  if (ru->ru_inblock || ru->ru_oublock) {
    fprintf(stderr, "%sperformed %,ld read%s and %,ld write i/o operations\n",
            PREFIX, ru->ru_inblock, ru->ru_inblock == 1 ? "" : "s",
            ru->ru_oublock);
  }
  if (ru->ru_nsignals) {
    fprintf(stderr, "%sreceived %,ld signals\n", PREFIX, ru->ru_nsignals);
  }
  if (ru->ru_nswap) {
    fprintf(stderr, "%sgot swapped %,ld times\n", PREFIX, ru->ru_nswap);
  }
}

struct rusage rusage;

int main(int argc, char *argv[]) {
  int pid, wstatus;
  long double ts1, ts2;
  sigset_t chldmask, savemask;
  struct sigaction dflt, ignore, saveint, savequit;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s PROG [ARGS...]\n", argv[0]);
    return 1;
  }
  dflt.sa_flags = 0;
  dflt.sa_handler = SIG_DFL;
  sigemptyset(&dflt.sa_mask);
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  ts1 = nowl();
  CHECK_NE(-1, (pid = fork()));
  if (!pid) {
    sigaction(SIGINT, &dflt, 0);
    sigaction(SIGQUIT, &dflt, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execvp(argv[1], argv + 1);
    fprintf(stderr, "exec failed %d\n", errno);
    _Exit(127);
  }
  while (wait4(pid, &wstatus, 0, &rusage) == -1) {
    CHECK_EQ(EINTR, errno);
  }
  ts2 = nowl();
  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigprocmask(SIG_SETMASK, &savemask, 0);
  fprintf(stderr, "%stook %,ldµs wall time\n", PREFIX,
          (int64_t)((ts2 - ts1) * 1e6));
  PrintResourceReport(&rusage);
  if (WIFEXITED(wstatus)) {
    return WEXITSTATUS(wstatus);
  } else {
    return 128 + WTERMSIG(wstatus);
  }
}
