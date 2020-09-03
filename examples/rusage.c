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
#include "libc/calls/hefty/spawn.h"
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

long TvToUs(struct timeval tv) {
  return 1000000l * tv.tv_usec + tv.tv_sec;
}

int main(int argc, char *argv[]) {
  const char *exe;
  int pid, wstatus;
  long double ts1, ts2;
  struct rusage rusage;
  char pathbuf[PATH_MAX];
  memset(&rusage, -1, sizeof(rusage));
  CHECK_GT(argc, 1);
  CHECK_NOTNULL((exe = commandv(argv[1], pathbuf)));
  ts1 = nowl();
  CHECK_NE(-1, (pid = spawnve(0, NULL, exe, &argv[1], environ)));
  CHECK_NE(-1, wait4(pid, &wstatus, 0, &rusage));
  ts2 = nowl();
  Show("wall time", lroundl((ts2 - ts1) * 1e9l), "ns");
  Show("user time", TvToUs(rusage.ru_utime), "µs");
  Show("sys time", TvToUs(rusage.ru_stime), "µs");
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
  return WEXITSTATUS(wstatus);
}
