/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"

int sys_utimensat_old(int dirfd, const char *path, const struct timespec ts[2],
                      int flags) {
  char buf[PATH_MAX + 1];

  // validate usage
  if (path) {
    if (dirfd != AT_FDCWD) {
      return enotsup();
    }
  } else {
    unassert(dirfd >= 0);
  }
  if (flags) {
    return einval();
  }

  // xnu has futimes(), but rhel5 only has utimes()
  // on linux we'll try to get the path from procfs
  if (IsLinux() && !path) {
    char procpath[36];
    char *p = procpath;
    p = stpcpy(p, "/proc/");
    p = FormatInt32(p, getpid());
    p = stpcpy(p, "/fd/");
    p = FormatInt32(p, dirfd);
    ssize_t got;
    if ((got = readlink(procpath, buf, sizeof(buf))) == -1) {
      return -1;
    }
    if (got == sizeof(buf)) {
      return enametoolong();
    }
    unassert(buf[0] == '/');
    buf[got] = 0;
    path = buf;
  }

  // perform preliminary system calls ahead of time
  struct timeval now;
  if (!ts || ts[0].tv_nsec == UTIME_NOW || ts[1].tv_nsec == UTIME_NOW) {
    unassert(!gettimeofday(&now, 0));
  }
  struct stat st;
  if (ts && (ts[0].tv_nsec == UTIME_OMIT || ts[1].tv_nsec == UTIME_OMIT)) {
    if (path) {
      if (sys_fstatat(AT_FDCWD, path, &st, 0) == -1) {
        return -1;
      }
    } else {
      if (sys_fstat(dirfd, &st) == -1) {
        return -1;
      }
    }
  }

  // change the timestamps
  struct timeval tv[2];
  if (ts) {
    if (ts[0].tv_nsec == UTIME_NOW) {
      tv[0] = now;
    } else if (ts[0].tv_nsec == UTIME_OMIT) {
      tv[0] = timespec_totimeval(st.st_atim);
    } else {
      tv[0] = timespec_totimeval(ts[0]);
    }
    if (ts[1].tv_nsec == UTIME_NOW) {
      tv[1] = now;
    } else if (ts[1].tv_nsec == UTIME_OMIT) {
      tv[1] = timespec_totimeval(st.st_mtim);
    } else {
      tv[1] = timespec_totimeval(ts[1]);
    }
  } else {
    tv[0] = now;
    tv[1] = now;
  }

  // apply the new timestamps
  if (path) {
    return sys_utimes(path, tv);
  } else {
    return sys_futimes(dirfd, tv);
  }
}
