#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/log/log.h"
#include "libc/nt/nt/process.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/calls/calls.h"
#include "libc/time/time.h"

noinline void dostuff(void) {
  srand(rand64()); /* seeds rand() w/ intel rdrnd, auxv, etc. */
  for (unsigned i = 0; i < 5; ++i) {
    int us = rand() % 500000;
    usleep(us);
    printf("%s%u%s%u [%s=%d]\n", "hello no. ", i, " from ", getpid(), "us", us);
    fflush(stdout);
  }
}

int main(int argc, char *argv[]) {
  fprintf(stderr, "%p\n", RtlCloneUserProcess);
  int child;
  if ((child = fork()) == -1) perror("fork"), exit(1);
  if (!child) {
    /* child process */
    dostuff();
    return 0;
  } else {
    /* parent process */
    dostuff();
    /* note: abandoned children become zombies */
    int rc, wstatus;
    if ((rc = wait(&wstatus)) == -1) perror("wait"), exit(1);
    return WEXITSTATUS(wstatus);
  }
}
