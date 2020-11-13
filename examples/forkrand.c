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
#include "libc/log/log.h"
#include "libc/nt/nt/process.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/time/time.h"

noinline void dostuff(void) {
  int i, us;
  srand(rand64()); /* seeds rand() w/ intel rdrnd, auxv, etc. */
  for (i = 0; i < 5; ++i) {
    us = rand() % 500000;
    usleep(us);
    printf("%s%u%s%u [%s=%d]\n", "hello no. ", i, " from ", getpid(), "us", us);
    fflush(stdout);
  }
}

int main(int argc, char *argv[]) {
  int rc, child, wstatus;
  CHECK_NE(-1, (child = fork()));
  if (!child) {
    /* child process */
    dostuff();
    return 0;
  } else {
    /* parent process */
    dostuff();
    /* note: abandoned children become zombies */
    CHECK_NE(-1, (rc = wait(&wstatus)));
    return WEXITSTATUS(wstatus);
  }
}
