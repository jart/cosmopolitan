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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
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
 * @param advice can be MADV_SEQUENTIAL, MADV_RANDOM, etc.
 * @return 0 on success, or -1 w/ errno
 * @raise EBADF if `fd` isn't a valid file descriptor
 * @raise ESPIPE if `fd` refers to a pipe
 * @raise EINVAL if `advice` was invalid
 * @raise ENOSYS on XNU and OpenBSD
 */
int fadvise(int fd, uint64_t offset, uint64_t len, int advice) {
  int rc;
  if (IsLinux()) {
    rc = sys_fadvise(fd, offset, len, advice);
  } else if (IsFreebsd() || IsNetbsd()) {
    if (IsFreebsd()) {
      rc = sys_fadvise(fd, offset, len, advice);
    } else {
      rc = sys_fadvise_netbsd(fd, offset, offset, len, advice);
    }
    npassert(rc >= 0);
    if (rc) {
      errno = rc;
      rc = -1;
    }
  } else if (IsWindows()) {
    rc = sys_fadvise_nt(fd, offset, len, advice);
  } else {
    rc = enosys();
  }
  STRACE("fadvise(%d, %'lu, %'lu, %d) → %d% m", fd, offset, len, advice, rc);
  return rc;
}

__weak_reference(fadvise, fadvise64);
