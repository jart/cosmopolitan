#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "third_party/linenoise/linenoise.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/mem.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

int main(int argc, char *argv[]) {
  int ws;
  char *line;
  char ps1[100];
  sigset_t mask, om;
  posix_spawnattr_t attr;
  ShowCrashReports();
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &mask, &om);
  posix_spawnattr_init(&attr);
  posix_spawnattr_setsigmask(&attr, &om);
  for (ws = 0;;) {
    if (WIFSIGNALED(ws)) {
      ksnprintf(ps1, sizeof(ps1), "\e[1;31m%G\e[0m :> ", ws, WTERMSIG(ws));
    } else {
      ksnprintf(ps1, sizeof(ps1), "%d :> ", ws, WEXITSTATUS(ws));
    }
    if (!(line = linenoiseWithHistory(ps1, "unbourne"))) {
      break;
    }
    if (*line) {
      int i = 0;
      char *args[64];
      args[i++] = strtok(line, " \t\v\r\n");
      while ((args[i++] = strtok(0, " \t\v\r\n"))) {
      }
      int pid;
      posix_spawnp(&pid, args[0], 0, &attr, args, environ);
      wait(&ws);
    }
    free(line);
  }
  posix_spawnattr_destroy(&attr);
  return 0;
}
