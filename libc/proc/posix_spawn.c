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
#include "libc/proc/posix_spawn.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/proc/ntspawn.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rlimit.internal.h"
#include "libc/calls/struct/rusage.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/handlock.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/alloca.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/proc/posix_spawn.h"
#include "libc/proc/posix_spawn.internal.h"
#include "libc/proc/proc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#ifndef SYSDEBUG
#define read        sys_read
#define write       sys_write
#define close       sys_close
#define pipe2       sys_pipe2
#define getgid      sys_getgid
#define setgid      sys_setgid
#define getuid      sys_getuid
#define setuid      sys_setuid
#define setsid      sys_setsid
#define setpgid     sys_setpgid
#define fcntl       __sys_fcntl
#define wait4       __sys_wait4
#define openat      __sys_openat
#define setrlimit   sys_setrlimit
#define sigprocmask sys_sigprocmask
#endif

static atomic_bool has_vfork;  // i.e. not qemu/wsl/xnu/openbsd

static void posix_spawn_unhand(int64_t hands[3]) {
  for (int i = 0; i < 3; ++i) {
    if (hands[i] != -1) {
      CloseHandle(hands[i]);
    }
  }
}

static void posix_spawn_inherit(int64_t hands[3], bool32 bInherit) {
  for (int i = 0; i < 3; ++i) {
    if (hands[i] != -1) {
      SetHandleInformation(hands[i], kNtHandleFlagInherit, bInherit);
    }
  }
}

static textwindows errno_t posix_spawn_windows_impl(
    int *pid, const char *path, const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]) {
  int i;

  // create file descriptor work area
  char stdio_kind[3] = {kFdEmpty, kFdEmpty, kFdEmpty};
  intptr_t stdio_handle[3] = {-1, -1, -1};
  for (i = 0; i < 3; ++i) {
    if (g_fds.p[i].kind != kFdEmpty && !(g_fds.p[i].flags & O_CLOEXEC)) {
      stdio_kind[i] = g_fds.p[i].kind;
      stdio_handle[i] = g_fds.p[i].handle;
    }
  }

  // reserve object for tracking proces
  struct Proc *proc;
  __proc_lock();
  proc = __proc_new();
  __proc_unlock();
  if (!proc) return -1;

  // apply user file actions
  intptr_t close_handle[3] = {-1, -1, -1};
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
          int64_t hand;
          int e = errno;
          char16_t path16[PATH_MAX];
          uint32_t perm, share, disp, attr;
          unassert(a->fildes < 3u);
          if (__mkntpathat(AT_FDCWD, a->path, 0, path16) != -1 &&
              GetNtOpenFlags(a->oflag, a->mode,  //
                             &perm, &share, &disp, &attr) != -1 &&
              (hand = CreateFile(path16, perm, share, 0, disp, attr, 0))) {
            stdio_kind[a->fildes] = kFdFile;
            close_handle[a->fildes] = hand;
            stdio_handle[a->fildes] = hand;
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
      posix_spawn_unhand(close_handle);
      __proc_lock();
      __proc_free(proc);
      __proc_unlock();
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
  short flags = 0;
  bool bInheritHandles = false;
  uint32_t dwCreationFlags = 0;
  for (i = 0; i < 3; ++i) {
    bInheritHandles |= stdio_handle[i] != -1;
  }
  if (attrp && *attrp) {
    flags = (*attrp)->flags;
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
  __hand_rlock();
  posix_spawn_inherit(stdio_handle, true);
  rc = ntspawn(path, argv, envp, v, 0, 0, bInheritHandles, dwCreationFlags, 0,
               &startinfo, &procinfo);
  posix_spawn_inherit(stdio_handle, false);
  posix_spawn_unhand(close_handle);
  __hand_runlock();
  if (rc == -1) {
    int err = errno;
    __proc_lock();
    __proc_free(proc);
    __proc_unlock();
    errno = e;
    return err;
  }

  // return the result
  CloseHandle(procinfo.hThread);
  proc->pid = procinfo.dwProcessId;
  proc->handle = procinfo.hProcess;
  if (pid) *pid = proc->pid;
  __proc_lock();
  __proc_add(proc);
  __proc_unlock();
  return 0;
}

static const char *DescribePid(char buf[12], int err, int *pid) {
  if (err) return "n/a";
  if (!pid) return "NULL";
  FormatInt32(buf, *pid);
  return buf;
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

/**
 * Spawns process, the POSIX way.
 *
 * This provides superior process creation performance across systems
 *
 * Processes are normally spawned by calling fork() and execve(), but
 * that goes slow on Windows if the caller has allocated a nontrivial
 * number of memory mappings, all of which need to be copied into the
 * forked child, only to be destroyed a moment later. On UNIX systems
 * fork() bears a similar cost that's 100x less bad, which is copying
 * the page tables. So what this implementation does is on Windows it
 * calls CreateProcess() directly and on UNIX it uses vfork() if it's
 * possible (XNU and OpenBSD don't have it). On UNIX this API has the
 * benefit of avoiding the footguns of using vfork() directly because
 * this implementation will ensure signal handlers can't be called in
 * the child process since that'd likely corrupt the parent's memory.
 * On systems with a real vfork() implementation, the execve() status
 * code is returned by this function via shared memory; otherwise, it
 * gets passed via a temporary pipe (on systems like QEmu, Blink, and
 * XNU/OpenBSD) whose support is auto-detected at runtime.
 *
 * @param pid if non-null shall be set to child pid on success
 * @param path is resolved path of program which is not `$PATH` searched
 * @param file_actions specifies close(), dup2(), and open() operations
 * @param attrp specifies signal masks, user ids, scheduling, etc.
 * @param envp is environment variables, or `environ` if null
 * @return 0 on success or error number on failure
 * @raise ETXTBSY if another process has `path` open in write mode
 * @raise ENOEXEC if file is executable but not a valid format
 * @raise ENOMEM if remaining stack memory is insufficient
 * @raise EACCES if execute permission was denied
 * @see posix_spawnp() for `$PATH` searching
 * @returnserrno
 * @tlsrequired
 * @threadsafe
 */
errno_t posix_spawn(int *pid, const char *path,
                    const posix_spawn_file_actions_t *file_actions,
                    const posix_spawnattr_t *attrp, char *const argv[],
                    char *const envp[]) {
  if (IsWindows()) {
    return posix_spawn_windows(pid, path, file_actions, attrp, argv, envp);
  }
  int pfds[2];
  bool use_pipe;
  volatile int status = 0;
  sigset_t blockall, oldmask;
  int child, res, cs, e = errno;
  volatile bool can_clobber = false;
  sigfillset(&blockall);
  sigprocmask(SIG_SETMASK, &blockall, &oldmask);
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  if ((use_pipe = !atomic_load_explicit(&has_vfork, memory_order_acquire))) {
    if (pipe2(pfds, O_CLOEXEC)) {
      res = errno;
      goto ParentFailed;
    }
  }
  if (!(child = vfork())) {
    can_clobber = true;
    sigset_t *childmask;
    bool lost_cloexec = 0;
    struct sigaction dfl = {0};
    short flags = attrp && *attrp ? (*attrp)->flags : 0;
    if (use_pipe) close(pfds[0]);
    for (int sig = 1; sig < _NSIG; sig++) {
      if (__sighandrvas[sig] != (long)SIG_DFL &&
          (__sighandrvas[sig] != (long)SIG_IGN ||
           ((flags & POSIX_SPAWN_SETSIGDEF) &&
            sigismember(&(*attrp)->sigdefault, sig) == 1))) {
        sigaction(sig, &dfl, 0);
      }
    }
    if (flags & POSIX_SPAWN_SETSID) {
      setsid();
    }
    if ((flags & POSIX_SPAWN_SETPGROUP) && setpgid(0, (*attrp)->pgroup)) {
      goto ChildFailed;
    }
    if ((flags & POSIX_SPAWN_RESETIDS) && setgid(getgid())) {
      goto ChildFailed;
    }
    if ((flags & POSIX_SPAWN_RESETIDS) && setuid(getuid())) {
      goto ChildFailed;
    }
    if (file_actions) {
      struct _posix_faction *a;
      for (a = *file_actions; a; a = a->next) {
        if (use_pipe && pfds[1] == a->fildes) {
          int p2;
          if ((p2 = dup(pfds[1])) == -1) {
            goto ChildFailed;
          }
          lost_cloexec = true;
          close(pfds[1]);
          pfds[1] = p2;
        }
        switch (a->action) {
          case _POSIX_SPAWN_CLOSE:
            if (close(a->fildes)) {
              goto ChildFailed;
            }
            break;
          case _POSIX_SPAWN_DUP2:
            if (dup2(a->fildes, a->newfildes) == -1) {
              goto ChildFailed;
            }
            break;
          case _POSIX_SPAWN_OPEN: {
            int t;
            if ((t = openat(AT_FDCWD, a->path, a->oflag, a->mode)) == -1) {
              goto ChildFailed;
            }
            if (t != a->fildes) {
              if (dup2(t, a->fildes) == -1) {
                close(t);
                goto ChildFailed;
              }
              if (close(t)) {
                goto ChildFailed;
              }
            }
            break;
          }
          default:
            __builtin_unreachable();
        }
      }
    }
    if (IsLinux() || IsFreebsd() || IsNetbsd()) {
      if (flags & POSIX_SPAWN_SETSCHEDULER) {
        if (sched_setscheduler(0, (*attrp)->schedpolicy,
                               &(*attrp)->schedparam) == -1) {
          goto ChildFailed;
        }
      }
      if (flags & POSIX_SPAWN_SETSCHEDPARAM) {
        if (sched_setparam(0, &(*attrp)->schedparam)) {
          goto ChildFailed;
        }
      }
    }
    if (flags & POSIX_SPAWN_SETRLIMIT) {
      for (int rez = 0; rez <= ARRAYLEN((*attrp)->rlim); ++rez) {
        if ((*attrp)->rlimset & (1u << rez)) {
          if (setrlimit(rez, (*attrp)->rlim + rez)) {
            goto ChildFailed;
          }
        }
      }
    }
    if (lost_cloexec) {
      fcntl(pfds[1], F_SETFD, FD_CLOEXEC);
    }
    if (flags & POSIX_SPAWN_SETSIGMASK) {
      childmask = &(*attrp)->sigmask;
    } else {
      childmask = &oldmask;
    }
    sigprocmask(SIG_SETMASK, childmask, 0);
    if (!envp) envp = environ;
    execve(path, argv, envp);
  ChildFailed:
    res = errno;
    if (!use_pipe) {
      status = res;
    } else {
      write(pfds[1], &res, sizeof(res));
    }
    _Exit(127);
  }
  if (use_pipe) {
    close(pfds[1]);
  }
  if (child != -1) {
    if (!use_pipe) {
      res = status;
    } else {
      if (can_clobber) {
        atomic_store_explicit(&has_vfork, true, memory_order_release);
      }
      res = 0;
      read(pfds[0], &res, sizeof(res));
    }
    if (!res) {
      if (pid) *pid = child;
    } else {
      wait4(child, 0, 0, 0);
    }
  } else {
    res = errno;
  }
  if (use_pipe) {
    close(pfds[0]);
  }
ParentFailed:
  sigprocmask(SIG_SETMASK, &oldmask, 0);
  pthread_setcancelstate(cs, 0);
  errno = e;
  return res;
}
