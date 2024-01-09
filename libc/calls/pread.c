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
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads from file at offset.
 *
 * This function never changes the current position of `fd`.
 *
 * @param fd is something open()'d earlier, noting pipes might not work
 * @param buf is copied into, cf. copy_file_range(), sendfile(), etc.
 * @param size in range [1..0x7ffff000] is reasonable
 * @param offset is bytes from start of file at which read begins
 * @return [1..size] bytes on success, 0 on EOF, or -1 w/ errno; with
 *     exception of size==0, in which case return zero means no error
 * @raise ESPIPE if `fd` isn't seekable
 * @raise EINVAL if `offset` is negative
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise EIO if a complicated i/o error happened
 * @raise EINTR if signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @see pwrite(), write()
 * @cancelationpoint
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t pread(int fd, void *buf, size_t size, int64_t offset) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;

  if (offset < 0) {
    rc = einval();
  } else if (fd < 0) {
    rc = ebadf();
  } else if (IsAsan() && !__asan_is_valid(buf, size)) {
    rc = efault();
  } else if (__isfdkind(fd, kFdZip)) {
    rc = _weaken(__zipos_read)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle,
        (struct iovec[]){{buf, size}}, 1, offset);
  } else if (!IsWindows()) {
    rc = sys_pread(fd, buf, size, offset, offset);
  } else if (__isfdkind(fd, kFdSocket)) {
    rc = espipe();
  } else if (__isfdkind(fd, kFdFile) || __isfdkind(fd, kFdDevNull)) {
    rc = sys_read_nt(fd, (struct iovec[]){{buf, size}}, 1, offset);
  } else {
    rc = ebadf();
  }
  npassert(rc == -1 || (size_t)rc <= size);

  END_CANCELATION_POINT;
  DATATRACE("pread(%d, [%#.*hhs%s], %'zu, %'zd) → %'zd% m", fd,
            MAX(0, MIN(40, rc)), buf, rc > 40 ? "..." : "", size, offset, rc);
  return rc;
}

__weak_reference(pread, pread64);
