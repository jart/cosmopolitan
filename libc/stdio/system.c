/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"

/**
 * Launches program with system command interpreter.
 *
 * This embeds the Cosmopolitan Command Interpreter which provides
 * Bourne-like syntax on all platforms including Windows.
 *
 * @param cmdline is a unix shell script
 * @return -1 if child process couldn't be created, otherwise a wait
 *     status that can be accessed using macros like WEXITSTATUS(s),
 *     WIFSIGNALED(s), WTERMSIG(s), etc.
 * @threadsafe
 */
int system(const char *cmdline) {
  int pid, wstatus;
  sigset_t chldmask, savemask;
  struct sigaction ignore, saveint, savequit;
  if (!cmdline) return 1;
  BLOCK_CANCELLATIONS;
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  if (!(pid = fork())) {
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    _Exit(_cocmd(3, (char *[]){"system", "-c", cmdline, 0}, environ));
  } else if (pid != -1) {
    while (wait4(pid, &wstatus, 0, 0) == -1) {
      if (errno != EINTR) {
        wstatus = -1;
        break;
      }
    }
  } else {
    wstatus = -1;
  }
  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigprocmask(SIG_SETMASK, &savemask, 0);
  ALLOW_CANCELLATIONS;
  return wstatus;
}
