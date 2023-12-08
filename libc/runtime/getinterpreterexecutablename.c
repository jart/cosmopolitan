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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns path of executable interpreter.
 *
 * Unlike `program_executable_name` which is designed to figure out the
 * absolute path of the first argument passed to `execve()`, what we do
 * here is probe things like `/proc` and `sysctl()` to figure out if we
 * were launched by something like `ape-loader`, and then we return its
 * path. If we can't determine that path, possibly because we're on XNU
 * or OpenBSD, then we return -1 with an error code.
 *
 * @param p receives utf8 output
 * @param n is byte size of res buffer
 * @return p on success or null w/ errno if out of buf or error
 * @see program_invocation_short_name
 * @see program_invocation_name
 * @see program_executable_name
 */
char *GetInterpreterExecutableName(char *p, size_t n) {
  int e;
  int cmd[4];
  ssize_t rc;
  e = errno;
  if (n < 2) {
    errno = ENAMETOOLONG;
  } else if (IsWindows() || IsXnu()) {
    // TODO(jart): Does XNU guarantee argv[0] is legit?
    //             Otherwise we should return NULL.
    //             What about OpenBSD?
    if (strlen(GetProgramExecutableName()) < n) {
      strcpy(p, GetProgramExecutableName());
      return p;
    }
    errno = ENAMETOOLONG;
  } else if ((rc = sys_readlinkat(AT_FDCWD, "/proc/self/exe", p, n - 1)) > 0) {
    p[rc] = 0;
    return p;
  } else if ((rc = sys_readlinkat(AT_FDCWD, "/proc/curproc/file", p, n - 1)) >
             0) {
    errno = e;
    p[rc] = 0;
    return p;
  } else if (IsFreebsd() || IsNetbsd()) {
    cmd[0] = 1;         // CTL_KERN
    cmd[1] = 14;        // KERN_PROC
    if (IsFreebsd()) {  //
      cmd[2] = 12;      // KERN_PROC_PATHNAME
    } else {            //
      cmd[2] = 5;       // KERN_PROC_PATHNAME
    }                   //
    cmd[3] = -1;        // current process
    if (sys_sysctl(cmd, ARRAYLEN(cmd), p, &n, 0, 0) != -1) {
      errno = e;
      return p;
    }
  }
  return 0;
}
