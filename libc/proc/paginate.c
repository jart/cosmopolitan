/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"

#define RUN(fd, ofd, pfd, func) \
  do {                          \
    int i;                      \
    for (i = 0; i < 2; i++) {   \
      if (i == fd) {            \
        ofd = dup(fd);          \
        dup2(pfd[i], fd);       \
      }                         \
      close(pfd[i]);            \
    }                           \
    func;                       \
    dup2(ofd, fd);              \
    close(ofd);                 \
  } while (0)

/**
 * Displays wall of text in terminal with pagination.
 */
void __paginate(int fd, const char *s) {
  int ofd, pid, pfd[2];
  char *prog;
  if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) &&
      strcmp(nulltoempty(getenv("TERM")), "dumb") &&
      ((prog = commandv("less", _gc(malloc(PATH_MAX)), PATH_MAX)) ||
       (prog = commandv("more", _gc(malloc(PATH_MAX)), PATH_MAX))) &&
      !pipe2(pfd, O_CLOEXEC)) {
    if ((pid = fork()) != -1) {
      putenv("LC_ALL=C.UTF-8");
      putenv("LESSCHARSET=utf-8");
      putenv("LESS=-RS");
      if (!pid) {
        RUN(STDIN_FILENO, ofd, pfd, execv(prog, (char *const[]){prog, NULL}));
        _Exit(127);
      } else {
        RUN(STDOUT_FILENO, ofd, pfd, write(STDOUT_FILENO, s, strlen(s)));
        waitpid(pid, NULL, 0);
      }
      return;
    }
    close(pfd[0]);
    close(pfd[1]);
  }
  write(fd, s, strlen(s));
}
