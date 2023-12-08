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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

int sys_fadvise_netbsd(int, int, int64_t, int64_t, int) asm("sys_fadvise");

/**
 * Drops hints to O/S about intended I/O behavior.
 *
 * It makes a huge difference. For example, when copying a large file,
 * it can stop the system from persisting GBs of useless memory content.
 *
 * @param len 0 means until end of file
 * @param advice can be POSIX_FADV_SEQUENTIAL, POSIX_FADV_RANDOM, etc.
 * @return 0 on success, or errno on error
 * @raise EBADF if `fd` isn't a valid file descriptor
 * @raise EINVAL if `advice` is invalid or `len` is huge
 * @raise ESPIPE if `fd` refers to a pipe
 * @raise ENOTSUP if `fd` is a /zip file
 * @raise ENOSYS on XNU and OpenBSD
 * @returnserrno
 */
errno_t posix_fadvise(int fd, int64_t offset, int64_t len, int advice) {
  int rc, e = errno;
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotsup();
  } else if (IsLinux()) {
    rc = sys_fadvise(fd, offset, len, advice);
  } else if (IsFreebsd()) {
    rc = sys_fadvise(fd, offset, len, advice);
    unassert(rc >= 0);
  } else if (IsNetbsd()) {
    rc = sys_fadvise_netbsd(fd, offset, offset, len, advice);
    unassert(rc >= 0);
  } else if (IsWindows()) {
    rc = sys_fadvise_nt(fd, offset, len, advice);
  } else {
    rc = enosys();
  }
  if (rc == -1) {
    rc = errno;
    errno = e;
  }
  STRACE("posix_fadvise(%d, %'lu, %'lu, %d) → %s", fd, offset, len, advice,
         !rc ? "0" : _strerrno(rc));
  return rc;
}
