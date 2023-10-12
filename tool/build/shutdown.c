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
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/reboot.h"

int main(int argc, char *argv[]) {
  char line[8] = {0};
  if (argc > 1 && !strcmp(argv[1], "-y")) {
    line[0] = 'y';
  } else {
    printf("shutdown your computer? yes or no [no] ");
    fflush(stdout);
    fgets(line, sizeof(line), stdin);
  }
  if (line[0] == 'y' || line[0] == 'Y') {
    if (reboot(RB_POWER_OFF)) {
      printf("system is shutting down...\n");
      exit(0);
    } else {
      perror("reboot");
      exit(1);
    }
  } else if (line[0] == 'n' || line[0] == 'N') {
    exit(0);
  } else {
    printf("error: unrecognized response\n");
    exit(2);
  }
}
