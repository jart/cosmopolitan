/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bsf.h"
#include "libc/intrin/maps.h"
#include "libc/proc/posix_spawn.h"
#include "libc/proc/posix_spawn.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/pib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static atomic_bool has_vfork;  // i.e. not qemu/wsl/xnu/openbsd

/**
 * Spawns process, the POSIX way, e.g.
 *
 *     int pid, status;
 *     posix_spawnattr_t sa;
 *     posix_spawnattr_init(&sa);
 *     posix_spawnattr_setflags(&sa, POSIX_SPAWN_SETPGROUP);
 *     posix_spawn_file_actions_t fa;
 *     posix_spawn_file_actions_init(&fa);
 *     posix_spawn_file_actions_addopen(&fa, 0, "/dev/null", O_RDWR, 0644);
 *     posix_spawn_file_actions_adddup2(&fa, 0, 1);
 *     posix_spawnp(&pid, "lol", &fa, &sa, (char *[]){"lol", 0}, 0);
 *     posix_spawnp(&pid, "cat", &fa, &sa, (char *[]){"cat", 0}, 0);
 *     posix_spawn_file_actions_destroy(&fa);
 *     posix_spawnattr_destroy(&sa);
 *     while (wait(&status) != -1);
 *
 * The posix_spawn() function may be used to launch subprocesses. The
 * primary advantage of using posix_spawn() instead of the traditional
 * fork() / execve() combination for launching processes is efficiency
 * and cross-platform compatibility.
 *
 * 1. On Linux, FreeBSD, and NetBSD:
 *
 *    Cosmopolitan Libc's posix_spawn() uses vfork() under the hood on
 *    these platforms automatically, since it's faster than fork(). It's
 *    because vfork() creates a child process without needing to copy
 *    the parent's page tables, making it more efficient, especially for
 *    large processes. Furthermore, vfork() avoids the need to acquire
 *    every single mutex (see pthread_atfork() for more details) which
 *    makes it scalable in multi-threaded apps, since the other threads
 *    in your app can keep going while the spawning thread waits for the
 *    subprocess to call execve(). Normally vfork() is error-prone since
 *    there exists few functions that are @vforksafe. the posix_spawn()
 *    API is designed to offer maximum assurance that you can't shoot
 *    yourself in the foot. If you do, then file a bug with Cosmo.
 *
 * 2. On Windows:
 *
 *    posix_spawn() avoids fork() entirely. Windows doesn't natively
 *    support fork(), and emulating it can be slow and memory-intensive.
 *    By using posix_spawn(), we get a much faster process creation on
 *    Windows systems, because it only needs to call CreateProcess().
 *    Your file actions are replayed beforehand in a simulated way.
 *
 * 3. Simplified error handling:
 *
 *    posix_spawn() combines process creation and program execution in a
 *    single call, reducing the points of failure and simplifying error
 *    handling. One important thing that happens with Cosmopolitan's
 *    posix_spawn() implementation is that the error code of execve()
 *    inside your subprocess, should it fail, will be propagated to your
 *    parent process. This will happen efficiently via vfork() shared
 *    memory in the event your Linux environment supports this. If it
 *    doesn't, then Cosmopolitan will fall back to a throwaway pipe().
 *    The pipe is needed on platforms like XNU and OpenBSD which do not
 *    support vfork(). It's also needed under QEMU User.
 *
 * 4. Signal safety:
 *
 *    posix_spawn() guarantees your signal handler callback functions
 *    won't be executed in the child process. By default, it'll remove
 *    sigaction() callbacks atomically. This ensures that if something
 *    like a SIGTERM or SIGHUP is sent to the child process before it's
 *    had a chance to call execve(), then the child process will simply
 *    be terminated (like the spawned process would) instead of running
 *    whatever signal handlers the spawning process has installed. If
 *    you've set some signals to SIG_IGN, then that'll be preserved for
 *    the child process by posix_spawn(), unless you explicitly call
 *    posix_spawnattr_setsigdefault() to reset them.
 *
 * 5. Portability:
 *
 *    posix_spawn() is part of the POSIX standard, making it more
 *    portable across different UNIX-like systems and Windows (with
 *    appropriate libraries). Even the non-POSIX APIs we use here are
 *    portable; e.g. posix_spawn_file_actions_addchdir_np() is supported
 *    by glibc, musl libc, and apple libc too.
 *
 * When using posix_spawn() you have the option of passing an attributes
 * object that specifies how the child process should be created. These
 * functions are provided by Cosmopolitan Libc for setting attributes:
 *
 * - posix_spawnattr_init()
 * - posix_spawnattr_destroy()
 * - posix_spawnattr_setflags()
 * - posix_spawnattr_getflags()
 * - posix_spawnattr_setsigmask()
 * - posix_spawnattr_getsigmask()
 * - posix_spawnattr_setpgroup()
 * - posix_spawnattr_getpgroup()
 * - posix_spawnattr_setrlimit_np()
 * - posix_spawnattr_getrlimit_np()
 * - posix_spawnattr_setschedparam()
 * - posix_spawnattr_getschedparam()
 * - posix_spawnattr_setschedpolicy()
 * - posix_spawnattr_getschedpolicy()
 * - posix_spawnattr_setsigdefault()
 * - posix_spawnattr_getsigdefault()
 *
 * You can also pass an ordered list of file actions to perform. The
 * following APIs are provided by Cosmopolitan Libc for doing that:
 *
 * - posix_spawn_file_actions_init()
 * - posix_spawn_file_actions_destroy()
 * - posix_spawn_file_actions_adddup2()
 * - posix_spawn_file_actions_addopen()
 * - posix_spawn_file_actions_addclose()
 * - posix_spawn_file_actions_addchdir_np()
 * - posix_spawn_file_actions_addfchdir_np()
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
 */
errno_t posix_spawn(int *pid, const char *path,
                    const posix_spawn_file_actions_t *file_actions,
                    const posix_spawnattr_t *attrp, char *const argv[],
                    char *const envp[]) {
  int pfds[2];
  bool use_vfork;
  volatile int status = 0;
  sigset_t blockall, oldmask;
  int child, res, cs, e = errno;
  volatile int can_clobber = 1234;
  short flags = attrp && *attrp ? (*attrp)->flags : 0;
  sigfillset(&blockall);
  sigprocmask(SIG_SETMASK, &blockall, &oldmask);
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
  if (!(flags & POSIX_SPAWN_USEVFORK)) {
    use_vfork = false;
  } else if (IsWindows()) {
    use_vfork = true;
  } else {
    use_vfork = atomic_load_explicit(&has_vfork, memory_order_acquire);
  }
  if (!use_vfork) {
    if (pipe2(pfds, O_CLOEXEC)) {
      res = errno;
      goto ParentFailed;
    }
  }
  if (use_vfork) {
    child = vfork();
  } else if (IsWindows()) {
    child = fork();
  } else {
    // on unix systems without a true vfork() it shouldn't be necessary
    // to call fork() which locks all the mutexes that exist beacuse we
    // are only calling system calls that are designed to not use locks
    child = sys_fork();
    if (!child) {
      // any other mutex is basically forbidden, within posix_spawn()
      if (IsModeDbg())
        __get_tls()->tib_flags |= TIB_FLAG_SPAWNED;
      // --strace and --ftrace want to be able to print pids and tids
      __get_pib()->pid = sys_getpid().ax;
      struct CosmoTib *tib = __get_tls();
      int tid = IsLinux() || IsXnuSilicon() ? __get_pib()->pid : sys_gettid();
      atomic_init(&tib->tib_ctid, tid);
      atomic_init(&tib->tib_ptid, tid);
      // pretend we're vforked so the user can't open zipos filenames
      tib->tib_flags |= TIB_FLAG_VFORKED;
    }
  }
  if (child == -1) {
    res = errno;
    close(pfds[0]);
    close(pfds[1]);
    goto ParentFailed;
  }
  if (!child) {
    can_clobber = 9876;
    sigset_t childmask;
    bool lost_cloexec = 0;
    struct sigaction dfl = {0};
    if (!use_vfork)
      close(pfds[0]);
    struct CosmoPib *pib = __get_pib();
    for (int sig = 1; sig <= NSIG; sig++)
      if (pib->sighandrvas[sig - 1] != (long)SIG_DFL &&
          (pib->sighandrvas[sig - 1] != (long)SIG_IGN ||
           ((flags & POSIX_SPAWN_SETSIGDEF) &&
            sigismember(&(*attrp)->sigdefault, sig) == 1)))
        sigaction(sig, &dfl, 0);
    if (flags & POSIX_SPAWN_SETSID)
      setsid();
    if ((flags & POSIX_SPAWN_SETPGROUP) && setpgid(0, (*attrp)->pgroup))
      goto ChildFailed;
    if ((flags & POSIX_SPAWN_RESETIDS) && setgid(getgid()))
      goto ChildFailed;
    if ((flags & POSIX_SPAWN_RESETIDS) && setuid(getuid()))
      goto ChildFailed;
    if (file_actions) {
      struct _posix_faction *a;
      for (a = *file_actions; a; a = a->next) {
        if (!use_vfork && pfds[1] == a->fildes) {
          int p2;
          if ((p2 = dup(pfds[1])) == -1)
            goto ChildFailed;
          lost_cloexec = true;
          close(pfds[1]);
          pfds[1] = p2;
        }
        switch (a->action) {
          case _POSIX_SPAWN_CLOSE:
            if (close(a->fildes))
              goto ChildFailed;
            break;
          case _POSIX_SPAWN_DUP2:
            if (dup2(a->fildes, a->newfildes) == -1)
              goto ChildFailed;
            break;
          case _POSIX_SPAWN_OPEN: {
            int t;
            if ((t = openat(AT_FDCWD, a->path, a->oflag, a->mode)) == -1)
              goto ChildFailed;
            if (t != a->fildes) {
              if (dup2(t, a->fildes) == -1) {
                close(t);
                goto ChildFailed;
              }
              if (close(t))
                goto ChildFailed;
            }
            break;
          }
          case _POSIX_SPAWN_CHDIR:
            if (chdir(a->path) == -1)
              goto ChildFailed;
            break;
          case _POSIX_SPAWN_FCHDIR:
            if (fchdir(a->fildes) == -1)
              goto ChildFailed;
            break;
          default:
            __builtin_unreachable();
        }
      }
    }
    if (IsLinux() || IsFreebsd() || IsNetbsd()) {
      if (flags & POSIX_SPAWN_SETSCHEDULER)
        if (sched_setscheduler(0, (*attrp)->schedpolicy,
                               &(*attrp)->schedparam) == -1)
          goto ChildFailed;
      if (flags & POSIX_SPAWN_SETSCHEDPARAM)
        if (sched_setparam(0, &(*attrp)->schedparam))
          goto ChildFailed;
    }
    if (flags & POSIX_SPAWN_SETRLIMIT_NP) {
      int rlimset = (*attrp)->rlimset;
      while (rlimset) {
        int resource = bsf(rlimset);
        rlimset &= ~(1u << resource);
        if (setrlimit(resource, (*attrp)->rlim + resource)) {
          // MacOS ARM64 RLIMIT_STACK always returns EINVAL
          if (!IsXnuSilicon()) {
            goto ChildFailed;
          }
        }
      }
    }
    if (lost_cloexec)
      fcntl(pfds[1], F_SETFD, FD_CLOEXEC);
    if (flags & POSIX_SPAWN_SETSIGMASK) {
      childmask = (*attrp)->sigmask;
    } else {
      childmask = oldmask;
    }
    sigprocmask(SIG_SETMASK, &childmask, 0);
    if (!envp)
      envp = environ;
    execve(path, argv, envp);
  ChildFailed:
    res = errno;
    if (use_vfork) {
      status = res;
    } else {
      write(pfds[1], &res, sizeof(res));
    }
    _Exit(127);
  }
  unassert(can_clobber == 1234 || can_clobber == 9876);
  if (!use_vfork)
    close(pfds[1]);
  if (child != -1) {
    if (use_vfork) {
      res = status;
    } else {
      if (can_clobber == 9876)
        atomic_store_explicit(&has_vfork, true, memory_order_release);
      res = 0;
      read(pfds[0], &res, sizeof(res));
    }
    if (!res) {
      if (pid)
        *pid = child;
    } else {
      wait4(child, 0, 0, 0);
    }
  } else {
    res = errno;
  }
  if (!use_vfork)
    close(pfds[0]);
ParentFailed:
  sigprocmask(SIG_SETMASK, &oldmask, 0);
  pthread_setcancelstate(cs, 0);
  errno = e;
  return res;
}
