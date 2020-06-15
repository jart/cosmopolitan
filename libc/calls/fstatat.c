/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
