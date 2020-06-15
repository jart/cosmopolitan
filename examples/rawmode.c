#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "dsp/tty/tty.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/time/time.h"

int main(int argc, char *argv[]) {
  setenv("GDB", "nopenopenope", true);
  showcrashreports();
  ttyraw(kTtyLfToCrLf | kTtySigs);
  printf("hi\n"), sleep(1);
  if (argc > 1 && strstr(argv[1], "crash")) abort();
  printf("there\n"), sleep(1);
  return 0;
}
