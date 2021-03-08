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
#include "libc/calls/scheduler.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/spawn.h"
#include "libc/stdio/spawna.internal.h"
#include "libc/str/str.h"

/**
 * Spawns process the POSIX way.
 *
 * @param pid is non-NULL and will be set to child pid in parent
 * @param path of executable that is not PATH searched
 * @return 0 on success or error number on failure
 */
int posix_spawn(int *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  unsigned mode;
  sigset_t allsigs;
  struct sigaction dfl;
  char *p, *q, opath[PATH_MAX];
  int s, fd, newfd, oflag, tempfd;
  if (!(*pid = vfork())) {
    if (attrp) {
      if (attrp->posix_attr_flags & POSIX_SPAWN_SETPGROUP) {
        if (setpgid(0, attrp->posix_attr_pgroup)) _exit(127);
      }
      if (attrp->posix_attr_flags & POSIX_SPAWN_SETSIGMASK) {
        sigprocmask(SIG_SETMASK, &attrp->posix_attr_sigmask, NULL);
      }
      if (attrp->posix_attr_flags & POSIX_SPAWN_RESETIDS) {
        setuid(getuid());
        setgid(getgid());
      }
      if (attrp->posix_attr_flags & POSIX_SPAWN_SETSIGDEF) {
        dfl.sa_handler = SIG_DFL;
        dfl.sa_flags = 0;
        sigfillset(&allsigs);
        for (s = 0; sigismember(&allsigs, s); s++) {
          if (sigismember(&attrp->posix_attr_sigdefault, s)) {
            if (sigaction(s, &dfl, NULL) == -1) _exit(127);
          }
        }
      }
    }
    if (file_actions) {
      for (p = *file_actions; *p; p = strchr(p, ')') + 1) {
        if (!strncmp(p, "close(", 6)) {
          if (sscanf(p + 6, "%d)", &fd) != 1) _exit(127);
          if (close(fd) == -1) _exit(127);
        } else if (!strncmp(p, "dup2(", 5)) {
          if (sscanf(p + 5, "%d,%d)", &fd, &newfd) != 2) _exit(127);
          if (dup2(fd, newfd) == -1) _exit(127);
        } else if (!strncmp(p, "open(", 5)) {
          if (sscanf(p + 5, "%d,", &fd) != 1) _exit(127);
          p = strchr(p, ',') + 1;
          q = strchr(p, '*');
          if (!q || q - p + 1 > PATH_MAX) _exit(127);
          strncpy(opath, p, q - p);
          opath[q - p] = '\0';
          if (sscanf(q + 1, "%o,%o)", &oflag, &mode) != 2) _exit(127);
          if (close(fd) == -1 && errno != EBADF) _exit(127);
          tempfd = open(opath, oflag, mode);
          if (tempfd == -1) _exit(127);
          if (tempfd != fd) {
            if (dup2(tempfd, fd) == -1) _exit(127);
            if (close(tempfd) == -1) _exit(127);
          }
        } else {
          _exit(127);
        }
      }
    }
    if (attrp) {
      if (attrp->posix_attr_flags & POSIX_SPAWN_SETSCHEDULER) {
        if (sched_setscheduler(0, attrp->posix_attr_schedpolicy,
                               &attrp->posix_attr_schedparam) == -1) {
          _exit(127);
        }
      }
      if (attrp->posix_attr_flags & POSIX_SPAWN_SETSCHEDPARAM) {
        if (sched_setparam(0, &attrp->posix_attr_schedparam) == -1) {
          _exit(127);
        }
      }
    }
    execve(path, argv, envp);
    _exit(127);
  } else {
    if (*pid == -1) return errno;
    return 0;
  }
}
