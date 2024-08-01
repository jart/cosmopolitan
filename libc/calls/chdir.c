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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets current directory.
 *
 * The current directory is shared by all threads in a process. This
 * does not update the `PWD` environment variable.
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ELOOP if a loop was detected resolving components of `path`
 * @raise EACCES if search permission was denied on directory
 * @raise ENOTDIR if component of `path` isn't a directory
 * @raise ENOMEM if insufficient memory was available
 * @raise EFAULT if `path` points to invalid memory
 * @raise ENOTSUP if `path` is a `/zip/...` file
 * @raise ENAMETOOLONG if `path` was too long
 * @raise ENOENT if `path` doesn't exist
 * @raise EIO if an i/o error happened
 * @asyncsignalsafe
 * @see fchdir()
 */
int chdir(const char *path) {
  int rc;
  struct ZiposUri zipname;
  if (_weaken(__get_tmpdir)) {
    _weaken(__get_tmpdir)();
  }
  if (_weaken(GetAddr2linePath)) {
    _weaken(GetAddr2linePath)();
  }
  if (_weaken(GetProgramExecutableName)) {
    _weaken(GetProgramExecutableName)();
  }
  if (!path) {
    rc = efault();
  } else if (_weaken(__zipos_parseuri) &&
             _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    rc = enotsup();
  } else if (!IsWindows()) {
    rc = sys_chdir(path);
  } else {
    rc = sys_chdir_nt(path);
  }
  STRACE("%s(%#s) → %d% m", "chdir", path, rc);
  return rc;
}
