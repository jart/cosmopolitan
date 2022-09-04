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
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  char name[254];
  CHECK_NE(-1, gethostname(name, sizeof(name)));
  printf("gethostname() → %`'s\n", name);
  CHECK_NE(-1, getdomainname(name, sizeof(name)));
  printf("getdomainname() → %`'s\n", name);
  return 0;
}
