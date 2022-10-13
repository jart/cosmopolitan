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
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/posix_spawn.h"
#include "libc/stdio/posix_spawn.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static int RunFileActions(struct _posix_faction *a) {
  int t;
  if (!a) return 0;
  if (RunFileActions(a->next) == -1) return -1;
  switch (a->action) {
    case _POSIX_SPAWN_CLOSE:
      return close(a->fildes);
    case _POSIX_SPAWN_DUP2:
      return dup2(a->fildes, a->newfildes);
    case _POSIX_SPAWN_OPEN:
      if ((t = open(a->path, a->oflag, a->mode)) == -1) return -1;
      if (t != a->fildes) {
        if (dup2(t, a->fildes) == -1) {
          close(t);
          return -1;
        }
        if (close(t) == -1) {
          return -1;
        }
      }
      return 0;
    default:
      unreachable;
  }
}

/**
 * Spawns process the POSIX way.
 *
 * @param pid if non-null shall be set to child pid on success
 * @param path is resolved path of program which is not `$PATH` searched
 * @param file_actions specifies close(), dup2(), and open() operations
 * @param attrp specifies signal masks, user ids, scheduling, etc.
 * @param envp is environment variables, or `environ` if null
 * @return 0 on success or error number on failure
 * @see posix_spawnp() for `$PATH` searching
 */
int posix_spawn(int *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  int s, child;
  sigset_t allsigs;
  struct sigaction dfl;
  if (!(child = _weaken(pthread_create) ? fork() : vfork())) {
    if (attrp && *attrp) {
      if ((*attrp)->flags & POSIX_SPAWN_SETPGROUP) {
        if (setpgid(0, (*attrp)->pgroup)) _Exit(127);
      }
      if ((*attrp)->flags & POSIX_SPAWN_SETSIGMASK) {
        sigprocmask(SIG_SETMASK, &(*attrp)->sigmask, 0);
      }
      if ((*attrp)->flags & POSIX_SPAWN_RESETIDS) {
        setuid(getuid());
        setgid(getgid());
      }
      if ((*attrp)->flags & POSIX_SPAWN_SETSIGDEF) {
        dfl.sa_handler = SIG_DFL;
        dfl.sa_flags = 0;
        sigfillset(&allsigs);
        for (s = 0; sigismember(&allsigs, s); s++) {
          if (sigismember(&(*attrp)->sigdefault, s)) {
            if (sigaction(s, &dfl, 0) == -1) _Exit(127);
          }
        }
      }
    }
    if (file_actions) {
      if (RunFileActions(*file_actions) == -1) {
        _Exit(127);
      }
    }
    if (attrp && *attrp) {
      if ((*attrp)->flags & POSIX_SPAWN_SETSCHEDULER) {
        if (sched_setscheduler(0, (*attrp)->schedpolicy,
                               &(*attrp)->schedparam) == -1) {
          if (errno != ENOSYS) _Exit(127);
        }
      }
      if ((*attrp)->flags & POSIX_SPAWN_SETSCHEDPARAM) {
        if (sched_setparam(0, &(*attrp)->schedparam) == -1) {
          if (errno != ENOSYS) _Exit(127);
        }
      }
    }
    if (!envp) envp = environ;
    execve(path, argv, envp);
    _Exit(127);
  } else if (child != -1) {
    if (pid) *pid = child;
    return 0;
  } else {
    return errno;
  }
}
