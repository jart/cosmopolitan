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
#include "libc/sysv/consts/sig.h"
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
 * Spawns process, the POSIX way.
 *
 * This function provides an API for vfork() that's intended to be less
 * terrifying to the uninitiated, since it only lets you define actions
 * that are @vforksafe. This function requires TLS not be disabled.
 *
 * @param pid if non-null shall be set to child pid on success
 * @param path is resolved path of program which is not `$PATH` searched
 * @param file_actions specifies close(), dup2(), and open() operations
 * @param attrp specifies signal masks, user ids, scheduling, etc.
 * @param envp is environment variables, or `environ` if null
 * @return 0 on success or error number on failure
 * @see posix_spawnp() for `$PATH` searching
 * @tlsrequired
 * @threadsafe
 */
int posix_spawn(int *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp, char *const argv[],
                char *const envp[]) {
  short flags = 0;
  sigset_t sigmask;
  int s, child, policy;
  struct sched_param param;
  struct sigaction dfl = {0};
  if (!(child = vfork())) {
    if (attrp && *attrp) {
      posix_spawnattr_getflags(attrp, &flags);
      if (flags & POSIX_SPAWN_SETPGROUP) {
        if (setpgid(0, (*attrp)->pgroup)) {
          _Exit(127);
        }
      }
      if (flags & POSIX_SPAWN_SETSIGMASK) {
        posix_spawnattr_getsigmask(attrp, &sigmask);
        sigprocmask(SIG_SETMASK, &sigmask, 0);
      }
      if ((flags & POSIX_SPAWN_RESETIDS) &&
          (setgid(getgid()) || setuid(getuid()))) {
        _Exit(127);
      }
      if (flags & POSIX_SPAWN_SETSIGDEF) {
        for (s = 1; s < 32; s++) {
          if (sigismember(&(*attrp)->sigdefault, s)) {
            if (sigaction(s, &dfl, 0) == -1) {
              _Exit(127);
            }
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
      if (flags & POSIX_SPAWN_SETSCHEDULER) {
        posix_spawnattr_getschedpolicy(attrp, &policy);
        posix_spawnattr_getschedparam(attrp, &param);
        if (sched_setscheduler(0, policy, &param) == -1) {
          _Exit(127);
        }
      }
      if (flags & POSIX_SPAWN_SETSCHEDPARAM) {
        posix_spawnattr_getschedparam(attrp, &param);
        if (sched_setparam(0, &param) == -1) {
          _Exit(127);
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
