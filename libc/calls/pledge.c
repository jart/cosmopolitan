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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/pledge.internal.h"
#include "libc/calls/prctl.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/errfuns.h"

/**
 * Permits system operations, e.g.
 *
 *     __pledge_mode = PLEDGE_PENALTY_KILL_PROCESS | PLEDGE_STDERR_LOGGING;
 *     if (pledge("stdio rpath tty", 0)) {
 *       perror("pledge");
 *       exit(1);
 *     }
 *
 * Pledging causes most system calls to become unavailable. Your system
 * call policy is enforced by the kernel (which means it can propagate
 * across execve() if permitted). Root access is not required. Support
 * is limited to Linux 2.6.23+ (c. RHEL6) and OpenBSD. If your kernel
 * isn't supported, then pledge() will return 0 and do nothing rather
 * than raising ENOSYS. This implementation doesn't consider lack of
 * system support to be an error by default. To perform a functionality
 * check, use `pledge(0,0)` which is a no-op that'll fail appropriately
 * when the necessary system support isn't available for restrictions.
 *
 * The promises you give pledge() define which system calls are allowed.
 * Error messages are logged when sandbox violations occur, but how that
 * happens depends on the `mode` parameter (see below).
 *
 * Timing is everything with pledge. It's designed to be a voluntary
 * self-imposed security model. That works best when programs perform
 * permission-hungry operations (e.g. calling GetSymbolTable) towards
 * the beginning of execution, and then relinquish privilege afterwards
 * by calling pledge().
 *
 * By default exit() is allowed. This is useful for processes that
 * perform pure computation and interface with the parent via shared
 * memory. On Linux we mean sys_exit (_Exit1), not sys_exit_group
 * (_Exit). The difference is effectively meaningless, since _Exit()
 * will attempt both. All it means is that, if you're using threads,
 * then a `pledge("", 0)` thread can't kill all your threads unless you
 * `pledge("stdio", 0)`.
 *
 * Once pledge is in effect, the chmod functions (if allowed) will not
 * permit the sticky/setuid/setgid bits to change. Linux will EPERM here
 * and OpenBSD should ignore those three bits rather than crashing.
 *
 * User and group IDs can't be changed once pledge is in effect. OpenBSD
 * should ignore chown without crashing; whereas Linux will just EPERM.
 *
 * Using pledge is irreversible. On Linux it causes PR_SET_NO_NEW_PRIVS
 * to be set on your process; however, if "id" or "recvfd" are allowed
 * then then they theoretically could permit the gaining of some new
 * privileges. You may call pledge() multiple times if "stdio" is
 * allowed. In that case, the process can only move towards a more
 * restrictive state.
 *
 * pledge() can't filter filesystem paths. See unveil() which lets you
 * do that. pledge() also can't do address firewalling. For example if
 * you use the `inet` promise then your process will be able to talk to
 * *every* internet address including public ones.
 *
 * `promises` is a string that may include any of the following groups
 * delimited by spaces.
 *
 * - "stdio" allows exit, close, dup, dup2, dup3, fchdir, fstat, fsync,
 *   fdatasync, ftruncate, getdents, getegid, getrandom, geteuid,
 *   getgid, getgroups, times, getrusage, getitimer, getpgid, getpgrp,
 *   getpid, getppid, getresgid, getresuid, getrlimit, getsid, wait4,
 *   gettimeofday, getuid, lseek, madvise, brk, arch_prctl, uname,
 *   set_tid_address, clock_getres, clock_gettime, clock_nanosleep,
 *   mremap, mmap, (PROT_EXEC and weird flags aren't allowed), mprotect
 *   (PROT_EXEC isn't allowed), msync, sync_file_range, migrate_pages,
 *   munmap, nanosleep, pipe, pipe2, read, readv, pread, recv, poll,
 *   recvfrom, preadv, write, writev, pwrite, pwritev, select, pselect6,
 *   copy_file_range, sendfile, tee, splice, vmsplice, alarm, pause,
 *   send, sendto (only if addr is null), setitimer, shutdown, sigaction
 *   (but SIGSYS is forbidden), sigaltstack, sigprocmask, sigreturn,
 *   sigsuspend, umask, mincore, socketpair, ioctl(FIONREAD),
 *   ioctl(FIONBIO), ioctl(FIOCLEX), ioctl(FIONCLEX), fcntl(F_GETFD),
 *   fcntl(F_SETFD), fcntl(F_GETFL), fcntl(F_SETFL), sched_yield,
 *   epoll_create, epoll_create1, epoll_ctl, epoll_wait, epoll_pwait,
 *   epoll_pwait2, clone(CLONE_THREAD), futex, set_robust_list,
 *   get_robust_list, setaffinity, sigpending.
 *
 * - "rpath" (read-only path ops) allows chdir, getcwd, open(O_RDONLY),
 *   openat(O_RDONLY), stat, fstat, lstat, fstatat, access, faccessat,
 *   faccessat2, readlink, readlinkat, statfs, fstatfs.
 *
 * - "wpath" (write path ops) allows getcwd, open(O_WRONLY),
 *   openat(O_WRONLY), stat, fstat, lstat, fstatat, access, faccessat,
 *   faccessat2, readlink, readlinkat, chmod, fchmod, fchmodat.
 *
 * - "cpath" (create path ops) allows open(O_CREAT), openat(O_CREAT),
 *   rename, renameat, renameat2, link, linkat, symlink, symlinkat,
 *   unlink, rmdir, unlinkat, mkdir, mkdirat.
 *
 * - "dpath" (create special path ops) allows mknod, mknodat, mkfifo.
 *
 * - "flock" allows flock, fcntl(F_GETLK), fcntl(F_SETLK),
 *   fcntl(F_SETLKW).
 *
 * - "tty" allows ioctl(TIOCGWINSZ), ioctl(TCGETS), ioctl(TCSETS),
 *   ioctl(TCSETSW), ioctl(TCSETSF).
 *
 * - "recvfd" allows recvmsg and recvmmsg.
 *
 * - "recvfd" allows sendmsg and sendmmsg.
 *
 * - "fattr" allows chmod, fchmod, fchmodat, utime, utimes, futimens,
 *   utimensat.
 *
 * - "inet" allows socket(AF_INET), listen, bind, connect, accept,
 *   accept4, getpeername, getsockname, setsockopt, getsockopt, sendto.
 *
 * - "anet" allows socket(AF_INET), listen, bind, accept,
 *   accept4, getpeername, getsockname, setsockopt, getsockopt, sendto.
 *
 * - "unix" allows socket(AF_UNIX), listen, bind, connect, accept,
 *   accept4, getpeername, getsockname, setsockopt, getsockopt.
 *
 * - "dns" allows socket(AF_INET), sendto, recvfrom, connect.
 *
 * - "proc" allows fork, vfork, clone, kill, tgkill, getpriority,
 *   setpriority, prlimit, setrlimit, setpgid, setsid.
 *
 * - "id" allows setuid, setreuid, setresuid, setgid, setregid,
 *   setresgid, setgroups, prlimit, setrlimit, getpriority, setpriority,
 *   setfsuid, setfsgid.
 *
 * - "settime" allows settimeofday and clock_adjtime.
 *
 * - "exec" allows execve, execveat. Note that `exec` alone might not be
 *   enough by itself to let your executable be executed. For dynamic,
 *   interpreted, and ape binaries, you'll usually want `rpath` and
 *   `prot_exec` too. With APE it's possible to work around this
 *   requirement, by "assimilating" your binaries beforehand. See the
 *   assimilate.com program and `--assimilate` flag which can be used to
 *   turn APE binaries into static native binaries.
 *
 * - "prot_exec" allows mmap(PROT_EXEC) and mprotect(PROT_EXEC). This is
 *   needed to (1) code morph mutexes in __enable_threads(), and it's
 *   needed to (2) launch non-static or non-native executables, e.g.
 *   non-assimilated APE binaries, or dynamic-linked executables.
 *
 * - "unveil" allows unveil() to be called, as well as the underlying
 *   landlock_create_ruleset, landlock_add_rule, landlock_restrict_self
 *   calls on Linux.
 *
 * - "vminfo" OpenBSD defines this for programs like `top`. On Linux,
 *   this is a placeholder group that lets tools like pledge.com check
 *   `__promises` and automatically unveil() a subset of files top would
 *   need, e.g. /proc/stat, /proc/meminfo.
 *
 * - "tmppath" allows unlink, unlinkat, and lstat. This is mostly a
 *   placeholder group for pledge.com, which reads the `__promises`
 *   global to determine if /tmp and $TMPPATH should be unveiled.
 *
 * `execpromises` only matters if "exec" is specified in `promises`. In
 * that case, this specifies the promises that'll apply once execve()
 * happens. If this is NULL then the default is used, which is
 * unrestricted. OpenBSD allows child processes to escape the sandbox
 * (so a pledged OpenSSH server process can do things like spawn a root
 * shell). Linux however requires monotonically decreasing privileges.
 * This function will will perform some validation on Linux to make sure
 * that `execpromises` is a subset of `promises`. Your libc wrapper for
 * execve() will then apply its SECCOMP BPF filter later. Since Linux
 * has to do this before calling sys_execve(), the executed process will
 * be weakened to have execute permissions too.
 *
 * `__pledge_mode` is available to improve the experience of pledge() on
 * Linux. It should specify one of the following penalties:
 *
 * - `PLEDGE_PENALTY_KILL_THREAD` causes the violating thread to be
 *   killed. This is the default on Linux. It's effectively the same as
 *   killing the process, since redbean has no threads. The termination
 *   signal can't be caught and will be either `SIGSYS` or `SIGABRT`.
 *   Consider enabling stderr logging below so you'll know why your
 *   program failed. Otherwise check the system log.
 *
 * - `PLEDGE_PENALTY_KILL_PROCESS` causes the process and all its
 *   threads to be killed. This is always the case on OpenBSD.
 *
 * - `PLEDGE_PENALTY_RETURN_EPERM` causes system calls to just return an
 *   `EPERM` error instead of killing. This is a gentler solution that
 *   allows code to display a friendly warning. Please note this may
 *   lead to weird behaviors if the software being sandboxed is lazy
 *   about checking error results.
 *
 * `mode` may optionally bitwise or the following flags:
 *
 * - `PLEDGE_STDERR_LOGGING` enables friendly error message logging
 *   letting you know which promises are needed whenever violations
 *   occur. Without this, violations will be logged to `dmesg` on Linux
 *   if the penalty is to kill the process. You would then need to
 *   manually look up the system call number and then cross reference it
 *   with the cosmopolitan libc pledge() documentation. You can also use
 *   `strace -ff` which is easier. This is ignored OpenBSD, which
 *   already has a good system log. Turning on stderr logging (which
 *   uses SECCOMP trapping) also means that the `WTERMSIG()` on your
 *   killed processes will always be `SIGABRT` on both Linux and
 *   OpenBSD. Otherwise, Linux prefers to raise `SIGSYS`. Enabling this
 *   option might not be a good idea if you're pledging `exec` because
 *   subprocesses can't inherit the `SIGSYS` handler this installs.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ENOSYS if `pledge(0, 0)` was used and security is not possible
 * @raise EINVAL if `execpromises` on Linux isn't a subset of `promises`
 * @raise EINVAL if `promises` allows exec and `execpromises` is null
 * @vforksafe
 */
int pledge(const char *promises, const char *execpromises) {
  int e, rc;
  unsigned long ipromises, iexecpromises;
  if (!promises) {
    // OpenBSD says NULL argument means it doesn't change, i.e.
    // pledge(0,0) on OpenBSD does nothing. The Cosmopolitan Libc
    // implementation defines pledge(0,0) as a no-op feature check.
    // Cosmo pledge() is currently implemented to succeed silently if
    // the necessary kernel features aren't supported by the host. Apps
    // may use pledge(0,0) to perform a support check, to determine if
    // pledge() will be able to impose the restrictions it advertises
    // within the host environment.
    if (execpromises) return einval();
    if (IsGenuineBlink()) return enosys();
    if (IsOpenbsd()) return sys_pledge(0, 0);
    if (!IsLinux()) return enosys();
    rc = sys_prctl(PR_GET_SECCOMP, 0, 0, 0, 0);
    if (rc == 0 || rc == 2) return 0;  // 2 means we're already filtered
    unassert(rc < 0);
    errno = -rc;
    return -1;
  } else if (!IsTiny() && IsGenuineBlink()) {
    rc = 0;  // blink doesn't support seccomp; avoid noisy log warnings
  } else if (!ParsePromises(promises, &ipromises, __promises) &&
             !ParsePromises(execpromises, &iexecpromises, __execpromises)) {
    if (IsLinux()) {
      // copy exec and execnative from promises to execpromises
      iexecpromises = ~(~iexecpromises | (~ipromises & (1ul << PROMISE_EXEC)));
      // if bits are missing in execpromises that exist in promises
      // then execpromises wouldn't be a monotonic access reduction
      // this check only matters when exec / execnative are allowed
      bool notsubset = ((ipromises & ~iexecpromises) &&
                        (~ipromises & (1ul << PROMISE_EXEC)));
      if (notsubset && execpromises) {
        STRACE("execpromises must be a subset of promises");
        rc = einval();
      } else {
        if (notsubset) iexecpromises = ipromises;
        rc = sys_pledge_linux(ipromises, __pledge_mode);
        if (rc > -4096u) errno = -rc, rc = -1;
      }
    } else {
      e = errno;
      rc = sys_pledge(promises, execpromises);
      if (rc && errno == ENOSYS) {
        errno = e;
        rc = 0;
      }
    }
    if (!rc && !__vforked &&
        (IsOpenbsd() || (IsLinux() && getpid() == gettid()))) {
      __promises = ipromises;
      __execpromises = iexecpromises;
    }
  } else {
    rc = einval();
  }
  STRACE("pledge(%#s, %#s) → %d% m", promises, execpromises, rc);
  return rc;
}
