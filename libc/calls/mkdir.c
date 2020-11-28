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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

static textwindows noinline int mkdir$nt(const char *path, uint32_t mode) {
  uint16_t path16[PATH_MAX];
  if (mkntpath(path, path16) == -1) return -1;
  if (CreateDirectory(path16, NULL)) {
    return 0;
  } else {
    return __winerr();
  }
}

/**
 * Creates directory a.k.a. folder.
 *
 * @param path is a UTF-8 string, preferably relative w/ forward slashes
 * @param mode can be, for example, 0755
 * @return 0 on success or -1 w/ errno
 * @error EEXIST, ENOTDIR, ENAMETOOLONG, EACCES
 * @asyncsignalsafe
 */
int mkdir(const char *path, uint32_t mode) {
  if (!path) return efault();
  if (!IsWindows()) {
    return mkdirat$sysv(AT_FDCWD, path, mode);
  } else {
    return mkdir$nt(path, mode);
  }
}
