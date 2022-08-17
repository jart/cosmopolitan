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
#include "libc/fmt/conv.h"
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

  sizefmt(ibuf, si.totalram, 1024);
  printf("%-16s %s\n", "totalram", ibuf);

  sizefmt(ibuf, si.freeram, 1024);
  printf("%-16s %s\n", "freeram", ibuf);

  sizefmt(ibuf, si.sharedram, 1024);
  printf("%-16s %s\n", "sharedram", ibuf);

  sizefmt(ibuf, si.bufferram, 1024);
  printf("%-16s %s\n", "bufferram", ibuf);

  sizefmt(ibuf, si.totalswap, 1024);
  printf("%-16s %s\n", "totalswap", ibuf);

  sizefmt(ibuf, si.freeswap, 1024);
  printf("%-16s %s\n", "freeswap", ibuf);

  printf("%-16s %lu\n", "processes", si.procs);

  sizefmt(ibuf, si.totalhigh, 1024);
  printf("%-16s %s\n", "totalhigh", ibuf);

  sizefmt(ibuf, si.freehigh, 1024);
  printf("%-16s %s\n", "freehigh", ibuf);

  sizefmt(ibuf, si.mem_unit, 1024);
  printf("%-16s %s\n", "mem_unit", ibuf);

  //
}
