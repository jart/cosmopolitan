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
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/runtime/stack.h"
#include "libc/sock/internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"
#include "libc/vga/vga.internal.h"

static size_t SumIovecBytes(const struct iovec *iov, int iovlen) {
  size_t count = 0;
  for (int i = 0; i < iovlen; ++i)
    if (ckd_add(&count, count, iov[i].iov_len))
      count = SIZE_MAX;
  return count;
}

static ssize_t writev_impl(int fd, const struct iovec *iov, int iovlen) {
  if (fd < 0)
    return ebadf();
  if (iovlen < 0)
    return einval();
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip)
    return ebadf();  // posix specifies this when not open()'d for writing

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

  if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    if (iovlen == 1) {
      return sys_write(fd, iov[0].iov_base, iov[0].iov_len);
    } else {
      return sys_writev(fd, iov, iovlen);
    }
  } else if (fd >= g_fds.n) {
    return ebadf();
  } else if (IsMetal()) {
    return sys_writev_metal(g_fds.p + fd, iov, iovlen);
  } else if (IsWindows()) {
    return sys_writev_nt(fd, iov, iovlen);
  } else {
    return enosys();
  }
}

/**
 * Writes data from multiple buffers.
 *
 * This is the same thing as write() except it has multiple buffers.
 * This yields a performance boost in situations where it'd be expensive
 * to stitch data together using memcpy() or issuing multiple syscalls.
 * This wrapper is implemented so that writev() calls where iovlen<2 may
 * be passed to the kernel as write() instead. This yields a 100 cycle
 * performance boost in the case of a single small iovec.
 *
 * Please note that it's not an error for a short write to happen. This
 * can happen in the kernel if EINTR happens after some of the write has
 * been committed. It can also happen if we need to polyfill this system
 * call using write().
 *
 * It's possible for file write request to be partially completed. For
 * example, if the sum of `iov` lengths exceeds 0x7ffff000 then bytes
 * beyond that will be ignored. This is a Linux behavior that Cosmo
 * polyfills across platforms.
 *
 * @return number of bytes actually handed off, or -1 w/ errno
 * @cancelationpoint
 * @restartable
 */
ssize_t writev(int fd, const struct iovec *iov, int iovlen) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;
  rc = writev_impl(fd, iov, iovlen);
  END_CANCELATION_POINT;
  STRACE("writev(%d, %s, %d) → %'ld% m", fd,
         DescribeIovec(rc != -1 ? rc : -2, iov, iovlen), iovlen, rc);
  return rc;
}
