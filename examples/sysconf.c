#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/runtime/sysconf.h"
#include "libc/stdio/stdio.h"

#define SYSCONF(NAME) printf("%-24s %,ld\n", #NAME, sysconf(NAME))

int main(int argc, char *argv[]) {
  SYSCONF(_SC_CLK_TCK);
  SYSCONF(_SC_PAGESIZE);
  SYSCONF(_SC_ARG_MAX);
  SYSCONF(_SC_CHILD_MAX);
  SYSCONF(_SC_OPEN_MAX);
  SYSCONF(_SC_NPROCESSORS_CONF);
  SYSCONF(_SC_NPROCESSORS_ONLN);
  SYSCONF(_SC_PHYS_PAGES);
  SYSCONF(_SC_AVPHYS_PAGES);
}
