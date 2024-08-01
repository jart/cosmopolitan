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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/safemacros.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/temp.h"
#include "libc/x/x.h"

static char *get_pagerpath(char *pathbuf, size_t pathbufsz) {
  char *pagerpath;
  if (strcmp(nulltoempty(getenv("TERM")), "dumb") && isatty(0) && isatty(1) &&
      ((pagerpath = commandv("less", pathbuf, pathbufsz)) ||
       (pagerpath = commandv("more", pathbuf, pathbufsz)) ||
       (pagerpath = commandv("more.exe", pathbuf, pathbufsz)) ||
       (pagerpath = commandv("more.com", pathbuf, pathbufsz)))) {
    return pagerpath;
  }
  return 0;
}

static bool run_pager(char *args[hasatleast 3]) {
  char16_t widepath[PATH_MAX];
  int n, pid;
  if (IsWindows() && !strcasecmp(args[0], "/C/Windows/System32/more.com") &&
      (((n = __mkntpath(args[1], widepath)) == -1) ||
       !(args[1] = gc(utf16to8(widepath, n, 0))))) {
    return false;
  }
  if ((pid = fork()) != -1) {
    putenv("LC_ALL=C.UTF-8");
    putenv("LESSCHARSET=utf-8");
    putenv("LESS=-RS");
    if (!pid) {
      execv(args[0], args);
      _Exit(127);
    }
    waitpid(pid, 0, 0);
    return true;
  }
  return false;
}

/**
 * Displays wall of text in terminal with pagination.
 */
void __paginate(int fd, const char *s) {
  int tfd;
  char *args[3] = {0};
  char tmppath[] = "/tmp/paginate.XXXXXX";
  char progpath[PATH_MAX];
  bool done;
  if ((args[0] = get_pagerpath(progpath, sizeof(progpath)))) {
    if ((tfd = mkstemp(tmppath)) != -1) {
      write(tfd, s, strlen(s));
      close(tfd);
      args[1] = tmppath;
      done = run_pager(args);
      unlink(tmppath);
      if (done) {
        return;
      }
    }
  }
  write(fd, s, strlen(s));
}

/**
 * Displays a file in terminal with pagination
 */
void __paginate_file(int fd, const char *path) {
  char *args[3] = {0};
  char progpath[PATH_MAX];
  if ((args[0] = get_pagerpath(progpath, sizeof(progpath)))) {
    args[1] = (char *)path;
    if (run_pager(args)) {
      return;
    }
  }
  int sfd = open(path, O_RDONLY);
  if (sfd != -1) {
    ssize_t n;
    while ((n = read(sfd, progpath, sizeof(progpath)) > 0)) {
      write(fd, progpath, n);
    }
  }
  close(sfd);
}
