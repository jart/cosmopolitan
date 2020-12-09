#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/alg/alg.h"
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/x/x.h"

const char kProgram[] = "o/default/examples/hello.com";

int main(int argc, char *argv[]) {
  /**
   * Runs make if hello.com doesn't exist.
   *
   * 1. gc() automates calling free() on return.
   * 2. xasprintf("foo %s", "bar") is our version of "foo %s" % ("bar")
   * 3. Demonstrates correct escaping for bourne shell
   */
  if (!fileexists(kProgram)) {
    system(gc(xasprintf("%s '%s'", "make -j4",
                        gc(replacestr(kProgram, "'", "'\"'\"'")))));
  }

  /**
   * Our version of subprocess.Popen
   * 1. Doesn't require fork() so pain-free on NT
   * 2. Google checks are like assert() but better
   */
  ssize_t transferred;
  int child, wstatus, procfds[3] = {STDIN_FILENO, -1, STDERR_FILENO};
  CHECK_NE(-1,
           (child = spawnve(0, procfds, /* run w/o shell */ kProgram,
                            (char *const[]){/* argv[0]   */ basename(kProgram),
                                            /* argv[1]   */ "boop",
                                            /* sentinel  */ NULL},
                            environ)));
  printf("%s %s: ", kProgram, "says");
  fflush(stdout);
  for (;;) {
    transferred = copyfd(procfds[1], NULL, fileno(stdout), NULL, INT_MAX, 0);
    CHECK_NE(-1, transferred);
    if (!transferred) break;
  }
  CHECK_NE(-1, waitpid(child, &wstatus, 0));
  CHECK_EQ(0, WEXITSTATUS(wstatus));

  return 0;
}
