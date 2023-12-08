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
#include "libc/calls/internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static struct Splice {
  _Atomic(uint32_t) once;
  bool ok;
} g_splice;

static bool HasSplice(void) {
  int e;
  bool ok;
  e = errno;
  if (IsLinux()) {
    // Our testing indicates splice() doesn't work as documneted on
    // RHEL5 and RHEL7 so let's require a modern kernel to be safe.
    // We choose close_range() for this since it's listed by pledge
    ok = sys_close_range(-1, -2, 0) == -1 && errno == EINVAL;
  } else {
    ok = false;
  }
  errno = e;
  return ok;
}

static void splice_init(void) {
  g_splice.ok = HasSplice();
}

/**
 * Transfers data to/from pipe.
 *
 * @param flags can have SPLICE_F_{MOVE,NONBLOCK,MORE,GIFT}
 * @param opt_in_out_inoffset may be specified if `infd` isn't a pipe and is
 *     used as both an input and output parameter for pread() behavior
 * @param opt_in_out_outoffset may be specified if `outfd` isn't a pipe and is
 *     used as both an input and output parameter for pwrite() behavior
 * @return number of bytes transferred, 0 on input end, or -1 w/ errno
 * @raise EBADF if `infd` or `outfd` aren't open files or append-only
 * @raise ENOTSUP if `infd` or `outfd` is a zip file descriptor
 * @raise ESPIPE if an offset arg was specified for a pipe fd
 * @raise EINVAL if offset was given for non-seekable device
 * @raise EINVAL if file system doesn't support splice()
 * @raise EFAULT if one of the pointers memory is bad
 * @raise EINVAL if `flags` is invalid
 * @raise ENOSYS if not Linux 5.9+
 * @see copy_file_range() for file ↔ file
 * @see sendfile() for seekable → socket
 */
ssize_t splice(int infd, int64_t *opt_in_out_inoffset, int outfd,
               int64_t *opt_in_out_outoffset, size_t uptobytes,
               uint32_t flags) {
  ssize_t rc;
  cosmo_once(&g_splice.once, splice_init);
  if (!g_splice.ok) {
    rc = enosys();
  } else if (IsAsan() && ((opt_in_out_inoffset &&
                           !__asan_is_valid(opt_in_out_inoffset, 8)) ||
                          (opt_in_out_outoffset &&
                           !__asan_is_valid(opt_in_out_outoffset, 8)))) {
    rc = efault();
  } else if (__isfdkind(infd, kFdZip) || __isfdkind(outfd, kFdZip)) {
    rc = enotsup();
  } else {
    rc = sys_splice(infd, opt_in_out_inoffset, outfd, opt_in_out_outoffset,
                    uptobytes, flags);
  }
  STRACE("splice(%d, %s, %d, %s, %'zu, %#x) → %'ld% m", infd,
         DescribeInOutInt64(rc, opt_in_out_inoffset), outfd,
         DescribeInOutInt64(rc, opt_in_out_outoffset), uptobytes, flags);
  return rc;
}
