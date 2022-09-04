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
#include "libc/calls/struct/utsname.h"
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  struct utsname names;
  if (uname(&names)) return 1;
  printf("%-10s %`'s\n", "sysname", names.sysname);
  printf("%-10s %`'s\n", "release", names.release);
  printf("%-10s %`'s\n", "version", names.version);
  printf("%-10s %`'s\n", "machine", names.machine);
  printf("%-10s %`'s\n", "nodename", names.nodename);
  printf("%-10s %`'s\n", "domainname", names.domainname);
  return 0;
}
