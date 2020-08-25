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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

int utimensat$xnu(int dirfd, const char *path, const struct timespec ts[2],
                  int flags) {
  int i;
  struct timeval now, tv[2];
  if (flags) return einval();
  if (!ts || ts[0].tv_nsec == UTIME_NOW || ts[1].tv_nsec == UTIME_NOW) {
    gettimeofday(&now, NULL);
  }
  if (ts) {
    for (i = 0; i < 2; ++i) {
      if (ts[i].tv_nsec == UTIME_NOW) {
        tv[i] = now;
      } else if (ts[i].tv_nsec == UTIME_OMIT) {
        return einval();
      } else {
        tv[i].tv_sec = ts[i].tv_sec;
        tv[i].tv_usec = div1000int64(ts[i].tv_nsec);
      }
    }
  } else {
    tv[0] = now;
    tv[1] = now;
  }
  if (path) {
    if (dirfd == AT_FDCWD) {
      return utimes$sysv(path, tv);
    } else {
      return enosys();
    }
  } else {
    if (dirfd != AT_FDCWD) {
      return futimes$sysv(dirfd, tv);
    } else {
      return einval();
    }
  }
}
