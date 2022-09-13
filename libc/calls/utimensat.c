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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Sets atime/mtime on file, the modern way.
 *
 * This is two functions in one. If `path` is null then this function
 * becomes the same as `futimens(dirfd, ts)`.
 *
 * XNU and RHEL5 only have microsecond accuracy and there's no `dirfd`
 * relative support.
 *
 * @param dirfd should AT_FDCWD
 * @param ts is atime/mtime, or null for current time
 * @param flags can have AT_SYMLINK_NOFOLLOW
 * @raise ENOSYS on RHEL5 if path is NULL
 * @raise EINVAL if flags had unrecognized bits
 * @raise EINVAL if path is NULL and flags has AT_SYMLINK_NOFOLLOW
 * @raise EBADF if dirfd isn't a valid fd or AT_FDCWD
 * @raise EFAULT if path or ts memory was invalid
 * @raise EROFS if file system is read-only
 * @raise ENAMETOOLONG
 * @raise EPERM
 * @see futimens()
 * @asyncsignalsafe
 */
int utimensat(int dirfd, const char *path, const struct timespec ts[2],
              int flags) {
  int rc;
  if (IsAsan() && ((dirfd == AT_FDCWD && !__asan_is_valid(path, 1)) ||
                   (ts && (!__asan_is_valid_timespec(ts + 0) ||
                           !__asan_is_valid_timespec(ts + 1))))) {
    rc = efault();  // bad memory
  } else if ((flags & ~AT_SYMLINK_NOFOLLOW)) {
    rc = einval();  // unsupported flag
  } else if (!path && flags) {
    rc = einval();  // futimens() doesn't take flags
  } else if ((path || __isfdkind(dirfd, kFdZip)) && _weaken(__zipos_notat) &&
             (rc = __zipos_notat(dirfd, path)) == -1) {
    STRACE("zipos utimensat not supported yet");
  } else if (!IsWindows()) {
    rc = sys_utimensat(dirfd, path, ts, flags);
  } else {
    rc = sys_utimensat_nt(dirfd, path, ts, flags);
  }
  if (ts) {
    STRACE("utimensat(%s, %#s, {{%,ld, %,ld}, {%,ld, %,ld}}, %#b) → %d% m",
           DescribeDirfd(dirfd), path, ts[0].tv_sec, ts[0].tv_nsec,
           ts[1].tv_sec, ts[1].tv_nsec, flags, rc);
  } else {
    STRACE("utimensat(%s, %#s, 0, %#b) → %d% m", DescribeDirfd(dirfd), path,
           flags, rc);
  }
  return rc;
}
