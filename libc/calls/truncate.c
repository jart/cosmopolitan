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
#include "libc/calls/cp.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes size of file.
 *
 * If the file size is increased, the extended area shall appear as if
 * it were zero-filled. If your file size is decreased, the extra data
 * shall be lost.
 *
 * Some operating systems implement an optimization, where `length` is
 * treated as a logical size and the requested physical space won't be
 * allocated until non-zero values get written into it. Our tests show
 * this happens on Linux (usually with 4096 byte granularity), FreeBSD
 * (which favors 512-byte granularity), and MacOS (prefers 4096 bytes)
 * however Windows, OpenBSD, and NetBSD always reserve physical space.
 * This may be inspected using stat() then consulting stat::st_blocks.
 *
 * @param path is name of file that shall be resized
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `length` is negative
 * @raise EINTR if signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EFBIG or EINVAL if `length` is too huge
 * @raise EFAULT if `path` points to invalid memory
 * @raise EACCES if we don't have permission to search a component of `path`
 * @raise ENOTDIR if a directory component in `path` exists as non-directory
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise ELOOP if a loop was detected resolving components of `path`
 * @raise EROFS if `path` is on a read-only filesystem (e.g. zip)
 * @raise ENOENT if `path` doesn't exist or is an empty string
 * @raise ETXTBSY if `path` is an executable being executed
 * @raise ENOSYS on bare metal
 * @cancelationpoint
 * @see ftruncate()
 */
int truncate(const char *path, int64_t length) {
  int rc;
  struct ZiposUri zipname;
  BEGIN_CANCELATION_POINT;

  if (IsMetal()) {
    rc = enosys();
  } else if (!path || (IsAsan() && !__asan_is_valid_str(path))) {
    rc = efault();
  } else if (_weaken(__zipos_parseuri) &&
             _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    rc = erofs();
  } else if (!IsWindows()) {
    rc = sys_truncate(path, length, length);
    if (IsNetbsd() && rc == -1 && errno == ENOSPC) {
      errno = EFBIG;  // POSIX doesn't specify ENOSPC for truncate()
    }
  } else {
    rc = sys_truncate_nt(path, length);
  }

  END_CANCELATION_POINT;
  STRACE("truncate(%#s, %'ld) → %d% m", path, length, rc);
  return rc;
}
