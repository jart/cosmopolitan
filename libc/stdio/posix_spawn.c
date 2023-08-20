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
#include "libc/stdio/posix_spawn.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/ntspawn.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/alloca.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/posix_spawn.h"
#include "libc/stdio/posix_spawn.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static void posix_spawn_cleanup3fds(int fds[3]) {
  for (int i = 0; i < 3; ++i) {
    if (fds[i] != -1) {
      int e = errno;
      if (close(fds[i])) {
        errno = e;
      }
    }
  }
}

static const char *DescribePid(char buf[12], int err, int *pid) {
  if (err) return "n/a";
  if (!pid) return "NULL";
  FormatInt32(buf, *pid);
  return buf;
}

static textwindows errno_t posix_spawn_windows_impl(
    int *pid, const char *path, const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
  int i;

  // create file descriptor work area
  char stdio_kind[3] = {kFdEmpty, kFdEmpty, kFdEmpty};
  intptr_t stdio_handle[3] = {-1, -1, -1};
  int close_this_fd_later[3] = {-1, -1, -1};
  for (i = 0; i < 3; ++i) {
    if (g_fds.p[i].kind != kFdEmpty && !(g_fds.p[i].flags & O_CLOEXEC)) {
      stdio_kind[i] = g_fds.p[i].kind;
      stdio_handle[i] = g_fds.p[i].handle;
    }
  }

  // reserve a fake pid for this spawn
  int child = __reservefd(-1);

  // apply user file actions
  if (file_actions) {
    int err = 0;
    for (struct _posix_faction *a = *file_actions; a && !err; a = a->next) {
      switch (a->action) {
        case _POSIX_SPAWN_CLOSE:
          unassert(a->fildes < 3u);
          stdio_kind[a->fildes] = kFdEmpty;
          stdio_handle[a->fildes] = -1;
          break;
        case _POSIX_SPAWN_DUP2:
          unassert(a->newfildes < 3u);
          if (__isfdopen(a->fildes)) {
            stdio_kind[a->newfildes] = g_fds.p[a->fildes].kind;
            stdio_handle[a->newfildes] = g_fds.p[a->fildes].handle;
          } else {
            err = EBADF;
          }
          break;
        case _POSIX_SPAWN_OPEN: {
          int fd, e = errno;
          unassert(a->fildes < 3u);
          if ((fd = open(a->path, a->oflag, a->mode)) != -1) {
            close_this_fd_later[a->fildes] = fd;
            stdio_kind[a->fildes] = g_fds.p[fd].kind;
            stdio_handle[a->fildes] = g_fds.p[fd].handle;
          } else {
            err = errno;
            errno = e;
          }
          break;
        }
        default:
          __builtin_unreachable();
      }
    }
    if (err) {
      posix_spawn_cleanup3fds(close_this_fd_later);
      __releasefd(child);
      return err;
    }
  }

  // create the windows process start info
  int bits;
  char buf[32], *v = 0;
  if (_weaken(socket)) {
    for (bits = i = 0; i < 3; ++i) {
      if (stdio_kind[i] == kFdSocket) {
        bits |= 1 << i;
      }
    }
    FormatInt32(stpcpy(buf, "__STDIO_SOCKETS="), bits);
    v = buf;
  }
  struct NtStartupInfo startinfo = {
      .cb = sizeof(struct NtStartupInfo),
      .dwFlags = kNtStartfUsestdhandles,
      .hStdInput = stdio_handle[0],
      .hStdOutput = stdio_handle[1],
      .hStdError = stdio_handle[2],
  };

  // figure out the flags
  short flags;
  bool bInheritHandles = false;
  uint32_t dwCreationFlags = 0;
  for (i = 0; i < 3; ++i) {
    bInheritHandles |= stdio_handle[i] != -1;
  }
  if (attrp && *attrp && !posix_spawnattr_getflags(attrp, &flags)) {
    if (flags & POSIX_SPAWN_SETSID) {
      dwCreationFlags |= kNtDetachedProcess;
    }
    if (flags & POSIX_SPAWN_SETPGROUP) {
      dwCreationFlags |= kNtCreateNewProcessGroup;
    }
  }

  // launch the process
  int rc, e = errno;
  struct NtProcessInformation procinfo;
  if (!envp) envp = environ;
  rc = ntspawn(path, argv, envp, v, 0, 0, bInheritHandles, dwCreationFlags, 0,
               &startinfo, &procinfo);
  posix_spawn_cleanup3fds(close_this_fd_later);
  if (rc == -1) {
    int err = errno;
    __releasefd(child);
    errno = e;
    return err;
  }

  // track the process
  CloseHandle(procinfo.hThread);
  g_fds.p[child].kind = kFdProcess;
  g_fds.p[child].handle = procinfo.hProcess;
  g_fds.p[child].flags = O_CLOEXEC;
  g_fds.p[child].zombie = false;

  // return the result
  if (pid) *pid = child;
  return 0;
}

static textwindows dontinline errno_t posix_spawn_windows(
    int *pid, const char *path, const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
  int err;
  if (!path || !argv ||
      (IsAsan() && (!__asan_is_valid_str(path) ||      //
                    !__asan_is_valid_strlist(argv) ||  //
                    (envp && !__asan_is_valid_strlist(envp))))) {
    err = EFAULT;
  } else {
    err = posix_spawn_windows_impl(pid, path, file_actions, attrp, argv, envp);
  }
  STRACE("posix_spawn([%s], %#s, %s, %s) → %s",
         DescribePid(alloca(12), err, pid), path, DescribeStringList(argv),
         DescribeStringList(envp), !err ? "0" : _strerrno(err));
  return err;
}

static wontreturn void posix_spawn_die(const char *fail_func) {
  STRACE("posix_spawn: %s failed% m", fail_func);
  _Exit(127);
}

static void RunUnixFileActions(struct _posix_faction *a) {
  for (; a; a = a->next) {
    switch (a->action) {
      case _POSIX_SPAWN_CLOSE:
        if (close(a->fildes)) {
          posix_spawn_die("close");
        }
        break;
      case _POSIX_SPAWN_DUP2:
        if (dup2(a->fildes, a->newfildes) == -1) {
          posix_spawn_die("dup2");
        }
        break;
      case _POSIX_SPAWN_OPEN: {
        int t;
        if ((t = open(a->path, a->oflag, a->mode)) == -1) {
          posix_spawn_die("open");
        }
        if (t != a->fildes) {
          if (dup2(t, a->fildes) == -1) {
            close(t);
            posix_spawn_die("dup2");
          }
          if (close(t)) {
            posix_spawn_die("close");
          }
        }
        break;
      }
      default:
        __builtin_unreachable();
    }
  }
}

/**
 * Spawns process, the POSIX way.
 *
 * This provides superior process creation performance across systems.
 *
 * Processes are normally spawned by calling fork() and execve(), but
 * that goes slow on Windows if the caller has allocated a nontrivial
 * number of memory mappings, all of which need to be copied into the
 * forked child, only to be destroyed a moment later. On UNIX systems
 * fork() bears a similar cost that's 100x less bad, which is copying
 * the page tables. So what this implementation does is on Windows it
 * calls CreateProcess() directly and on UNIX it uses vfork() if it's
 * possible (XNU and OpenBSD don't have it).
 *
 * If the child process exits with status 127 then use the `--strace`
 * flag to get an explanation of failures that occurred during spawn.
 *
 * @param pid if non-null shall be set to child pid on success
 * @param path is resolved path of program which is not `$PATH` searched
 * @param file_actions specifies close(), dup2(), and open() operations
 * @param attrp specifies signal masks, user ids, scheduling, etc.
 * @param envp is environment variables, or `environ` if null
 * @return 0 on success or error number on failure
 * @see posix_spawnp() for `$PATH` searching
 * @returnserrno
 * @tlsrequired
 * @threadsafe
 */
errno_t posix_spawn(int *pid, const char *path,
                    const posix_spawn_file_actions_t *file_actions,
                    const posix_spawnattr_t *attrp, char *const argv[],
                    char *const envp[]) {
  short flags = 0;
  sigset_t sigmask;
  int s, child, policy;
  struct sched_param param;
  struct sigaction dfl = {0};
  if (IsWindows()) {
    return posix_spawn_windows(pid, path, file_actions, attrp, argv, envp);
  }
  if (!(child = vfork())) {
    if (attrp && *attrp) {
      posix_spawnattr_getflags(attrp, &flags);
      if (flags & POSIX_SPAWN_SETSID) {
        if (setsid()) {
          posix_spawn_die("setsid");
        }
      }
      if (flags & POSIX_SPAWN_SETPGROUP) {
        if (setpgid(0, (*attrp)->pgroup)) {
          posix_spawn_die("setpgid");
        }
      }
      if (flags & POSIX_SPAWN_SETSIGMASK) {
        posix_spawnattr_getsigmask(attrp, &sigmask);
        sigprocmask(SIG_SETMASK, &sigmask, 0);
      }
      if ((flags & POSIX_SPAWN_RESETIDS) &&
          (setgid(getgid()) || setuid(getuid()))) {
        posix_spawn_die("setuid");
      }
      if (flags & POSIX_SPAWN_SETSIGDEF) {
        for (s = 1; s < 32; s++) {
          if (sigismember(&(*attrp)->sigdefault, s)) {
            if (sigaction(s, &dfl, 0)) {
              posix_spawn_die("sigaction");
            }
          }
        }
      }
    }
    if (file_actions) {
      RunUnixFileActions(*file_actions);
    }
    if (attrp && *attrp) {
      if (flags & POSIX_SPAWN_SETSCHEDULER) {
        posix_spawnattr_getschedpolicy(attrp, &policy);
        posix_spawnattr_getschedparam(attrp, &param);
        if (sched_setscheduler(0, policy, &param) == -1) {
          posix_spawn_die("sched_setscheduler");
        }
      }
      if (flags & POSIX_SPAWN_SETSCHEDPARAM) {
        posix_spawnattr_getschedparam(attrp, &param);
        if (sched_setparam(0, &param)) {
          posix_spawn_die("sched_setparam");
        }
      }
    }
    if (!envp) envp = environ;
    execve(path, argv, envp);
    posix_spawn_die("execve");
  } else if (child != -1) {
    if (pid) *pid = child;
    return 0;
  } else {
    return errno;
  }
}
