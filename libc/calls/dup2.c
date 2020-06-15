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
#include "libc/dce.h"

/**
 * Duplicates file descriptor, granting it specific number.
 *
 * @param oldfd isn't closed afterwards
 * @param newfd if already assigned, is silently closed beforehand;
 *     unless it's equal to oldfd, in which case dup2() is a no-op
 * @return new file descriptor, or -1 w/ errno
 * @asyncsignalsafe
 */
int dup2(int oldfd, int newfd) {
  if (oldfd == newfd) return newfd;
  if (!IsWindows()) {
    return dup3$sysv(oldfd, newfd, 0);
  } else {
    return dup$nt(oldfd, newfd, 0);
  }
}
