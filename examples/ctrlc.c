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
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"

volatile bool gotctrlc;

void GotCtrlC(int sig) {
  gotctrlc = true;
}

int main(int argc, char *argv[]) {
  ssize_t rc;
  size_t got, wrote;
  unsigned char buf[512];
  struct sigaction saint = {.sa_handler = GotCtrlC};
  fprintf(stderr, "This echos stdio until Ctrl+C is pressed.\n");
  CHECK_NE(-1, sigaction(SIGINT, &saint, NULL));
  for (;;) {
    rc = read(0, buf, 512);
    if (rc != -1) {
      got = rc;
    } else {
      CHECK_EQ(EINTR, errno);
      break;
    }
    if (!got) break;
    rc = write(1, buf, got);
    if (rc != -1) {
      wrote = rc;
    } else {
      CHECK_EQ(EINTR, errno);
      break;
    }
    CHECK_EQ(got, wrote);
  }
  if (gotctrlc) {
    fprintf(stderr, "Got Ctrl+C\n");
  } else {
    fprintf(stderr, "Got EOF\n");
  }
  return 0;
}
