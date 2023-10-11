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
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/clock.h"

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

  sizefmt(ibuf, si.totalram * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "totalram", si.totalram, ibuf);

  sizefmt(ibuf, si.freeram * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "freeram", si.freeram, ibuf);

  sizefmt(ibuf, si.sharedram * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "sharedram", si.sharedram, ibuf);

  sizefmt(ibuf, si.bufferram * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "bufferram", si.bufferram, ibuf);

  sizefmt(ibuf, si.totalswap * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "totalswap", si.totalswap, ibuf);

  sizefmt(ibuf, si.freeswap * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "freeswap", si.freeswap, ibuf);

  printf("%-16s %lu\n", "processes", si.procs);

  sizefmt(ibuf, si.totalhigh * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "totalhigh", si.totalhigh, ibuf);

  sizefmt(ibuf, si.freehigh * si.mem_unit, 1024);
  printf("%-16s %ld (%s)\n", "freehigh", si.freehigh, ibuf);

  sizefmt(ibuf, si.mem_unit, 1024);
  printf("%-16s %s\n", "mem_unit", ibuf);

  //
}
