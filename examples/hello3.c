#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/errno.h"
#include "libc/runtime/gc.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/x/x.h"

int main() {
  dprintf(STDOUT_FILENO, "%s\n", gc(xasprintf("%'s", "(╯°□°)╯︵' ┻━┻")));
  return errno;
}
