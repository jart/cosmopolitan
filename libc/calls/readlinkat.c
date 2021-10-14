/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Reads symbolic link.
 *
 * This does *not* nul-terminate the buffer.
 *
 * It is recommended that malloc() be linked into your program when
 * using this function. Otherwise the buffer should be larger. It should
 * also be noted that, without malloc, long names with many astral plane
 * characters might not decode properly.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     file is a relative path, then file is opened relative to dirfd
 * @param path must be a symbolic link pathname
 * @param buf will receive symbolic link contents, and won't be modified
 *     unless the function succeeds (with the exception of no-malloc nt)
 * @return number of bytes written to buf, or -1 w/ errno; if the
 *     return is equal to bufsiz then truncation may have occurred
 * @error EINVAL if path isn't a symbolic link
 * @asyncsignalsafe
 */
ssize_t readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz) {
  ssize_t bytes;
  struct ZiposUri zipname;
  if ((IsAsan() && !__asan_is_valid(buf, bufsiz)) || (bufsiz && !buf)) {
    bytes = efault();
  } else if (weaken(__zipos_notat) && __zipos_notat(dirfd, path) == -1) {
    SYSDEBUG("TOOD: zipos support for readlinkat");
    bytes = enosys(); /* TODO(jart): code me */
  } else if (!IsWindows()) {
    bytes = sys_readlinkat(dirfd, path, buf, bufsiz);
  } else {
    bytes = sys_readlinkat_nt(dirfd, path, buf, bufsiz);
  }
  SYSDEBUG("readlinkat(%d, %s, 0x%p, 0x%x) -> %d %s", (long)dirfd, path, buf,
           bufsiz, bytes, bytes != -1 ? "" : strerror(errno));
  return bytes;
}
