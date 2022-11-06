/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

static struct CopyFileRange {
  pthread_once_t once;
  bool ok;
} g_copy_file_range;

static bool HasCopyFileRange(void) {
  bool ok;
  int e, rc;
  e = errno;
  BLOCK_CANCELLATIONS;
  if (IsLinux()) {
    // We modernize our detection by a few years for simplicity.
    // This system call is chosen since it's listed by pledge().
    // https://www.cygwin.com/bugzilla/show_bug.cgi?id=26338
    ok = sys_close_range(-1, -2, 0) == -1 && errno == EINVAL;
  } else if (IsFreebsd()) {
    ok = sys_copy_file_range(-1, 0, -1, 0, 0, 0) == -1 && errno == EBADF;
  } else {
    ok = false;
  }
  ALLOW_CANCELLATIONS;
  errno = e;
  return ok;
}

static void copy_file_range_init(void) {
  g_copy_file_range.ok = HasCopyFileRange();
}

/**
 * Transfers data between files.
 *
 * If this system call is available (Linux c. 2018 or FreeBSD c. 2021)
 * and the file system supports it (e.g. ext4) and the source and dest
 * files are on the same file system, then this system call shall make
 * copies go about 2x faster.
 *
 * This implementation requires Linux 5.9+ even though the system call
 * was introduced in Linux 4.5. That's to ensure ENOSYS works reliably
 * due to a faulty backport, that happened in RHEL7. FreeBSD detection
 * on the other hand will work fine.
 *
 * @param infd is source file, which should be on same file system
 * @param opt_in_out_inoffset may be specified for pread() behavior
 * @param outfd should be a writable file, but not `O_APPEND`
 * @param opt_in_out_outoffset may be specified for pwrite() behavior
 * @param uptobytes is maximum number of bytes to transfer
 * @param flags is reserved for future use and must be zero
 * @return number of bytes transferred, or -1 w/ errno
 * @raise EXDEV if source and destination are on different filesystems
 * @raise EBADF if `infd` or `outfd` aren't open files or append-only
 * @raise EPERM if `fdout` refers to an immutable file on Linux
 * @raise ENOTSUP if `infd` or `outfd` is a zip file descriptor
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINVAL if ranges overlap or `flags` is non-zero
 * @raise EFBIG if `setrlimit(RLIMIT_FSIZE)` is exceeded
 * @raise EFAULT if one of the pointers memory is bad
 * @raise ERANGE if overflow happens computing ranges
 * @raise ENOSPC if file system has run out of space
 * @raise ETXTBSY if source or dest is a swap file
 * @raise EINTR if a signal was delivered instead
 * @raise EISDIR if source or dest is a directory
 * @raise ENOSYS if not Linux 5.9+ or FreeBSD 13+
 * @raise EIO if a low-level i/o error happens
 * @see sendfile() for seekable → socket
 * @see splice() for fd ↔ pipe
 * @cancellationpoint
 */
ssize_t copy_file_range(int infd, int64_t *opt_in_out_inoffset, int outfd,
                        int64_t *opt_in_out_outoffset, size_t uptobytes,
                        uint32_t flags) {
  ssize_t rc;
  pthread_once(&g_copy_file_range.once, copy_file_range_init);
  BEGIN_CANCELLATION_POINT;

  if (!g_copy_file_range.ok) {
    rc = enosys();
  } else if (IsAsan() && ((opt_in_out_inoffset &&
                           !__asan_is_valid(opt_in_out_inoffset, 8)) ||
                          (opt_in_out_outoffset &&
                           !__asan_is_valid(opt_in_out_outoffset, 8)))) {
    rc = efault();
  } else if (__isfdkind(infd, kFdZip) || __isfdkind(outfd, kFdZip)) {
    rc = enotsup();
  } else {
    rc = sys_copy_file_range(infd, opt_in_out_inoffset, outfd,
                             opt_in_out_outoffset, uptobytes, flags);
  }

  END_CANCELLATION_POINT;
  STRACE("copy_file_range(%d, %s, %d, %s, %'zu, %#x) → %'ld% m", infd,
         DescribeInOutInt64(rc, opt_in_out_inoffset), outfd,
         DescribeInOutInt64(rc, opt_in_out_outoffset), uptobytes, flags);
  return rc;
}
