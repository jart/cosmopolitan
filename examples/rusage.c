#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/struct/rusage.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/itoa.h"
#include "libc/log/appendresourcereport.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"

int main(int argc, char *argv[]) {

  const char *prog = argv[0];
  if (!prog) prog = "rusage";

  if (argc < 2) {
    tinyprint(2, prog, ": missing command\n", NULL);
    exit(1);
  }

  // block process management signals
  sigset_t mask, orig;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &mask, &orig);

  struct timespec started = timespec_real();

  // launch subprocess
  int child = fork();
  if (child == -1) {
    perror(prog);
    exit(1);
  }
  if (!child) {
    sigprocmask(SIG_SETMASK, &orig, 0);
    execvp(argv[1], argv + 1);
    _Exit(127);
  }

  // wait for subprocess
  int ws;
  struct rusage ru;
  struct sigaction ignore;
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, 0);
  sigaction(SIGQUIT, &ignore, 0);
  if (wait4(child, &ws, 0, &ru) == -1) {
    perror(prog);
    exit(1);
  }

  // compute wall time
  char strmicros[27];
  struct timespec ended = timespec_real();
  struct timespec elapsed = timespec_sub(ended, started);
  FormatInt64Thousands(strmicros, timespec_tomicros(elapsed));

  // show report
  char *b = 0;
  appends(&b, "took ");
  appends(&b, strmicros);
  appends(&b, "µs wall time\n");
  AppendResourceReport(&b, &ru, "\n");
  write(2, b, appendz(b).i);

  // propagate status
  if (WIFSIGNALED(ws)) {
    signal(WTERMSIG(ws), SIG_DFL);
    raise(WTERMSIG(ws));
  }
  return WEXITSTATUS(ws);
}
