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
#include "libc/dce.h"
#include "libc/calls/internal.h"
#include "libc/calls/calls.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/consts/s.h"

/**
 * Creates filesystem inode.
 *
 * @param mode is octal mode, e.g. 0600; needs to be or'd with one of:
 *     S_IFDIR: directory
 *     S_IFIFO: named pipe
 *     S_IFREG: regular file
 *    S_IFSOCK: named socket
 *     S_IFBLK: block device (root has authorization)
 *     S_IFCHR: character device (root has authorization)
 * @param dev it's complicated
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int mknod(const char *path, uint32_t mode, uint64_t dev) {
  if (mode & S_IFREG) return creat(path, mode & ~S_IFREG);
  if (mode & S_IFDIR) return mkdir(path, mode & ~S_IFDIR);
  if (mode & S_IFIFO) return mkfifo(path, mode & ~S_IFIFO);
  if (!IsWindows()) {
    /* TODO(jart): Whys there code out there w/ S_xxx passed via dev? */
    return mknod$sysv(path, mode, dev);
  } else {
    return enosys();
  }
}
