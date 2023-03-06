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
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  char buf[256];
  if (!getdomainname(buf, sizeof(buf))) {
    printf("%s\n", buf);
    return 0;
  } else {
    return 1;
  }
}
