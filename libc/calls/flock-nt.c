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
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/errfuns.h"

textwindows int flock$nt(int fd, int op) {
  struct NtOverlapped ov;
  struct NtByHandleFileInformation info;
  if (!__isfdkind(fd, kFdFile)) return ebadf();
  memset(&ov, 0, sizeof(ov));
  if (GetFileInformationByHandle(g_fds.p[fd].handle, &info) &&
      ((!(op & LOCK_UN) &&
        LockFileEx(g_fds.p[fd].handle, op, 0, info.nFileSizeLow,
                   info.nFileSizeHigh, &ov)) ||
       ((op & LOCK_UN) && UnlockFileEx(g_fds.p[fd].handle, 0, info.nFileSizeLow,
                                       info.nFileSizeHigh, &ov)))) {
    return 0;
  } else {
    return __winerr();
  }
}
