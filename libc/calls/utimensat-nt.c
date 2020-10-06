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
#include "libc/conv/conv.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

textwindows int utimensat$nt(int dirfd, const char *path,
                             const struct timespec ts[2], int flags) {
  int i, rc;
  int64_t fh;
  bool closeme;
  uint16_t path16[PATH_MAX];
  struct NtFileTime ft[2], *ftp[2];
  if (flags) return einval();
  if (path) {
    if (dirfd == AT_FDCWD) {
      if (mkntpath(path, path16) == -1) return -1;
      if ((fh = CreateFile(path16, kNtFileWriteAttributes, kNtFileShareRead,
                           NULL, kNtOpenExisting, kNtFileAttributeNormal, 0)) !=
          -1) {
        closeme = true;
      } else {
        return winerr();
      }
    } else {
      return einval();
    }
  } else {
    return ebadf();
  }
  if (!ts || ts[0].tv_nsec == UTIME_NOW || ts[1].tv_nsec == UTIME_NOW) {
    GetSystemTimeAsFileTime(ft);
  }
  if (ts) {
    for (i = 0; i < 2; ++i) {
      if (ts[i].tv_nsec == UTIME_NOW) {
        ftp[i] = ft;
      } else if (ts[i].tv_nsec == UTIME_OMIT) {
        ftp[i] = NULL;
      } else {
        ft[i] = TimeSpecToFileTime(ts[i]);
        ftp[i] = &ft[i];
      }
    }
  } else {
    ftp[0] = ft;
    ftp[1] = ft;
  }
  if (SetFileTime(fh, NULL, ftp[0], ftp[1])) {
    rc = 0;
  } else {
    rc = winerr();
  }
  if (closeme) {
    CloseHandle(fh);
  }
  return rc;
}
