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
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/sysparam.h"

int main() {
  int nproc;
  char ibuf[12];
  nproc = __get_cpu_count();
  nproc = MAX(1, nproc);
  FormatInt32(ibuf, nproc);
  tinyprint(1, ibuf, "\n", NULL);
}
