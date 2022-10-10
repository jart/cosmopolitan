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
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"

dontinline void dostuff(const char *s) {
  int i, us;
  srand(_rand64()); /* seeds rand() w/ intel rdrnd, auxv, etc. */
  for (i = 0; i < 5; ++i) {
    us = rand() % 500000;
    usleep(us);
    printf("hello no. %u from %s %u [us=%d]\n", i, s, getpid(), us);
    fflush(stdout);
  }
}

int main(int argc, char *argv[]) {
  int rc, child, wstatus;
  CHECK_NE(-1, (child = fork()));
  if (!child) {
    /* child process */
    dostuff("child");
    return 0;
  } else {
    /* parent process */
    dostuff("parent");
    /* note: abandoned children become zombies */
    CHECK_NE(-1, (rc = wait(&wstatus)));
    CHECK_EQ(0, WEXITSTATUS(wstatus));
    return 0;
  }
}
