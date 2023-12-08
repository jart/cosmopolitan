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
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

static textwindows int sys_utimensat_nt_impl(int dirfd, const char *path,
                                             const struct timespec ts[2],
                                             int flags) {
  int i, rc;
  int64_t fh, closeme;
  uint16_t path16[PATH_MAX];
  struct NtFileTime ft[2], *ftp[2];

  if (path) {
    if (__mkntpathat(dirfd, path, 0, path16) == -1) return -1;
    if ((fh = CreateFile(
             path16, kNtFileWriteAttributes,
             kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, NULL,
             kNtOpenExisting,
             kNtFileAttributeNormal | kNtFileFlagBackupSemantics |
                 ((flags & AT_SYMLINK_NOFOLLOW) ? kNtFileFlagOpenReparsePoint
                                                : 0),
             0)) != -1) {
      closeme = fh;
    } else {
      return __winerr();
    }
  } else if (__isfdkind(dirfd, kFdFile)) {
    fh = g_fds.p[dirfd].handle;
    closeme = -1;
  } else {
    return ebadf();
  }

  if (!ts || ts[0].tv_nsec == UTIME_NOW || ts[1].tv_nsec == UTIME_NOW) {
    GetSystemTimePreciseAsFileTime(ft);
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
    rc = __winerr();
  }

  if (closeme != -1) {
    CloseHandle(fh);
  }

  return rc;
}

textwindows int sys_utimensat_nt(int dirfd, const char *path,
                                 const struct timespec ts[2], int flags) {
  int rc;
  BLOCK_SIGNALS;
  rc = sys_utimensat_nt_impl(dirfd, path, ts, flags);
  ALLOW_SIGNALS;
  return rc;
}
