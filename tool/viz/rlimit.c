#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/color.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"

/**
 * @fileoverview tool for printing and changing system resource limits
 *
 * This is what you do if you want to not accidentally bomb your system
 * with runaway code. If you haven't accidentally bombed your UNIX
 * system before then you're not pushing it hard enough.
 */

static void SetLimit(int resource, uint64_t soft, uint64_t hard) {
  struct rlimit old;
  struct rlimit lim = {soft, hard};
  if (resource == 127) return;
  if (setrlimit(resource, &lim) == -1) {
    if (!getrlimit(resource, &old)) {
      lim.rlim_max = MIN(hard, old.rlim_max);
      lim.rlim_cur = MIN(soft, old.rlim_max);
      if (!setrlimit(resource, &lim)) {
        fprintf(stderr, "%sNOTE: SETRLIMIT(%s) DOWNGRADED TO {%,ld, %,ld}\n",
                DescribeRlimitName(resource), lim.rlim_cur, lim.rlim_max);
        return;
      }
    }
    fprintf(stderr, "ERROR: SETRLIMIT(%s, %,ld, %,ld) FAILED %m%n",
            DescribeRlimitName(resource), soft, hard);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int i, rc;
  char rlnbuf[20];
  struct rlimit rlim;

  // // example of how you might change the limits
  // SetLimit(RLIMIT_CPU, 3, 33);
  // SetLimit(RLIMIT_NPROC, 4, 128);
  // SetLimit(RLIMIT_NOFILE, 32, 128);
  // SetLimit(RLIMIT_SIGPENDING, 16, 1024);
  // SetLimit(RLIMIT_AS, 8 * 1024 * 1024, 1l * 1024 * 1024 * 1024);
  // SetLimit(RLIMIT_RSS, 8 * 1024 * 1024, 1l * 1024 * 1024 * 1024);
  // SetLimit(RLIMIT_DATA, 8 * 1024 * 1024, 1l * 1024 * 1024 * 1024);
  // SetLimit(RLIMIT_FSIZE, 8 * 1000 * 1000, 1l * 1000 * 1000 * 1000);

  for (i = 0; i < RLIM_NLIMITS; ++i) {
    rc = getrlimit(i, &rlim);
    printf("SETRLIMIT(%-20s, %,16ld, %,16ld) → %d %s\n",
           (DescribeRlimitName)(rlnbuf, i), rlim.rlim_cur, rlim.rlim_max, rc,
           !rc ? "" : strerror(errno));
  }

  return 0;
}
