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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/errfuns.h"

/**
 * Reads symbolic link.
 *
 * This does *not* nul-terminate the buffer.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     file is a relative path, then file is opened relative to dirfd
 * @param path must be a symbolic link pathname
 * @param buf will receive symbolic link contents; it won't be modified
 *     unless the function succeeds; this buffer is never nul-terminated
 * @param bufsiz specifies how many bytes are available at `buf`
 * @return number of bytes written to buf, or -1 w/ errno; if the
 *     return is equal to bufsiz then truncation may have occurred
 * @raise EINVAL if path isn't a symbolic link
 * @raise ENOENT if `path` didn't exist
 * @raise ENOENT if `path` was an empty string
 * @raise EFAULT if `path` or `buf` pointed to bad memory
 * @raise ENOTDIR if parent component existed that's not a directory
 * @raise ENOTDIR if base component ends with slash and is not a dir
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EBADF on relative `path` when `dirfd` isn't open or `AT_FDCWD`
 * @raise ELOOP if a loop was detected resolving parent components
 * @raise ENOMEM if insufficient memory was available to the impl
 * @asyncsignalsafe
 */
ssize_t readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz) {
  char mybuf[1];
  ssize_t bytes;
  if (IsLinux() && !bufsiz) {
    // the linux kernel will einval if bufsiz is zero. linux is the only
    // os that does this. it's much simpler if we reserve einval for the
    // important thing posix specifies, which is path not being symlink.
    buf = mybuf;
    bufsiz = 1;
  }
  if (kisdangerous(path)) {
    bytes = efault();
  } else if (_weaken(__zipos_notat) &&
             (bytes = __zipos_notat(dirfd, path)) == -1) {
    STRACE("TODO: zipos support for readlinkat");
    bytes = einval();
  } else if (!IsWindows()) {
    bytes = sys_readlinkat(dirfd, path, buf, bufsiz);
  } else {
    bytes = sys_readlinkat_nt(dirfd, path, buf, bufsiz);
  }
  if (IsLinux() && buf == mybuf && bytes == 1)
    bytes = 0;
  STRACE("readlinkat(%s, %#s, [%#.*s]) → %d% m", DescribeDirfd(dirfd), path,
         (int)MAX(0, bytes), buf, bytes);
  return bytes;
}
