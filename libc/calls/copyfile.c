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
#include "libc/nt/files.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"

int copyfile(const char *frompath, const char *topath, bool dontoverwrite) {
  if (IsWindows()) {
    char16_t frompath16[PATH_MAX], topath16[PATH_MAX];
    if (mkntpath(frompath, frompath16) == -1) return -1;
    if (mkntpath(topath, topath16) == -1) return -1;
    if (CopyFile(frompath16, topath16, dontoverwrite)) {
      return 0;
    } else {
      return winerr();
    }
  } else {
    struct stat st;
    ssize_t transferred;
    int rc, fromfd, tofd;
    int64_t inoffset, outoffset;
    rc = -1;
    if ((fromfd = openat$sysv(AT_FDCWD, frompath, O_RDONLY, 0)) != -1) {
      if (fstat$sysv(fromfd, &st) != -1 &&
          (tofd = openat$sysv(AT_FDCWD, topath,
                              O_WRONLY | O_CREAT | (dontoverwrite ? O_EXCL : 0),
                              st.st_mode & 0777)) != -1) {
        inoffset = 0;
        outoffset = 0;
        while (st.st_size && (transferred = copy_file_range(
                                  fromfd, &inoffset, tofd, &outoffset,
                                  st.st_size, 0)) != -1) {
          st.st_size -= transferred;
        }
        if (!st.st_size) rc = 0;
        rc |= close$sysv(tofd);
      }
      rc |= close$sysv(fromfd);
    }
    return rc;
  }
}
