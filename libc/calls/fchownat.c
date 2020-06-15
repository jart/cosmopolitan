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

/**
 * Changes owner and/or group of pathname.
 *
 * @param dirfd is open()'d relative-to directory, or AT_FDCWD, etc.
 * @param uid is user id, or -1 to not change
 * @param gid is group id, or -1 to not change
 * @param flags can have AT_SYMLINK_NOFOLLOW, etc.
 * @return 0 on success, or -1 w/ errno
 * @see chown(), lchown() for shorthand notation
 * @see /etc/passwd for user ids
 * @see /etc/group for group ids
 * @asyncsignalsafe
 */
int fchownat(int dirfd, const char *pathname, uint32_t uid, uint32_t gid,
             uint32_t flags) {
  /* TODO(jart): Windows? */
  return fchownat$sysv(dirfd, pathname, uid, gid, flags);
}
