/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/weaken.h"
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"
#include "libc/time/time.h"
#include "libc/zipos/zipos.internal.h"

#define __NR_utimensat_linux 0x118 /*RHEL5:CVE-2010-3301*/

int sys_utimensat(int dirfd, const char *path, const struct timespec ts[2],
                  int flags) {
  int rc, olderr;
  struct timeval tv[2];
  if (weaken(__zipos_notat) && weaken(__zipos_notat)(dirfd, path) == -1) {
    return -1; /* TODO(jart): implement me */
  }
  if (!IsXnu()) {
    olderr = errno;
    rc = __sys_utimensat(dirfd, path, ts, flags);
    if (((rc == -1 && errno == ENOSYS) || rc == __NR_utimensat_linux) &&
        dirfd == AT_FDCWD && !flags) {
      errno = olderr;
      if (ts) {
        tv[0].tv_sec = ts[0].tv_sec;
        tv[0].tv_usec = ts[0].tv_nsec / 1000;
        tv[1].tv_sec = ts[1].tv_sec;
        tv[1].tv_usec = ts[1].tv_nsec / 1000;
        rc = sys_utimes(path, tv);
      } else {
        rc = sys_utimes(path, NULL);
      }
    }
    return rc;
  } else {
    return sys_utimensat_xnu(dirfd, path, ts, flags);
  }
}
