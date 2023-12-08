/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads symbolic link.
 *
 * This does *not* nul-terminate the buffer.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     file is a relative path, then file is opened relative to dirfd
 * @param path must be a symbolic link pathname
 * @param buf will receive symbolic link contents, and won't be modified
 *     unless the function succeeds (with the exception of no-malloc nt)
 *     and this buffer will *not* be nul-terminated
 * @return number of bytes written to buf, or -1 w/ errno; if the
 *     return is equal to bufsiz then truncation may have occurred
 * @raise EINVAL if path isn't a symbolic link
 * @raise ENOENT if `path` didn't exist
 * @raise ENOTDIR if parent component existed that's not a directory
 * @raise ENOTDIR if base component ends with slash and is not a dir
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EBADF on relative `path` when `dirfd` isn't open or `AT_FDCWD`
 * @raise ELOOP if a loop was detected resolving parent components
 * @asyncsignalsafe
 */
ssize_t readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz) {
  ssize_t bytes;
  if ((bufsiz && !buf) || (IsAsan() && (!__asan_is_valid_str(path) ||
                                        !__asan_is_valid(buf, bufsiz)))) {
    bytes = efault();
  } else if (_weaken(__zipos_notat) &&
             (bytes = __zipos_notat(dirfd, path)) == -1) {
    STRACE("TODO: zipos support for readlinkat");
  } else if (!IsWindows()) {
    bytes = sys_readlinkat(dirfd, path, buf, bufsiz);
  } else {
    bytes = sys_readlinkat_nt(dirfd, path, buf, bufsiz);
  }
  STRACE("readlinkat(%s, %#s, [%#.*s]) → %d% m", DescribeDirfd(dirfd), path,
         (int)MAX(0, bytes), buf, bytes);
  return bytes;
}
