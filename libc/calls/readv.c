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
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sock/internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"

static size_t SumIovecBytes(const struct iovec *iov, int iovlen) {
  size_t count = 0;
  for (int i = 0; i < iovlen; ++i)
    if (ckd_add(&count, count, iov[i].iov_len))
      count = SIZE_MAX;
  return count;
}

static ssize_t readv_impl(int fd, const struct iovec *iov, int iovlen) {
  if (fd < 0)
    return ebadf();
  if (iovlen < 0)
    return einval();

  // XNU and BSDs will EINVAL if requested bytes exceeds INT_MAX
  // this is inconsistent with Linux which ignores huge requests
  if (!IsLinux()) {
    size_t sum, remain = 0x7ffff000;
    if ((sum = SumIovecBytes(iov, iovlen)) > remain) {
      struct iovec *iov2;
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Walloca-larger-than="
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
      iov2 = alloca(iovlen * sizeof(struct iovec));
      CheckLargeStackAllocation(iov2, iovlen * sizeof(struct iovec));
#pragma GCC pop_options
      for (int i = 0; i < iovlen; ++i) {
        iov2[i] = iov[i];
        if (remain >= iov2[i].iov_len) {
          remain -= iov2[i].iov_len;
        } else {
          iov2[i].iov_len = remain;
          remain = 0;
        }
      }
      iov = iov2;
    }
  }

  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    return _weaken(__zipos_read)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, iov, iovlen, -1);
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    if (iovlen == 1) {
      return sys_read(fd, iov[0].iov_base, iov[0].iov_len);
    } else {
      return sys_readv(fd, iov, iovlen);
    }
  } else if (fd >= g_fds.n) {
    return ebadf();
  } else if (IsMetal()) {
    return sys_readv_metal(fd, iov, iovlen);
  } else if (IsWindows()) {
    return sys_readv_nt(fd, iov, iovlen);
  } else {
    return enosys();
  }
}

/**
 * Reads data to multiple buffers.
 *
 * This is the same thing as read() except it has multiple buffers.
 * This yields a performance boost in situations where it'd be expensive
 * to stitch data together using memcpy() or issuing multiple syscalls.
 * This wrapper is implemented so that readv() calls where iovlen<2 may
 * be passed to the kernel as read() instead. This yields a 100 cycle
 * performance boost in the case of a single small iovec.
 *
 * It's possible for file write request to be partially completed. For
 * example, if the sum of `iov` lengths exceeds 0x7ffff000 then bytes
 * beyond that will be ignored. This is a Linux behavior that Cosmo
 * polyfills across platforms.
 *
 * @return number of bytes actually read, or -1 w/ errno
 * @cancelationpoint
 * @restartable
 */
ssize_t readv(int fd, const struct iovec *iov, int iovlen) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;
  rc = readv_impl(fd, iov, iovlen);
  END_CANCELATION_POINT;
  STRACE("readv(%d, [%s], %d) → %'ld% m", fd, DescribeIovec(rc, iov, iovlen),
         iovlen, rc);
  return rc;
}
