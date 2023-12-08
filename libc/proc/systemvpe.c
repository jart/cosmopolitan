/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"

/**
 * Executes program and waits for it to complete, e.g.
 *
 *     systemvpe("ls", (char *[]){"ls", dir, 0}, environ);
 *
 * This function is designed to do the same thing as system() except
 * rather than taking a shell script argument it accepts an array of
 * strings which are safely passed directly to execve().
 *
 * This function is 5x faster than system() and generally safer, for
 * most command running use cases that don't need to control the i/o
 * file descriptors.
 *
 * @param prog is path searched (if it doesn't contain a slash) from
 *     the $PATH environment variable in `environ` (not your `envp`)
 * @return -1 if child process couldn't be created, otherwise a wait
 *     status that can be accessed using macros like WEXITSTATUS(s),
 *     WIFSIGNALED(s), WTERMSIG(s), etc.
 * @see system()
 */
int systemvpe(const char *prog, char *const argv[], char *const envp[]) {
  char *exe;
  int pid, wstatus;
  char pathbuf[PATH_MAX + 1];
  sigset_t chldmask, savemask;
  if (!(exe = commandv(prog, pathbuf, sizeof(pathbuf)))) {
    return -1;
  }
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGINT);
  sigaddset(&chldmask, SIGQUIT);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  if (!(pid = vfork())) {
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execve(prog, argv, envp);
    _Exit(127);
  } else if (pid == -1) {
    wstatus = -1;
  } else {
    struct sigaction ignore, saveint, savequit;
    ignore.sa_flags = 0;
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    sigaction(SIGINT, &ignore, &saveint);
    sigaction(SIGQUIT, &ignore, &savequit);
    BLOCK_CANCELATION;
    while (wait4(pid, &wstatus, 0, 0) == -1) {
      if (errno != EINTR) {
        wstatus = -1;
        break;
      }
    }
    ALLOW_CANCELATION;
    sigaction(SIGQUIT, &savequit, 0);
    sigaction(SIGINT, &saveint, 0);
  }
  sigprocmask(SIG_SETMASK, &savemask, 0);
  return wstatus;
}
