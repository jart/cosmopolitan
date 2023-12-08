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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Replaces current process with program.
 *
 * On Windows, `argv` and `envp` can't contain binary strings. They need
 * to be valid UTF-8 in order to round-trip the WIN32 API, without being
 * corrupted.
 *
 * On Windows, only file descriptors 0, 1 and 2 can be passed to a child
 * process in such a way that allows them to be automatically discovered
 * when the child process initializes. Cosmpolitan currently treats your
 * other file descriptors as implicitly O_CLOEXEC.
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
  if (!prog || !argv || !envp ||
      (IsAsan() && (!__asan_is_valid_str(prog) ||      //
                    !__asan_is_valid_strlist(argv) ||  //
                    !__asan_is_valid_strlist(envp)))) {
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
