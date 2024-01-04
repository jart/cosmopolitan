/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/temp.h"

/**
 * Displays wall of text in terminal with pagination.
 */
void __paginate(int fd, const char *s) {
  int tfd, pid;
  char *args[3] = {0};
  char tmppath[] = "/tmp/paginate.XXXXXX";
  char progpath[PATH_MAX];
  if (strcmp(nulltoempty(getenv("TERM")), "dumb") && isatty(0) && isatty(1) &&
      ((args[0] = commandv("less", progpath, sizeof(progpath))) ||
       (args[0] = commandv("more", progpath, sizeof(progpath))) ||
       (args[0] = commandv("more.exe", progpath, sizeof(progpath))))) {
    if ((tfd = mkstemp(tmppath)) != -1) {
      write(tfd, s, strlen(s));
      close(tfd);
      args[1] = tmppath;
      if ((pid = fork()) != -1) {
        putenv("LC_ALL=C.UTF-8");
        putenv("LESSCHARSET=utf-8");
        putenv("LESS=-RS");
        if (!pid) {
          execv(args[0], args);
          _Exit(127);
        }
        waitpid(pid, 0, 0);
        unlink(tmppath);
        return;
      }
      unlink(tmppath);
    }
  }
  write(fd, s, strlen(s));
}
