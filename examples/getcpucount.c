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
#include "libc/fmt/itoa.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/stdio.h"

int main(int argc, char *argv[]) {
  int c, n;
  char a[22], *p;
  if ((c = GetCpuCount())) {
    p = FormatInt64(a, c);
    *p++ = '\n';
    return write(1, a, p - a) == p - a ? 0 : 1;
  } else {
    return 1;
  }
}
