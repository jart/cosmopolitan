#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/struct/sysinfo.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  int64_t x;
  char ibuf[21];
  struct sysinfo si;
  CHECK_NE(-1, sysinfo(&si));

  printf("%-16s", "uptime");
  x = si.uptime / (24 * 60 * 60);
  si.uptime %= 24 * 60 * 60;
  if (x) {
    printf(" %ld day%s", x, x == 1 ? "" : "s");
  }
  x = si.uptime / (60 * 60);
  si.uptime %= 60 * 60;
  if (x) {
    printf(" %ld hour%s", x, x == 1 ? "" : "s");
  }
  x = si.uptime / (60);
  si.uptime %= 60;
  if (x) {
    printf(" %ld minute%s", x, x == 1 ? "" : "s");
  }
  x = si.uptime;
  if (x) {
    printf(" %ld second%s", x, x == 1 ? "" : "s");
  }
  printf("\n");

  printf("%-16s %g %g %g\n", "load",  //
         1. / 65536 * si.loads[0],    //
         1. / 65536 * si.loads[1],    //
         1. / 65536 * si.loads[2]);   //

  FormatMemorySize(ibuf, si.totalram);
  printf("%-16s %s\n", "totalram", ibuf);

  FormatMemorySize(ibuf, si.freeram);
  printf("%-16s %s\n", "freeram", ibuf);

  FormatMemorySize(ibuf, si.sharedram);
  printf("%-16s %s\n", "sharedram", ibuf);

  FormatMemorySize(ibuf, si.bufferram);
  printf("%-16s %s\n", "bufferram", ibuf);

  FormatMemorySize(ibuf, si.totalswap);
  printf("%-16s %s\n", "totalswap", ibuf);

  FormatMemorySize(ibuf, si.freeswap);
  printf("%-16s %s\n", "freeswap", ibuf);

  printf("%-16s %lu\n", "processes", si.procs);

  FormatMemorySize(ibuf, si.totalhigh);
  printf("%-16s %s\n", "totalhigh", ibuf);

  FormatMemorySize(ibuf, si.freehigh);
  printf("%-16s %s\n", "freehigh", ibuf);

  FormatMemorySize(ibuf, si.mem_unit);
  printf("%-16s %s\n", "mem_unit", ibuf);

  //
}
