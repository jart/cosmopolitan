/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"
#include "libc/x/xgetline.h"

static int pid;

static void RelaySig(int sig, struct siginfo *si, void *uc) {
  kill(pid, sig);
}

int main(int argc, char *argv[]) {
  FILE *f;
  bool ok;
  char *s, *p;
  int64_t micros;
  long double t1, t2;
  int ws, pipefds[2];
  setvbuf(stdout, malloc(BUFSIZ), _IOLBF, BUFSIZ);
  setvbuf(stderr, malloc(BUFSIZ), _IOLBF, BUFSIZ);
  if (argc < 2) {
    f = stdin;
    t1 = nowl();
  } else {
    sigset_t block, mask;
    struct sigaction saignore = {.sa_handler = SIG_IGN};
    struct sigaction sadefault = {.sa_handler = SIG_DFL};
    struct sigaction sarelay = {.sa_sigaction = RelaySig,
                                .sa_flags = SA_RESTART};
    sigemptyset(&block);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &block, &mask);
    sigaction(SIGHUP, &sarelay, 0);
    sigaction(SIGTERM, &sarelay, 0);
    sigaction(SIGINT, &saignore, 0);
    sigaction(SIGQUIT, &saignore, 0);
    CHECK_NE(-1, pipe(pipefds));
    CHECK_NE(-1, (pid = vfork()));
    if (!pid) {
      close(pipefds[0]);
      dup2(pipefds[1], 1);
      sigaction(SIGHUP, &sadefault, NULL);
      sigaction(SIGTERM, &sadefault, NULL);
      sigaction(SIGINT, &sadefault, NULL);
      sigaction(SIGQUIT, &sadefault, NULL);
      sigprocmask(SIG_SETMASK, &mask, NULL);
      execvp(argv[1], argv + 1);
      _exit(127);
    }
    t1 = nowl();
    close(pipefds[1]);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    CHECK((f = fdopen(pipefds[0], "r")));
    CHECK_NE(-1, setvbuf(f, malloc(4096), _IOLBF, 4096));
  }
  if ((p = xgetline(f))) {
    t2 = nowl();
    micros = (t2 - t1) * 1e6;
    t1 = t2;
    printf("%16ld\n", micros);
    do {
      s = xgetline(f);
      t2 = nowl();
      micros = (t2 - t1) * 1e6;
      t1 = t2;
      printf("%16ld %s", micros, p);
      free(p);
    } while ((p = s));
  }
  ok = !ferror(f);
  if (argc < 2) return ok ? 0 : 1;
  fclose(f);
  while (waitpid(pid, &ws, 0) == -1) CHECK_EQ(EINTR, errno);
  return !WIFEXITED(ws) ? 128 + WTERMSIG(ws) : ok ? WEXITSTATUS(ws) : 1;
}
