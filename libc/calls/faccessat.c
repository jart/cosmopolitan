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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Checks if effective user can access path in particular ways.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     file is a relative path, then file is opened relative to dirfd
 * @param path is a filename or directory
 * @param mode can be R_OK, W_OK, X_OK, F_OK
 * @param flags can have AT_EACCESS, AT_SYMLINK_NOFOLLOW
 * @note on Linux flags is only supported on Linux 5.8+
 * @return 0 if ok, or -1 and sets errno
 * @asyncsignalsafe
 */
int faccessat(int dirfd, const char *path, int mode, uint32_t flags) {
  int e, rc;
  if (IsAsan() && !__asan_is_valid(path, 1)) {
    rc = efault();
  } else if (_weaken(__zipos_notat) &&
             _weaken(__zipos_notat)(dirfd, path) == -1) {
    rc = -1; /* TODO(jart): implement me */
  } else if (!IsWindows()) {
    e = errno;
    if (!flags) goto NoFlags;
    if ((rc = sys_faccessat2(dirfd, path, mode, flags)) == -1) {
      if (errno == ENOSYS) {
        errno = e;
      NoFlags:
        rc = sys_faccessat(dirfd, path, mode, flags);
      }
    }
  } else {
    rc = sys_faccessat_nt(dirfd, path, mode, flags);
  }
  STRACE("faccessat(%s, %#s, %#o, %#x) → %d% m", DescribeDirfd(dirfd), path,
         mode, flags, rc);
  return rc;
}
