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
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates symbolic link.
 *
 * This is like link() but adds a tiny indirection to make the fact that
 * the file is a link obvious. It also enables certain other features,
 * like the ability to be broken.
 *
 * @param target can be relative and needn't exist
 * @param linkpath is what gets created
 * @return 0 on success, or -1 w/ errno
 * @note Windows NT only lets admins do this
 * @asyncsignalsafe
 */
int symlink(const char *target, const char *linkpath) {
  if (!target || !linkpath) return efault();
  if (!IsWindows()) {
    return symlinkat$sysv(target, AT_FDCWD, linkpath);
  } else {
    return symlink$nt(target, linkpath);
  }
}
