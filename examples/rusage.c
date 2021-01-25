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
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/time/time.h"

void Show(const char *name, long measurement, const char *unit) {
  fprintf(stderr, "%-*s%,*d %s\n", 32, name, 32, measurement, unit);
}

long TvToNs(struct timeval tv) {
  return tv.tv_sec * 1000000000 + tv.tv_usec * 1000;
}

int main(int argc, char *argv[]) {
  int pid, wstatus;
  long double ts1, ts2;
  struct rusage rusage;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s PROG [ARGS...]\n", argv[0]);
    return 1;
  }
  memset(&rusage, -1, sizeof(rusage));
  CHECK_GT(argc, 1);
  ts1 = nowl();
  if (!(pid = vfork())) {
    execvp(argv[1], argv + 1);
    abort();
  }
  CHECK_NE(-1, wait4(pid, &wstatus, 0, &rusage));
  ts2 = nowl();
  Show("wall time", lroundl((ts2 - ts1) * 1e9l), "ns");
  Show("user time", TvToNs(rusage.ru_utime), "ns");
  Show("sys time", TvToNs(rusage.ru_stime), "ns");
  Show("maximum resident set size", rusage.ru_maxrss, "");
  Show("integral shared memory size", rusage.ru_ixrss, "");
  Show("integral unshared data size", rusage.ru_idrss, "");
  Show("integral unshared stack size", rusage.ru_isrss, "");
  Show("minor page faults", rusage.ru_minflt, "");
  Show("major page faults", rusage.ru_majflt, "");
  Show("swaps", rusage.ru_nswap, "");
  Show("block input ops", rusage.ru_inblock, "");
  Show("block output ops", rusage.ru_oublock, "");
  Show("ipc messages sent", rusage.ru_msgsnd, "");
  Show("ipc messages received", rusage.ru_msgrcv, "");
  Show("signals received", rusage.ru_nsignals, "");
  Show("voluntary context switches", rusage.ru_nvcsw, "");
  Show("involuntary context switches", rusage.ru_nivcsw, "");
  if (WIFEXITED(wstatus)) {
    return WEXITSTATUS(wstatus);
  } else {
    return 128 + WTERMSIG(wstatus);
  }
}
