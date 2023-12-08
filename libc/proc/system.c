/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
 * This implementation embeds the Cosmopolitan Command Interpreter which
 * provides Bourne-like syntax on all platforms, including Windows. Many
 * builtin commands are included, e.g. exit, cd, rm, [, cat, wait, exec,
 * env, echo, read, true, test, kill, touch, rmdir, mkdir, false, mktemp
 * and usleep. It's also possible to __static_yoink() the symbols `_tr`,
 * `_sed`, `_awk`, and `_curl` for the tr, sed, awk and curl commands if
 * you're using the Cosmopolitan mono-repo.
 *
 * If you just have a program name and arguments, and you don't need the
 * full power of a UNIX-like shell, then consider using the Cosmopolitan
 * provided API systemvpe() instead. It provides a safer alternative for
 * variable arguments than shell script escaping. It lets you clean your
 * environment variables, for even more safety. Finally it's 10x faster.
 *
 * It's important to check the returned status code. For example, if you
 * press CTRL-C while running your program you'll expect it to terminate
 * however that won't be the case if the SIGINT gets raised while inside
 * the system() function. If the child process doesn't handle the signal
 * then this will return e.g. WIFSIGNALED(ws) && WTERMSIG(ws) == SIGINT.
 *
 * @param cmdline is a unix shell script
 * @return -1 if child process couldn't be created, otherwise a wait
 *     status that can be accessed using macros like WEXITSTATUS(s),
 *     WIFSIGNALED(s), WTERMSIG(s), etc.
 * @see systemve()
 * @threadsafe
 */
int system(const char *cmdline) {
  int pid, wstatus;
  sigset_t chldmask, savemask;
  if (!cmdline) return 1;
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGINT);
  sigaddset(&chldmask, SIGQUIT);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  if (!(pid = fork())) {
    sigprocmask(SIG_SETMASK, &savemask, 0);
    _Exit(_cocmd(3, (char *[]){"system", "-c", (char *)cmdline, 0}, environ));
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
