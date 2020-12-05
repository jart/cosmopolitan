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
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/files.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns true if file exists at path.
 *
 * Please note that things which aren't strictly files, e.g. directories
 * or sockets, could be considered files for the purposes of this
 * function. The stat() function may be used to differentiate them.
 */
bool fileexists(const char *path) {
  int rc, olderr;
  struct stat st;
  uint16_t path16[PATH_MAX];
  if (!IsWindows()) {
    olderr = errno;
    rc = stat(path, &st);
    if (rc == -1 && (errno == ENOENT || errno == ENOTDIR)) {
      errno = olderr;
    }
    return rc != -1;
  } else {
    if (__mkntpath(path, path16) == -1) return -1;
    return GetFileAttributes(path16) != -1u;
  }
}
