/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/asan.internal.h"
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Changes last accessed/modified timestamps on file.
 *
 * @param tv is access/modified timestamps, or NULL which means now
 * @return 0 on success, or -1 w/ errno
 * @raise ENOTSUP if `path` is a zip filesystem path
 * @raise EROFS if `path` is on a read-only filesystem
 * @raise EFAULT if `path` or `tv` points to invalid memory
 * @raise EPERM if pledge() is in play without fattr promise
 * @raise ENOENT if `path` doesn't exist or is an empty string
 * @raise EACCES if unveil() is in play and `path` isn't unveiled
 * @raise ELOOP if a loop was detected resolving components of `path`
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EINVAL if `tv` specifies a microseconds value that's out of range
 * @raise EACCES if we don't have permission to search a component of `path`
 * @raise ENOTDIR if a directory component in `path` exists as non-directory
 * @raise ENOSYS on bare metal
 * @asyncsignalsafe
 * @see stat()
 */
int utimes(const char *path, const struct timeval tv[2]) {
  int rc;
  struct ZiposUri zipname;
  if (IsMetal()) {
    rc = enosys();
  } else if (IsAsan() && tv &&
             (!__asan_is_valid_timeval(tv + 0) ||
              !__asan_is_valid_timeval(tv + 1))) {
    rc = efault();
  } else if (_weaken(__zipos_parseuri) &&
             _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    rc = enotsup();
  } else if (!IsWindows()) {
    // we don't modernize utimes() into utimensat() because the
    // latter is poorly supported and utimes() works everywhere
    rc = sys_utimes(path, tv);
  } else {
    rc = sys_utimes_nt(path, tv);
  }
  STRACE("utimes(%#s, {%s, %s}) → %d% m", path, DescribeTimeval(0, tv),
         DescribeTimeval(0, tv ? tv + 1 : 0), rc);
  return rc;
}
