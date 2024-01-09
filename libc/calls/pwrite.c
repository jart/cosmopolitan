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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Writes to file at offset.
 *
 * This function never changes the current position of `fd`.
 *
 * @param fd is something open()'d earlier, noting pipes might not work
 * @param buf is copied from, cf. copy_file_range(), sendfile(), etc.
 * @param size in range [1..0x7ffff000] is reasonable
 * @param offset is bytes from start of file at which write begins,
 *     which can exceed or overlap the end of file, in which case your
 *     file will be extended
 * @return [1..size] bytes on success, or -1 w/ errno; noting zero is
 *     impossible unless size was passed as zero to do an error check
 * @see pread(), write()
 * @cancelationpoint
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t pwrite(int fd, const void *buf, size_t size, int64_t offset) {
  ssize_t rc;
  size_t wrote;
  BEGIN_CANCELATION_POINT;

  if (offset < 0) {
    rc = einval();
  } else if (fd == -1) {
    rc = ebadf();
  } else if (IsAsan() && !__asan_is_valid(buf, size)) {
    rc = efault();
  } else if (__isfdkind(fd, kFdZip)) {
    rc = ebadf();
  } else if (!IsWindows()) {
    rc = sys_pwrite(fd, buf, size, offset, offset);
  } else if (__isfdkind(fd, kFdSocket)) {
    rc = espipe();
  } else if (__isfdkind(fd, kFdFile) || __isfdkind(fd, kFdDevNull)) {
    rc = sys_write_nt(fd, (struct iovec[]){{(void *)buf, size}}, 1, offset);
  } else {
    return ebadf();
  }
  if (rc != -1) {
    wrote = (size_t)rc;
    if (!wrote) {
      npassert(size == 0);
    } else {
      npassert(wrote <= size);
    }
  }

  END_CANCELATION_POINT;
  DATATRACE("pwrite(%d, %#.*hhs%s, %'zu, %'zd) → %'zd% m", fd,
            MAX(0, MIN(40, rc)), buf, rc > 40 ? "..." : "", size, offset, rc);
  return rc;
}

__weak_reference(pwrite, pwrite64);
