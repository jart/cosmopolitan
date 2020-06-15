#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/bits.h"
#include "libc/macros.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/poll.h"

int main(int argc, char *argv[]) {
  int rc;
  int pollms = 1;
  struct pollfd fds[] = {{-1, 0}, {STDIN_FILENO, POLLIN}};
  if ((rc = poll(fds, ARRAYLEN(fds), pollms)) != -1) {
    printf("toto=%d\n", rc);
    return 0;
  }
  return 0;
}
