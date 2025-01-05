/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/pledge.h"
#include "libc/calls/pledge.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/promises.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Replaces current process with program.
 *
 * Your `prog` may be an actually portable executable or a platform
 * native binary (e.g. ELF, Mach-O, PE). On UNIX systems, your execve
 * implementation will try to find where the `ape` interpreter program
 * is installed on your system. The preferred location is `/usr/bin/ape`
 * except on Apple Silicon where it's `/usr/local/bin/ape`. The $TMPDIR
 * and $HOME locations that the APE shell script extracts the versioned
 * ape binaries to will also be checked as a fallback path. Finally, if
 * `prog` isn't an executable in any recognizable format, cosmo assumes
 * it's a bourne shell script and launches it under /bin/sh.
 *
 * The signal mask and pending signals are inherited by the new process.
 * Note the NetBSD kernel has a bug where pending signals are cleared.
 *
 * File descriptors that haven't been marked `O_CLOEXEC` through various
 * devices such as open() and fcntl() will be inherited by the executed
 * subprocess. The current file position of the duplicated descriptors
 * is shared across processes. On Windows, `prog` needs to be built by
 * cosmocc in order to properly inherit file descriptors. If a program
 * compiled by MSVC or Cygwin is launched instead, then only the stdio
 * file descriptors can be passed along.
 *
 * On Windows, `argv` and `envp` can't contain binary strings. They need
 * to be valid UTF-8 in order to round-trip the WIN32 API, without being
 * corrupted.
 *
 * On Windows, cosmo execve uses parent spoofing to implement the UNIX
 * behavior of replacing the current process. Since POSIX.1 also needs
 * us to maintain the same PID number too, the _COSMO_PID environemnt
 * variable is passed to the child process which specifies a spoofed
 * PID. Whatever is in that variable will be reported by getpid() and
 * other cosmo processes will be able to send signals to the process
 * using that pid, via kill(). These synthetic PIDs which are only
 * created by execve could potentially overlap with OS assignments if
 * Windows recycles them. Cosmo avoids that by tracking handles of
 * subprocesses. Each process has its own process manager thread, to
 * associate pids with win32 handles, and execve will tell the parent
 * process its new handle when it changes. However it's not perfect.
 * There's still situations where processes created by execve() can
 * cause surprising things to happen. For an alternative, consider
 * posix_spawn() which is fastest and awesomest across all OSes.
 *
 * On Windows, support is currently not implemented for inheriting
 * setitimer() and alarm() into an executed process.
 *
 * On Windows, support is currently not implemented for inheriting
 * getrusage() statistics into an executed process.
 *
 * The executed process will share the same terminal and current
 * directory.
 *
 * @param program will not be PATH searched, see commandv()
 * @param argv[0] is the name of the program to run
 * @param argv[1,n-2] optionally specify program arguments
 * @param argv[n-1] is NULL
 * @param envp[0,n-2] specifies "foo=bar" environment variables
 * @param envp[n-1] is NULL
 * @return doesn't return, or -1 w/ errno
 * @raise ETXTBSY if another process has `prog` open in write mode
 * @raise ENOEXEC if file is executable but not a valid format
 * @raise ENOMEM if remaining stack memory is insufficient
 * @raise EACCES if execute permission was denied
 * @asyncsignalsafe
 * @vforksafe
 */
int execve(const char *prog, char *const argv[], char *const envp[]) {
  int rc;
  struct ZiposUri uri;
  if (!prog || !argv || !envp) {
    rc = efault();
  } else {
    STRACE("execve(%#s, %s, %s)", prog, DescribeStringList(argv),
           DescribeStringList(envp));
    rc = 0;
    if (IsLinux() && __execpromises && _weaken(sys_pledge_linux)) {
      rc = _weaken(sys_pledge_linux)(__execpromises, __pledge_mode);
    }
    if (!rc) {
      if (0 && _weaken(__zipos_parseuri) &&
          (_weaken(__zipos_parseuri)(prog, &uri) != -1)) {
        rc = _weaken(__zipos_open)(&uri, O_RDONLY | O_CLOEXEC);
        if (rc != -1) {
          const int zipFD = rc;
          strace_enabled(-1);
          rc = fexecve(zipFD, argv, envp);
          close(zipFD);
          strace_enabled(+1);
        }
      } else if (!IsWindows()) {
        rc = sys_execve(prog, argv, envp);
      } else {
        rc = sys_execve_nt(prog, argv, envp);
      }
    }
  }
  STRACE("execve(%#s) failed %d% m", prog, rc);
  return rc;
}
