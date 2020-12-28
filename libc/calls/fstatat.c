/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"

/**
 * Performs stat() w/ features for threaded apps.
 *
 * @param dirfd can be AT_FDCWD or an open directory descriptor, and is
 *     ignored if pathname is absolute
 * @param flags can have AT_{EMPTY_PATH,NO_AUTOMOUNT,SYMLINK_NOFOLLOW}
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int fstatat(int dirfd, const char *pathname, struct stat *st, uint32_t flags) {
  int olderr = errno;
  int rc = fstatat$sysv(dirfd, pathname, st, flags);
  if (rc != -1) {
    stat2linux(st);
  } else if (errno == ENOSYS && dirfd == AT_FDCWD) {
    if (!flags) {
      errno = olderr;
      rc = stat(pathname, st);
    } else if (flags == AT_SYMLINK_NOFOLLOW) {
      errno = olderr;
      rc = lstat(pathname, st);
    }
  }
  return rc;
}
