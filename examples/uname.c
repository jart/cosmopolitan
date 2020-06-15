#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"

int main(int argc, char *argv[]) {
  unsigned need = 0;
  int opt;
  while ((opt = getopt(argc, argv, "hasnrvmd")) != -1) {
    switch (opt) {
      case 'a':
        need |= 0b111111;
        break;
      case 's':
        need |= 0b000001;
        break;
      case 'n':
        need |= 0b000010;
        break;
      case 'r':
        need |= 0b000100;
        break;
      case 'v':
        need |= 0b001000;
        break;
      case 'm':
        need |= 0b010000;
        break;
      case 'd':
        need |= 0b100000;
        break;
      default: /* -? or -h */
        fprintf(stderr, "%s: %s [%s]\n%s", "Usage", argv[0], "FLAGS",
                "  -a\tprint all\n"
                "  -s\tprint sysname\n"
                "  -n\tprint nodename\n"
                "  -r\tprint release\n"
                "  -v\tprint version\n"
                "  -m\tprint machine\n"
                "  -d\tprint domainname\n");
        exit(EXIT_FAILURE);
    }
  }
  if (!need) need = 0b000001;
  struct utsname data;
  if (uname(&data) == -1) perror("uname"), exit(EXIT_FAILURE);
  if (need & 0b000001) printf("%s ", data.sysname);
  if (need & 0b000010) printf("%s ", data.nodename);
  if (need & 0b000100) printf("%s ", data.release);
  if (need & 0b001000) printf("%s ", data.version);
  if (need & 0b010000) printf("%s ", data.machine);
  if (need & 0b100000) printf("%s ", data.domainname);
  printf("\n");
  return 0;
}
