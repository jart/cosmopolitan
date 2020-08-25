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
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens file, the modern way.
 *
 * @param dirfd is normally AT_FDCWD or an open relative directory thing
 * @param file is a UTF-8 string, preferably relative w/ forward slashes
 * @param flags should be O_RDONLY, O_WRONLY, or O_RDWR, and can be or'd
 *     with O_CREAT, O_TRUNC, O_APPEND, O_EXCL, O_CLOEXEC, O_TMPFILE
 * @param mode is an octal user/group/other permission signifier, that's
 *     ignored if O_CREAT or O_TMPFILE weren't passed
 * @return number needing close(), or -1 w/ errno
 * @asyncsignalsafe
 */
int openat(int dirfd, const char *pathname, int flags, ...) {
  va_list va;
  unsigned mode;
  va_start(va, flags);
  mode = va_arg(va, unsigned);
  va_end(va);
  if (!pathname) return efault();
  if (dirfd == AT_FDCWD) {
    return open(pathname, flags, mode);
  } else {
    return openat$sysv(dirfd, pathname, flags, mode);
  }
}
